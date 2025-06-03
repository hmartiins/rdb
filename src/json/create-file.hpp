#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

namespace json_utils {

// 🛠️ Cria (ou sobrescreve) o arquivo "<file_name>.json" a partir da json_string fornecida.
//    Lança std::runtime_error em caso de JSON inválido, string vazia ou falha ao abrir o arquivo.
inline void create_json_file(const std::string& file_name, const std::string& json_string)
{
    // Verifica se a string JSON foi passada
    if (json_string.empty())
    {
        // Mantendo o estilo original: usamos std::cerr e emoji de erro
        std::cerr << "❌ JSON string is empty!" << std::endl;
        throw std::runtime_error("JSON string is empty");
    }

    // Exibe no console (servidor) o que está criando
    std::cout << "🛠️  Server creating " << file_name << std::endl;

    try
    {
        // Tenta converter a string em JSON
        nlohmann::json parsed_json = nlohmann::json::parse(json_string);

        // Monta o nome final do arquivo
        std::string full_name = file_name + ".json";

        // Abre o arquivo em modo sobrescrever (truncate)
        std::ofstream file(full_name);
        if (file.is_open())
        {
            // Escreve o JSON “bonitinho” com indentação de 2 espaços
            file << parsed_json.dump(2) << std::endl;
            file.close();
        }
        else
        {
            std::cerr << "❌ Error opening/creating file: " << full_name << std::endl;
            throw std::runtime_error("Could not open file: " + full_name);
        }
    }
    catch (const nlohmann::json::parse_error& e)
    {
        // Em caso de string não ser JSON válido, nlohmann lança parse_error
        std::cerr << "❌ Error parsing string to JSON: " << e.what() << std::endl;
        throw std::runtime_error(std::string("Error parsing JSON: ") + e.what());
    }
    catch (const std::exception& e)
    {
        // Qualquer outra exceção (por via das dúvidas)
        std::cerr << "❌ Unexpected error in create_json_file: " << e.what() << std::endl;
        throw;
    }
}

} // namespace json_utils
