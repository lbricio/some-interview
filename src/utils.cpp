#include "utils.hpp"

std::pair<std::string, std::string> parse_https_url(const std::string& url) {
    const std::string https = "https://";
    auto host_start = https.size();
    auto path_start = url.find('/', host_start);
    std::string host   = url.substr(host_start, path_start - host_start);
    std::string target = url.substr(path_start);
    return {host, target};
}
