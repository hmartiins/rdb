#pragma once
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

namespace json_utils {

inline void update_json_file(const std::string& file_name, const std::string& new_json_string) {
    nlohmann::json parsed;
    try {
        parsed = nlohmann::json::parse(new_json_string);
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Invalid JSON format: " + std::string(e.what()));
    }

    std::string full_name = file_name + ".json";
    std::ofstream ofs(full_name, std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        throw std::runtime_error("Could not open file for update: " + full_name);
    }

    ofs << parsed.dump(4) << std::endl;
    ofs.close();
}

} // namespace json_utils
