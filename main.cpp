#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/json/src.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
namespace ssl   = net::ssl;
namespace json  = boost::json;

int main() {
    try {
        net::io_context ioc;
        ssl::context ctx(ssl::context::tlsv12_client);
        {
            const std::string host   = "rickandmortyapi.com";
            const std::string target = "/api/episode/28";

            net::ip::tcp::resolver resolver(ioc);
            auto results = resolver.resolve(host, "443");

            beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);
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

            // =======================
            // 2. PARSE DO JSON
            // =======================

            json::value jv = json::parse(res.body());
            json::object obj = jv.as_object();

            std::cout << "Name: "     << obj["name"].as_string()     << "\n";
            std::cout << "Episode: "  << obj["episode"].as_string()  << "\n";
            std::cout << "Air date: " << obj["air_date"].as_string() << "\n\n";

            // =======================
            // 3. EXTRAI URLs DOS PERSONAGENS
            // =======================

            std::vector<std::string> character_urls;
            const auto& arr = obj["characters"].as_array();
            character_urls.reserve(arr.size());

            for (const auto& v : arr) {
                character_urls.emplace_back(v.as_string().c_str());
            }

            // =======================
            // 4. PARA CADA PERSONAGEM, BUSCAR O NOME
            // =======================

			std::vector<std::string> characters;

            for (const auto& url : character_urls) {

                // parse simples da URL
                const std::string https = "https://";
                auto host_start = https.size();
                auto path_start = url.find('/', host_start);

                std::string char_host   = url.substr(host_start, path_start - host_start);
                std::string char_target = url.substr(path_start);

                net::ip::tcp::resolver char_resolver(ioc);
                auto char_results = char_resolver.resolve(char_host, "443");

                beast::ssl_stream<beast::tcp_stream> char_stream(ioc, ctx);
                beast::get_lowest_layer(char_stream).connect(char_results);
                char_stream.handshake(ssl::stream_base::client);

                http::request<http::empty_body> char_req{http::verb::get, char_target, 11};
                char_req.set(http::field::host, char_host);
                char_req.set(http::field::user_agent, "Boost.Beast Client");

                http::write(char_stream, char_req);

                beast::flat_buffer char_buffer;
                http::response<http::string_body> char_res;
                http::read(char_stream, char_buffer, char_res);

                beast::error_code char_ec;
                char_stream.shutdown(char_ec);

                json::value cjv = json::parse(char_res.body());
                json::object cobj = cjv.as_object();

				characters.push_back(cobj["name"].as_string().c_str());
            }

			sort(characters.begin(), characters.end());
			for (const auto &character : characters) {
				std::cout << character << ", " << std::endl;
			}
			std::cout << std::endl;
        }

    } catch (const std::exception& ex) {
        std::cerr << "Erro: " << ex.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
