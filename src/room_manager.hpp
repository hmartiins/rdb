#include <map>
#include <set>
#include <mutex>
#include <string>
#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>

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
    std::cout << "🔔 Notifying others in room: " << filename << std::endl;

    std::lock_guard<std::mutex> lock(mutex);

    std::cout << rooms.size() << " rooms available." << std::endl;
    for (const auto &[room_name, clients] : rooms)
    {
      std::cout << "Room: " << room_name << " has " << clients.size() << " clients." << std::endl;
    }

    auto it = rooms.find(filename);
    if (it != rooms.end())
    {
      for (int sock : it->second)
      {
        if (sock != sender_socket)
        {
          send(sock, message.c_str(), message.length(), 0);
        }
      }
    }
  }
};
