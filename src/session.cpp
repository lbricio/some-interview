#include "session.hpp"
#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>

#include "api.hpp"
#include "utils.hpp"

namespace beast = boost::beast;
namespace http  = beast::http;
namespace json  = boost::json;

template<class F>
auto with_retry(F&& f, int retries = 3) {
    for (int i = 0; i < retries; ++i) {
        try { return f(); }
        catch (...) {
            if (i == retries - 1) throw;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
    throw std::runtime_error("retry failed");
}

Session::Session(beast::tcp_stream& stream, RickAndMortyApi& api)
    : stream_(stream), api_(api) {}

void Session::send_response(http::status status,
                           const std::string& body,
                           const http::request<http::string_body>& req) {
    http::response<http::string_body> res{status, req.version()};
    res.set(http::field::content_type, "application/json");
    res.body() = body;
    res.prepare_payload();
    http::write(stream_, res);
}

void Session::help(const http::request<http::string_body>& req) {
    json::object h;
    h["service"] = "RickAndMorty Middleware";
    json::array cmds{"help", "character/<id>", "character/all", "exit"};
    h["commands"] = cmds;
    send_response(http::status::ok, json::serialize(h), req);
}

void Session::character_all(const http::request<http::string_body>& req) {
    auto list = with_retry([&]{ return api_.get_all_characters_basic(); });

    json::array arr;
    for (auto const& [id, name] : list) {
        arr.push_back({{"id", id}, {"name", name}});
    }

    json::object out;
    out["characters"] = arr;

    send_response(http::status::ok, json::serialize(out), req);
}

void Session::character_single(int id, const http::request<http::string_body>& req) {
    auto c = with_retry([&]{ return api_.get_character(id); });

    json::object o;
    o["id"]       = c.id;
    o["name"]     = c.name;
    o["status"]   = c.status;
    o["species"]  = c.species;
    o["gender"]   = c.gender;
    o["origin"]   = c.origin_name;
    o["location"] = c.location_name;

    json::array eps;
    for (int eid : c.episode_ids)
        eps.push_back(eid);

    o["episodes"] = eps;

    send_response(http::status::ok, json::serialize(o), req);
}

void Session::character_batch(const std::string& id_part,
                             const http::request<http::string_body>& req) {
    for (char ch : id_part) {
        if (!::isdigit(ch) && ch != ',') {
            send_response(http::status::bad_request,
                          R"({"error":"IDs must be numeric and comma-separated"})", req);
            return;
        }
    }

    std::vector<int> ids;
    std::stringstream ss(id_part);
    std::string tok;
    while (std::getline(ss, tok, ',')) {
        if (!tok.empty())
            ids.push_back(std::stoi(tok));
    }

    json::array arr;
    for (int cid : ids) {
        Character c = api_.get_character(cid);
        arr.push_back({{"id", c.id}, {"name", c.name}});
    }

    json::object out;
    out["characters"] = arr;

    send_response(http::status::ok, json::serialize(out), req);
}

void Session::route_request(const std::string& path,
                           const http::request<http::string_body>& req) {
    if (path == "/help") {
        help(req);
        return;
    }

    if (path.starts_with("/character/all")) {
        character_all(req);
        return;
    }

    if (path.starts_with("/character/")) {
        std::string id_part = path.substr(strlen("/character/"));

        if (std::all_of(id_part.begin(), id_part.end(), ::isdigit)) {
            character_single(std::stoi(id_part), req);
            return;
        }

        if (id_part.find(',') != std::string::npos) {
            character_batch(id_part, req);
            return;
        }
    }

    send_response(http::status::not_found, R"({"error":"route not found"})", req);
}

void Session::handle() {
    try {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(stream_, buffer, req);

        std::string path(req.target());
        route_request(path, req);
    }
    catch(std::exception const& e) {
        json::object err{{"error", e.what()}};
        http::response<http::string_body> res{http::status::bad_request, 11};
        res.set(http::field::content_type, "application/json");
        res.body() = json::serialize(err);
        http::write(stream_, res);
    }
} 
