#pragma once
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

namespace json_utils {

inline nlohmann::json read_json_file(const std::string& file_name) {
    std::string full_name = file_name + ".json";
    std::ifstream ifs(full_name);
    if (!ifs.is_open()) {
        throw std::runtime_error("Could not open JSON file: " + full_name);
    }

    try {
        nlohmann::json data;
        ifs >> data;
        return data;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Error parsing JSON in file " + full_name + ": " + e.what());
    }
}

} // namespace json_utils
