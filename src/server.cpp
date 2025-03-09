#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <thread>
#include "socket_utils.hpp"
#include "json/create-file.hpp"
#include "utils/split.hpp"
#include <vector>
#include "json/get-file.hpp"

constexpr int BUFFER_SIZE = 1024;
constexpr int MAX_QUEUE = 3;

int setupServerSocket(int port);
int acceptClientConnection(int server_fd);
void handleClientCommunication(int client_socket);

int main(int argc, char *argv[])
{
  int port = getPortArguments(argc, argv);
  if (port == -1)
    return EXIT_FAILURE;

  int server_fd = setupServerSocket(port);
  std::cout << "Server running on port " << port << "..." << std::endl;

  while (true)
  {
    int client_socket = acceptClientConnection(server_fd);
    std::thread clientThread(handleClientCommunication, client_socket);
    clientThread.detach();
  }

  return 0;
}

int setupServerSocket(int port)
{
  int server_fd;
  struct sockaddr_in address;

  server_fd = createSocket();

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    perror("Failed to bind");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, MAX_QUEUE) < 0)
  {
    perror("Failed to listen");
    exit(EXIT_FAILURE);
  }

  return server_fd;
}

int acceptClientConnection(int server_fd)
{
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
  if (new_socket < 0)
  {
    perror("Failed to accept connection");
    exit(EXIT_FAILURE);
  }

  return new_socket;
}

void handleClientCommunication(int client_socket)
{
  char buffer[BUFFER_SIZE] = {0};

  while (true)
  {
    memset(buffer, 0, sizeof(buffer));
    int valread = read(client_socket, buffer, sizeof(buffer));
    if (valread <= 0)
    {
      std::cout << "Client disconnected." << std::endl;
      break;
    }

    std::vector<std::string> split_result = split(buffer, ' ');
    std::string operation = split_result[0];
    std::string filename = split_result[1];
    std::string json_string = split_result.size() > 2 ? split_result[2] : "";

    if (operation == "CREATE")
    {
      createJsonFile(JsonData{.json_string = json_string, .file_name = filename});

      std::string response = "File created with success\n";
      send(client_socket, response.c_str(), response.length(), 0);
    }
    else if (operation == "READ")
    {
      try
      {
        json data = read_json_file(filename);
        std::string json_response = data.dump(2) + "\n";
        send(client_socket, json_response.c_str(), json_response.length(), 0);
      }
      catch (const std::exception &e)
      {
        std::string error_msg = "Error: " + std::string(e.what()) + "\n";
        send(client_socket, error_msg.c_str(), error_msg.length(), 0);
      }
    }
  }

  close(client_socket);
}
