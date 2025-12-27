#pragma once

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <string>
#include "api.hpp"

namespace beast = boost::beast;
namespace http  = beast::http;

class Session {
public:
    Session(beast::tcp_stream& stream, RickAndMortyApi& api);
    void handle();

private:
    void route_request(const std::string& path,
                       const http::request<http::string_body>& req);

    void help(const http::request<http::string_body>& req);
    void character_all(const http::request<http::string_body>& req);
    void character_single(int id, const http::request<http::string_body>& req);
    void character_batch(const std::string& id_part,
                         const http::request<http::string_body>& req);

    void send_response(http::status status,
                       const std::string& body,
                       const http::request<http::string_body>& req);

    beast::tcp_stream& stream_;
    RickAndMortyApi& api_;
};
