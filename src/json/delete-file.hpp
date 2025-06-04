#pragma once
#include <cstdio>
#include <stdexcept>
#include <string>

namespace json_utils {

inline void delete_json_file(const std::string& file_name) {
    std::string full_name = file_name + ".json";
    if (std::remove(full_name.c_str()) != 0) {
        throw std::runtime_error("Could not delete file (maybe it does not exist): " + full_name);
    }
}

} // namespace json_utils
