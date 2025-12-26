#define BOOST_SYSTEM_NO_LIB
#define BOOST_JSON_STANDALONE
#include <boost/json.hpp>

#include "api.hpp"
#include "utils.hpp"
#include <algorithm>

namespace json = boost::json;

RickAndMortyApi::RickAndMortyApi(HttpClient& client) : client_(client) {}

Episode RickAndMortyApi::get_episode(int id) {
    Episode ep;
    const std::string host   = "rickandmortyapi.com";
    const std::string target = "/api/episode/" + std::to_string(id);

    auto body = client_.get(host, target);
    json::object obj = json::parse(body).as_object();

    ep.name      = std::string(obj["name"].as_string().c_str());
    ep.episode   = std::string(obj["episode"].as_string().c_str());
    ep.air_date  = std::string(obj["air_date"].as_string().c_str());

    for (auto const& v : obj["characters"].as_array()) {
        ep.characters.emplace_back(v.as_string().c_str());
    }

    std::vector<std::string> names;
    names.reserve(ep.characters.size());

    for (auto const& url : ep.characters) {
        auto [h, t] = parse_https_url(url);
        auto cbody  = client_.get(h, t);
        json::object cobj = json::parse(cbody).as_object();
        names.emplace_back(cobj["name"].as_string().c_str());
    }

    std::sort(names.begin(), names.end());
    ep.characters = std::move(names);
    return ep;
}
