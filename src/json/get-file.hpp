#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json read_json_file(const std::string &filename)
{
  std::ifstream file(filename);
  if (!file)
  {
    throw std::runtime_error("Erro ao abrir o arquivo " + filename);
  }

  json jsonData;
  file >> jsonData;

  file.clear();
  file.seekg(0, std::ios::beg);
  file.close();

  return jsonData;
}