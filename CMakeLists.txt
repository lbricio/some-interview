cmake_minimum_required(VERSION 3.18)
project(RickMiddleware)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(GTest REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(app
    main.cpp
    src/http_client.cpp
    src/api.cpp
    src/utils.cpp
	src/session.cpp
)

target_include_directories(app PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(app PRIVATE
    Boost::boost
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    pthread
)

message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Boost include: ${Boost_INCLUDE_DIRS}")
message(STATUS "OpenSSL include: ${OPENSSL_INCLUDE_DIR}")

enable_testing()

add_executable(tests
    tests/test_main.cpp
	tests/test_endpoint.cpp
    src/utils.cpp
    src/api.cpp
    src/http_client.cpp
    src/session.cpp
	src/http_client.cpp
)

target_include_directories(tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    Boost::boost
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    pthread
)

add_test(NAME run_tests COMMAND tests)
