#define BOOST_SYSTEM_NO_LIB
#define BOOST_JSON_STANDALONE

#include <boost/json/src.hpp>
#include <boost/json.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>

#include <iostream>
#include "http_client.hpp"
#include "api.hpp"
#include "utils.hpp"

template<class F>
auto with_retry(F&& f, int retries = 3) {
    for (int i = 0; i < retries; ++i) {
        try { return f(); }
        catch (...) { if (i == retries-1) throw; }
    }
    throw std::runtime_error("retry failed");
}

int main() {
    HttpClient client(true);
    RickAndMortyApi api(client);

    try {
        auto ep = with_retry([&]{ return api.get_episode(28); });

        std::cout << "Name: "     << ep.name << "\n";
        std::cout << "Episode: "  << ep.episode << "\n";
        std::cout << "Air date: " << ep.air_date << "\n\n";

        for (auto const& name : ep.characters) {
            std::cout << name << "\n";
        }
    } catch (std::exception const& ex) {
        std::cerr << "Erro: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
