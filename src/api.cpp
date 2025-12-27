#include "api.hpp"
#include "utils.hpp"
#include "models.hpp"
#include <boost/json.hpp>
#include <algorithm>

namespace json = boost::json;

RickAndMortyApi::RickAndMortyApi(HttpClient& client) : client_(client) {}

Character RickAndMortyApi::parse_character(const json::object& obj) {
    Character c;
    c.id      = obj.at("id").as_int64();
    c.name    = obj.at("name").as_string().c_str();
    c.status  = obj.at("status").as_string().c_str();
    c.species = obj.at("species").as_string().c_str();
    c.gender  = obj.at("gender").as_string().c_str();
    c.origin_name   = obj.at("origin").as_object().at("name").as_string().c_str();
    c.location_name = obj.at("location").as_object().at("name").as_string().c_str();

    for (auto const& epv : obj.at("episode").as_array()) {
        auto url = std::string(epv.as_string().c_str());
        auto pos = url.find_last_of('/');
        if (pos != std::string::npos)
            c.episode_ids.push_back(std::stoi(url.substr(pos+1)));
    }
    return c;
}

Character RickAndMortyApi::get_character(int id) {
    if (auto it = character_cache_.find(id); it != character_cache_.end()) {
        return it->second;
    }

    const std::string host = "rickandmortyapi.com";
    const std::string target = "/api/character/" + std::to_string(id);

    auto body = client_.get(host, target);
    auto obj  = json::parse(body).as_object();

    Character c = parse_character(obj);
    character_cache_.emplace(c.id, c);

    return c;
}

std::vector<Character> RickAndMortyApi::get_characters_page(int page) {
    const std::string host = "rickandmortyapi.com";
    const std::string target = "/api/character?page=" + std::to_string(page);

    auto body = client_.get(host, target);
    auto root = json::parse(body).as_object();
    auto arr  = root.at("results").as_array();

    std::vector<Character> out;
    out.reserve(arr.size());

    for (auto const& v : arr) {
        auto cobj = v.as_object();
        int cid   = cobj.at("id").as_int64();

        if (auto it = character_cache_.find(cid); it != character_cache_.end()) {
            out.push_back(it->second);
        } else {
            Character c = parse_character(cobj);
            character_cache_.emplace(c.id, c);
            out.push_back(c);
        }
    }
    return out;
}

bool RickAndMortyApi::has_character_cached(int id) const {
    return character_cache_.find(id) != character_cache_.end();
}

Episode RickAndMortyApi::get_episode(int id) {
    const std::string host = "rickandmortyapi.com";
    const std::string target = "/api/episode/" + std::to_string(id);

    auto body = client_.get(host, target);
    auto obj  = json::parse(body).as_object();

    Episode ep;
    ep.name     = std::string(obj.at("name").as_string().c_str());
    ep.episode  = std::string(obj.at("episode").as_string().c_str());
    ep.air_date = std::string(obj.at("air_date").as_string().c_str());

    for (auto const& u : obj.at("characters").as_array()) {
        auto url  = std::string(u.as_string().c_str());
        auto pos  = url.find_last_of('/');
        int cid   = std::stoi(url.substr(pos + 1));

        if (auto it = character_cache_.find(cid); it != character_cache_.end()) {
            ep.characters.push_back(it->second.name);
        } else {
            ep.characters.push_back(get_character(cid).name);
        }
    }

    std::sort(ep.characters.begin(), ep.characters.end());
    return ep;
}

std::string RickAndMortyApi::get_episode_all() {
    const std::string upstream = "/api/episode";
    return client_.get("rickandmortyapi.com", upstream);
}

std::string RickAndMortyApi::get_episode_single(int id) {
    const std::string upstream = "/api/episode/" + std::to_string(id);
    return client_.get("rickandmortyapi.com", upstream);
}

std::string RickAndMortyApi::get_episode_batch(const std::string& id_part) {
    const std::string upstream = "/api/episode/" + id_part;
    return client_.get("rickandmortyapi.com", upstream);
}

std::string RickAndMortyApi::get_episode_query(const std::string& full_target) {
    const std::string upstream = "/api/episode";
    return client_.get("rickandmortyapi.com", upstream + full_target);
}

std::vector<std::pair<int, std::string>> RickAndMortyApi::get_all_characters_basic() {
    const std::string host = "rickandmortyapi.com";
    std::string target = "/api/character";

    std::vector<std::pair<int, std::string>> basic_list;
    basic_list.reserve(826);

    while (!target.empty()) {
        auto body = client_.get(host, target);
        auto root = json::parse(body).as_object();

        for (auto const& v : root.at("results").as_array()) {
            auto obj = v.as_object();
            int id = obj.at("id").as_int64();
            std::string name = obj.at("name").as_string().c_str();

            basic_list.emplace_back(id, name);

            if (!character_cache_.contains(id)) {
                Character full = parse_character(obj);
                character_cache_.emplace(id, std::move(full));
            }
        }

        auto const& info = root.at("info").as_object();
        if (info.at("next").is_null()) {
            target.clear();
        } else {
            std::string next = info.at("next").as_string().c_str();
            auto pos = next.find("/api/character");
            target = (pos == std::string::npos) ? "" : next.substr(pos);
        }
    }

    return basic_list;
}

std::vector<Character> RickAndMortyApi::get_characters_by_ids(const std::vector<int>& ids) {
    if (ids.empty()) return {};

    std::string id_list = std::to_string(ids[0]);
    for (size_t i = 1; i < ids.size(); ++i) {
        id_list += "," + std::to_string(ids[i]);
    }

    const std::string host   = "rickandmortyapi.com";
    const std::string target = "/api/character/" + id_list;

    auto body = client_.get(host, target);
    json::value jv   = json::parse(body);
    std::vector<Character> out;

    if (jv.is_object()) {
        auto obj = jv.as_object();
        Character c = parse_character(obj);
        character_cache_.try_emplace(c.id, c);
        out.push_back(std::move(c));
    }
    else if (jv.is_array()) {
        for (auto const& v : jv.as_array()) {
            auto obj = v.as_object();
            Character c = parse_character(obj);
            character_cache_.try_emplace(c.id, c);
            out.push_back(std::move(c));
        }
    }
    return out;
}

std::string RickAndMortyApi::route_query(const std::string& target) {
    const std::string host = "rickandmortyapi.com";
    return client_.get(host, target);
}
