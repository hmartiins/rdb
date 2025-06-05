#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json update_json_file(const std::string &filename_raw,
                      const std::string &id,
                      const std::string &new_json_str)
{
  try
  {
    std::string filename = filename_raw;
    if (filename.size() < 5 || filename.substr(filename.size() - 5) != ".json")
    {
      filename += ".json";
    }
    std::cout << "📄 Update file: " << filename << " with id " << id << std::endl;

    std::ifstream infile(filename);
    if (!infile.is_open())
    {
      throw std::runtime_error("Error to open file: " + filename);
    }

    json existing;
    infile >> existing;
    infile.close();

    if (!existing.contains("id") || existing["id"].get<std::string>() != id)
    {
      throw std::runtime_error("ID not matching (expectaded = " + id +
                               ", recived = " +
                               (existing.contains("id") ? existing["id"].get<std::string>() : std::string("nenhum")) +
                               ")");
    }

    if (new_json_str.empty())
    {
      throw std::runtime_error("JSON de atualização está vazio!");
    }
    json to_update = json::parse(new_json_str);

    for (auto &[key, val] : to_update.items())
    {
      existing[key] = val;
    }

    std::ofstream outfile(filename);
    if (!outfile.is_open())
    {
      throw std::runtime_error("Não foi possível abrir para escrita: " + filename);
    }
    outfile << existing.dump(2);
    outfile.close();

    return json::object({{"message", "File updated successfully: " + filename}});
  }
  catch (const std::exception &e)
  {
    std::string error_msg = "❌ Error: " + std::string(e.what());
    std::cout << error_msg << std::endl;
    return json::object({{"error", error_msg}});
  }
}