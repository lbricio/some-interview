cmake_minimum_required(VERSION 3.18)
project(RickMiddleware)

# 1. Definir padrão C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# 2. Importar dependências via Conan
# (O Conan já gerou o conan_toolchain.cmake, então REQUIRED basta)
find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)  # << adicionar aqui

add_executable(app
    main.cpp
    src/http_client.cpp
    src/api.cpp
    src/utils.cpp
)

target_include_directories(app PRIVATE
    ${CMAKE_SOURCE_DIR}/include  # << include do seu projeto
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

# 5. (Opcional) Se quiser logs do processo de build:
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Boost include: ${Boost_INCLUDE_DIRS}")
message(STATUS "OpenSSL include: ${OPENSSL_INCLUDE_DIR}")
