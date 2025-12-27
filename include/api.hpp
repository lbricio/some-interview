#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <boost/json.hpp>

#include "http_client.hpp"
#include "utils.hpp"
#include "models.hpp"

class RickAndMortyApi {
public:
    explicit RickAndMortyApi(HttpClient& client);
    Episode get_episode(int id);
    Character get_character(int id);
    bool has_character_cached(int id) const;
    std::string route_query(const std::string& target);
    std::vector<Character> get_characters_page(int page);
    std::vector<std::pair<int, std::string>> get_all_characters_basic();
    std::vector<Character> get_all_characters();
    std::vector<Character> get_characters_by_ids(const std::vector<int>& ids);
    
    std::string get_episode_all();
    std::string get_episode_single(int id);
    std::string get_episode_batch(const std::string& id_part);
    std::string get_episode_query(const std::string& full_target);
    
private:
    Character parse_character(const boost::json::object& obj);
    HttpClient& client_;
    std::unordered_map<int, Character> character_cache_;
};
