#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <thread>
#include <nlohmann/json.hpp>

#include "socket_utils.hpp"
#include "protocol.hpp"
#include "room_manager.hpp"
#include "json/create-file.hpp"
#include "json/read-file.hpp"
#include "json/update-file.hpp"

using json = nlohmann::json;

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
  std::string payload;

  while (true)
  {
    if (!recvMessage(client_socket, payload))
    {
      std::cout << "🔴 Client disconnected." << std::endl;
      break;
    }

    json request;
    try
    {
      request = json::parse(payload);
    }
    catch (const std::exception &e)
    {
      json error = {{"error", "Invalid JSON request: " + std::string(e.what())}};
      sendMessage(client_socket, error.dump());
      continue;
    }

    if (!request.contains("op") || !request.contains("filename"))
    {
      json error = {{"error", "Missing required fields: op, filename"}};
      sendMessage(client_socket, error.dump());
      continue;
    }

    std::string operation = request["op"];
    std::string filename = request["filename"];
    std::string json_string = request.contains("data") ? request["data"].dump() : "";
    std::string id = request.value("id", "");

    if (operation == "CREATE")
    {
      json data = create_json_file(JsonData{.json_string = json_string, .file_name = filename});
      sendMessage(client_socket, data.dump(2));
    }
    else if (operation == "READ")
    {
      roomManager.joinRoom(filename, client_socket);
      json data = read_json_file(filename);
      sendMessage(client_socket, data.dump(2));
    }
    else if (operation == "UPDATE")
    {
      json data = update_json_file(filename, id, json_string);
      sendMessage(client_socket, data.dump(2));

      std::string notify_msg = "File " + filename + " was updated by another client.";
      roomManager.notifyOthers(filename, client_socket, notify_msg);
    }
    else
    {
      json error = {{"error", "Unknown operation: " + operation}};
      sendMessage(client_socket, error.dump());
    }
  }

  roomManager.leaveAllRooms(client_socket);
  close(client_socket);
}
