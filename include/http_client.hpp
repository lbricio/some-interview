#pragma once
#include <string>

class HttpClient {
public:
    explicit HttpClient(bool verbose = false);
    std::string get(const std::string& host, const std::string& target);
private:
    bool verbose_;
};
