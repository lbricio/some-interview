#define BOOST_SYSTEM_NO_LIB
#define BOOST_JSON_STANDALONE

#include <gtest/gtest.h>
#include <iostream>

#include <boost/json/src.hpp>
#include <boost/json.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/url.hpp>
#include <sstream>

#include "session.hpp"
#include "models.hpp"
#include "utils.hpp"
#include "api.hpp"

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
namespace json  = boost::json;

TEST(UtilsTest, ParseHttpsUrl) {
    auto [host, target] = parse_https_url("https://rickandmortyapi.com/api/character/2");
    EXPECT_FALSE(host.empty());
    EXPECT_FALSE(target.empty());
}

TEST(ModelTest, CharacterStruct) {
    Character c;
    c.id = 10;
    c.name = "Test";
    EXPECT_EQ(c.id, 10);
    EXPECT_EQ(c.name, "Test");
}

class GlobalTestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        std::cout << "[Test] Inicializando suite de testes...\n";
    }
    void TearDown() override {
        std::cout << "[Test] Suite finalizada.\n";
    }
};

int main(int argc, char** argv) {
    ::testing::AddGlobalTestEnvironment(new GlobalTestEnvironment);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
