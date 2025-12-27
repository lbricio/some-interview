#include <gtest/gtest.h>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <sstream>
#include <string>

#include "session.hpp"
#include "api.hpp"
#include "utils.hpp"
#include "models.hpp"

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
namespace json  = boost::json;

std::string do_request(beast::tcp_stream& stream, const std::string& raw_req) {
    net::io_context ioc;
    net::ip::tcp::acceptor acceptor{ioc, {net::ip::tcp::v4(), 0}};
    auto port = acceptor.local_endpoint().port();

    net::ip::tcp::socket sock{ioc};
    sock.connect({net::ip::address_v4::loopback(), port});

    auto server_socket = acceptor.accept();
    beast::tcp_stream server_stream{std::move(server_socket)};

    net::write(sock, net::buffer(raw_req));

    char data[8192]{};
    size_t n = sock.read_some(net::buffer(data));
    std::string http_response(data, n);

    auto pos = http_response.find("\r\n\r\n");
    if (pos == std::string::npos)
        throw std::runtime_error("HTTP response malformed in test");

    return http_response.substr(pos + 4);
}

TEST(EndpointTest, EpisodeSingle) {
    net::io_context ioc;
    HttpClient client(false);
    RickAndMortyApi api(client);

    beast::tcp_stream stream{ioc};
    std::string raw_req = "GET /episode/28 HTTP/1.1\r\nHost: localhost\r\n\r\n";

    auto body = do_request(stream, raw_req);
    auto obj = json::parse(body).as_object();

    EXPECT_TRUE(obj.contains("id"));
    EXPECT_TRUE(obj.contains("name"));
    EXPECT_TRUE(obj.contains("episode"));
    EXPECT_TRUE(obj.contains("air_date"));

    EXPECT_EQ(obj.at("id").as_int64(), 28);
    EXPECT_FALSE(obj.at("name").as_string().empty());
}

TEST(EndpointTest, CharacterAll) {
    net::io_context ioc;
    HttpClient client(false);
    RickAndMortyApi api(client);

    beast::tcp_stream stream{ioc};
    std::string raw_req = "GET /character/all HTTP/1.1\r\nHost: localhost\r\n\r\n";

    auto body = do_request(stream, raw_req);
    auto obj = json::parse(body).as_object();

    EXPECT_TRUE(obj.contains("characters"));
    EXPECT_EQ(obj.at("characters").as_array().size(), 826);
}

TEST(EndpointTest, CharacterBatch) {
    net::io_context ioc;
    HttpClient client(false);
    RickAndMortyApi api(client);

    beast::tcp_stream stream{ioc};
    std::string raw_req = "GET /character/1,183 HTTP/1.1\r\nHost: localhost\r\n\r\n";

    auto body = do_request(stream, raw_req);
    auto obj  = json::parse(body).as_object();
    auto arr  = obj.at("characters").as_array();

    EXPECT_EQ(arr.size(), 2);

    std::vector<int> ids;
    for (auto const& v : arr)
        ids.push_back(v.as_object().at("id").as_int64());

    EXPECT_TRUE(
        (ids[0] == 1 && ids[1] == 183) ||
        (ids[0] == 183 && ids[1] == 1)
    );
}
