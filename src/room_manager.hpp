#pragma once

#include <map>
#include <set>
#include <mutex>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <nlohmann/json.hpp>

#include "json/read-file.hpp"
#include "protocol.hpp"

using json = nlohmann::json;

class RoomManager
{
private:
  std::map<std::string, std::set<int>> rooms; // filename -> client sockets
  std::mutex mutex;

public:
  void joinRoom(const std::string &filename, int client_socket)
  {
    std::lock_guard<std::mutex> lock(mutex);
    rooms[filename].insert(client_socket);
  }

  void leaveAllRooms(int client_socket)
  {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto &[_, clients] : rooms)
    {
      clients.erase(client_socket);
    }
  }

  void notifyOthers(const std::string &filename, int sender_socket, const std::string &message)
  {
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << "🔔 Notifying others in room: " << filename << std::endl;

    auto it = rooms.find(filename);
    if (it == rooms.end())
      return;

    json updatedData = read_json_file(filename);
    json notification = {
      {"event", "update"},
      {"message", message},
      {"data", updatedData}};
    std::string payload = notification.dump(2);

    for (int sock : it->second)
    {
      if (sock != sender_socket)
        sendMessage(sock, payload);
    }
  }
};
