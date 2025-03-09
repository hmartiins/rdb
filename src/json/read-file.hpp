#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json read_json_file(const std::string &filename)
{
  std::cout << "📖 Server reading " << filename << std::endl;

  try
  {
    std::ifstream file(filename);
    if (!file)
    {
      throw std::runtime_error("❌ Error to open file " + filename);
    }

    json jsonData;
    file >> jsonData;

    file.clear();
    file.seekg(0, std::ios::beg);
    file.close();

    return jsonData;
  }
  catch (const std::exception &e)
  {
    std::string error_msg = "❌ Error: " + std::string(e.what()) + "\n";
  }
}