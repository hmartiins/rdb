#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "../utils/random-id.hpp"

using json = nlohmann::json;

struct JsonData
{
  std::string json_string;
  std::string file_name;
};

json create_json_file(JsonData data)
{
  std::cout << "🛠️ Server creating..." << std::endl;
  try
  {
    std::string filename = data.file_name;
    if (filename.size() < 5 || filename.substr(filename.size() - 5) != ".json")
    {
      filename += ".json";
    }
    std::cout << "📄 Creating file: " << filename << std::endl;

    if (data.json_string.empty())
    {
      throw std::runtime_error("JSON string is empty! " + filename);
    }

    json parsed_json = json::parse(data.json_string);
    parsed_json["id"] = generate_random_id();

    std::ofstream file(filename);
    if (file.is_open())
    {
      file << parsed_json.dump(2);
      file.close();

      return json::object({{"message", "File created successfully: " + filename}});
    }
    else
    {
      throw std::runtime_error("Error to open file " + filename);
    }
  }
  catch (const std::exception &e)
  {
    std::string error_msg = "❌ Error: " + std::string(e.what()) + "\n";
    std::cout << error_msg << std::endl;
    return json::object({{"error", error_msg}});
  }
}
