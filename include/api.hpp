#pragma once
#include <string>
#include <vector>
#include "http_client.hpp"

struct Episode {
    std::string name;
    std::string episode;
    std::string air_date;
    std::vector<std::string> characters;
};

class RickAndMortyApi {
public:
    explicit RickAndMortyApi(HttpClient& client);
    Episode get_episode(int id);
private:
    HttpClient& client_;
};
