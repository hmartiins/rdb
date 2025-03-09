#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct JsonData
{
  std::string json_string;
  std::string file_name;
};

void createJsonFile(JsonData data)
{
  if (data.json_string.empty())
  {
    std::cerr << "JSON string is empty!" << std::endl;
    return;
  }

  std::cout << "🛠️ Server creating " << data.file_name << std::endl;

  try
  {
    json parsed_json = json::parse(data.json_string);

    std::ofstream file(data.file_name.append(".json"));
    if (file.is_open())
    {
      file << parsed_json.dump(2);
      file.close();
    }
    else
    {
      std::cerr << "Error to open file!" << std::endl;
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error to parse string to JSON: " << e.what() << std::endl;
  }
}
