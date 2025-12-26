#define BOOST_SYSTEM_NO_LIB
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "http_client.hpp"
#include <iostream>

HttpClient::HttpClient(bool verbose) : verbose_(verbose) {}

std::string HttpClient::get(const std::string& host, const std::string& target) {
    namespace beast = boost::beast;
    namespace http  = beast::http;
    namespace net   = boost::asio;
    namespace ssl   = net::ssl;

    net::io_context ioc;
    ssl::context ctx(ssl::context::tlsv12_client);
    ctx.set_default_verify_paths();

    net::ip::tcp::resolver resolver(ioc);
    auto const results = resolver.resolve(host, "443");

    ssl::stream<beast::tcp_stream> stream(ioc, ctx);
    beast::get_lowest_layer(stream).connect(results);
    stream.handshake(ssl::stream_base::client);

    http::request<http::empty_body> req{http::verb::get, target, 11};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, "Boost.Beast Client");

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);

    beast::error_code ec;
    stream.shutdown(ec);

    if (verbose_) {
        std::cout << "[HTTP GET] " << host << target << "\n";
    }
    return res.body();
}
