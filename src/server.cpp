// src/server.cpp

#include <iostream>
#include <thread>
#include <vector>
#include <cstring>        // memset
#include <unistd.h>       // read, send, close
#include <algorithm>      // std::remove
#include <unordered_map>
#include <mutex>

#include "socket_utils.hpp"       // createSocket(), getPortArguments(...)
#include "utils/split.hpp"      // split(str, ' ')
#include "utils/logger.hpp"     // util::Logger

#include "json/create-file.hpp" // json_utils::create_json_file(...)
#include "json/read-file.hpp"   // json_utils::read_json_file(...)
#include "json/update-file.hpp" // json_utils::update_json_file(...)
#include "json/delete-file.hpp" // json_utils::delete_json_file(...)

#include <nlohmann/json.hpp>    // nlohmann::json

void handleClient(int client_socket) {
    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];

    while (true) {
        // Lê dados do cliente
        memset(buffer, 0, sizeof(buffer));
        int valread = read(client_socket, buffer, sizeof(buffer));
        if (valread <= 0) {
            // Cliente desconectou ou erro de leitura
            close(client_socket);
            break;
        }

        // Converte para std::string e separa por espaços
        std::string msg(buffer);
        auto tokens = split(msg, ' ');
        if (tokens.empty()) {
            continue;
        }

        std::string operation = tokens[0];
        std::string filename;
        std::string json_string;

        if (tokens.size() >= 2) {
            filename = tokens[1];
        }
        if ((operation == "CREATE" || operation == "UPDATE") && tokens.size() >= 3) {
            // Tudo após "OP filename " é JSON (pode conter espaços)
            json_string = msg.substr(operation.size() + 1 + filename.size() + 1);
        }

        // ──────────── CREATE ────────────
        if (operation == "CREATE") {
            try {
                json_utils::create_json_file(filename, json_string);
                util::Logger::instance().log(
                    "CLIENT " + std::to_string(client_socket) + " -> CREATE " + filename + ".json (ok)"
                );
                std::string response = "✔ File created successfully.\n";
                send(client_socket, response.c_str(), (int)response.length(), 0);
            } catch (const std::exception& e) {
                util::Logger::instance().log(
                    "CLIENT " + std::to_string(client_socket) + " -> CREATE " + filename + ".json (err: " + e.what() + ")"
                );
                std::string err = "✗ CREATE error: " + std::string(e.what()) + "\n";
                send(client_socket, err.c_str(), (int)err.length(), 0);
            }
        }
        // ──────────── READ ────────────
        else if (operation == "READ") {
            try {
                nlohmann::json data = json_utils::read_json_file(filename);
                util::Logger::instance().log(
                    "CLIENT " + std::to_string(client_socket) + " -> READ " + filename + ".json (ok)"
                );
                std::string json_response = data.dump(4) + "\n";
                send(client_socket, json_response.c_str(), (int)json_response.length(), 0);
            } catch (const std::exception& e) {
                util::Logger::instance().log(
                    "CLIENT " + std::to_string(client_socket) + " -> READ " + filename + ".json (err: " + e.what() + ")"
                );
                std::string err = "✗ READ error: " + std::string(e.what()) + "\n";
                send(client_socket, err.c_str(), (int)err.length(), 0);
            }
        }
        // ──────────── UPDATE ────────────
        else if (operation == "UPDATE") {
            try {
                json_utils::update_json_file(filename, json_string);
                util::Logger::instance().log(
                    "CLIENT " + std::to_string(client_socket) + " -> UPDATE " + filename + ".json (ok)"
                );
                std::string response = "✔ File updated successfully.\n";
                send(client_socket, response.c_str(), (int)response.length(), 0);
            } catch (const std::exception& e) {
                util::Logger::instance().log(
                    "CLIENT " + std::to_string(client_socket) + " -> UPDATE " + filename + ".json (err: " + e.what() + ")"
                );
                std::string err = "✗ UPDATE error: " + std::string(e.what()) + "\n";
                send(client_socket, err.c_str(), (int)err.length(), 0);
            }
        }
        // ──────────── DELETE ────────────
        else if (operation == "DELETE") {
            try {
                json_utils::delete_json_file(filename);
                util::Logger::instance().log(
                    "CLIENT " + std::to_string(client_socket) + " -> DELETE " + filename + ".json (ok)"
                );
                std::string response = "✔ File deleted successfully.\n";
                send(client_socket, response.c_str(), (int)response.length(), 0);
            } catch (const std::exception& e) {
                util::Logger::instance().log(
                    "CLIENT " + std::to_string(client_socket) + " -> DELETE " + filename + ".json (err: " + e.what() + ")"
                );
                std::string err = "✗ DELETE error: " + std::string(e.what()) + "\n";
                send(client_socket, err.c_str(), (int)err.length(), 0);
            }
        }
        // ──────────── OPERAÇÃO DESCONHECIDA ────────────
        else {
            std::string error_msg = "✖ Unknown operation: " + operation + "\n";
            send(client_socket, error_msg.c_str(), (int)error_msg.length(), 0);
        }
    }
}

int main(int argc, char* argv[]) {
    // Obtém a porta por CLI: --port <número>
    int port = getPortArguments(argc, argv);
    if (port < 0) {
        std::cerr << "Uso: " << argv[0] << " --port <número>" << std::endl;
        return 1;
    }

    // Cria o socket TCP
    int server_fd = createSocket();

    // Configura bind e listen manualmente:
    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // 0.0.0.0
    address.sin_port        = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("[socket_utils] bind() failed");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("[socket_utils] listen() failed");
        close(server_fd);
        return 1;
    }

    std::cout << "Server running on port " << port << std::endl;

    // Loop principal: aceita conexões e dispara uma thread para cada cliente
    while (true) {
        sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            perror("[socket_utils] accept() failed");
            continue;
        }

        // Cria uma thread para tratar este cliente
        std::thread t(handleClient, client_socket);
        t.detach();
    }

    // Nunca chega aqui, mas para manter simetria:
    close(server_fd);
    return 0;
}
