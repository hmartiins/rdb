#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <thread>
#include "socket_utils.hpp"
#include "utils/split.hpp"
#include <vector>

#include "room_manager.hpp"
#include "json/create-file.hpp"
#include "json/read-file.hpp"
#include "json/update-file.hpp"

constexpr int BUFFER_SIZE = 1024;
constexpr int MAX_QUEUE = 3;

int setupServerSocket(int port);
int acceptClientConnection(int server_fd);
void handleClientCommunication(int client_socket, RoomManager &roomManager);

int main(int argc, char *argv[])
{
  RoomManager roomManager;

  int port = getPortArguments(argc, argv);
  if (port == -1)
    return EXIT_FAILURE;

  int server_fd = setupServerSocket(port);
  std::cout << "🟢 Server running on port " << port << "..." << std::endl;

  while (true)
  {
    int client_socket = acceptClientConnection(server_fd);
    std::thread clientThread(handleClientCommunication, client_socket, std::ref(roomManager));
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
    perror("❌ Failed to bind");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, MAX_QUEUE) < 0)
  {
    perror("❌ Failed to listen");
    exit(EXIT_FAILURE);
  }

  return server_fd;
}

int acceptClientConnection(int server_fd)
{
  struct sockaddr_in address;
  socklen_t addrlen = sizeof(address);

  int new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
  if (new_socket < 0)
  {
    perror("❌ Failed to accept connection");
    exit(EXIT_FAILURE);
  }

  std::cout << "🔵 New client connected!" << std::endl;
  return new_socket;
}

void handleClientCommunication(int client_socket, RoomManager &roomManager)
{
  char buffer[BUFFER_SIZE] = {0};

  while (true)
  {
    memset(buffer, 0, sizeof(buffer));
    int valread = recv(client_socket, buffer, sizeof(buffer) - 1, 0); // Melhor que read()

    if (valread <= 0)
    {
      std::cout << "🔴 Client disconnected." << std::endl;
      break;
    }

    std::vector<std::string> split_result = split(buffer, ' ');

    if (split_result.size() < 2)
    {
      std::string error_msg = "❌ Invalid request format.\n";
      send(client_socket, error_msg.c_str(), error_msg.length(), 0);
      continue;
    }

    std::string operation = split_result[0];
    std::string filename = split_result[1];
    std::string json_string = split_result.size() > 2 ? split_result[2] : "";
    std::string id = split_result.size() > 3 ? split_result[3] : "";

    if (operation == "CREATE")
    {
      json data = create_json_file(JsonData{.json_string = json_string, .file_name = filename});
      std::string json_response = data.dump(2) + "\n";
      send(client_socket, json_response.c_str(), json_response.length(), 0);
    }
    else if (operation == "READ")
    {
      roomManager.joinRoom(filename, client_socket);
      json data = read_json_file(filename);
      std::string json_response = data.dump(2) + "\n";
      send(client_socket, json_response.c_str(), json_response.length(), 0);
    }
    else if (operation == "UPDATE")
    {
      json data = update_json_file(filename, id, json_string);
      std::string json_response = data.dump(2) + "\n";
      send(client_socket, json_response.c_str(), json_response.length(), 0);

      std::string notify_msg = "🔔 File " + filename + " was updated by another client.\n";
      roomManager.notifyOthers(filename, client_socket, notify_msg);
    }
    else
    {
      std::string error_msg = "❌ Unknown operation: " + operation + "\n";
      send(client_socket, error_msg.c_str(), error_msg.length(), 0);
    }
  }

  roomManager.leaveAllRooms(client_socket);
  close(client_socket);
}
