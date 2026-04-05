#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <thread>
#include <sstream>
#include <nlohmann/json.hpp>

#include "socket_utils.hpp"
#include "protocol.hpp"

using json = nlohmann::json;

int setupServerConnection(int port);
void receiveServerMessages(int sock);
void sendMessagesToServer(int sock);

int main(int argc, char *argv[])
{
  int port = getPortArguments(argc, argv);
  if (port == -1)
    return EXIT_FAILURE;

  int sock = setupServerConnection(port);

  std::thread receiveThread(receiveServerMessages, sock);

  sendMessagesToServer(sock);

  receiveThread.join();

  return 0;
}

int setupServerConnection(int port)
{
  int sock;
  struct sockaddr_in serv_addr;

  sock = createSocket();
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
  {
    perror("Invalid address/Address not supported");
    exit(EXIT_FAILURE);
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    perror("Connection failed");
    exit(EXIT_FAILURE);
  }

  std::cout << "Connected to server!" << std::endl;
  return sock;
}

void receiveServerMessages(int sock)
{
  std::string payload;
  while (recvMessage(sock, payload))
  {
    std::cout << "\nServer: " << payload << std::endl;
    std::cout << "Client (you): " << std::flush;
  }
}

void sendMessagesToServer(int sock)
{
  std::cout << "Usage:" << std::endl;
  std::cout << "  CREATE <filename> <json>" << std::endl;
  std::cout << "  READ <filename>" << std::endl;
  std::cout << "  UPDATE <filename> <json> <id>" << std::endl;

  std::string line;
  while (true)
  {
    std::cout << "Client (you): ";
    if (!std::getline(std::cin, line) || line.empty())
      continue;

    std::istringstream iss(line);
    std::string op, filename;
    iss >> op >> filename;

    std::string rest;
    std::getline(iss, rest);
    if (!rest.empty() && rest[0] == ' ')
      rest = rest.substr(1);

    json request;
    request["op"] = op;
    request["filename"] = filename;

    if (op == "CREATE")
    {
      try
      {
        request["data"] = json::parse(rest);
      }
      catch (...)
      {
        std::cerr << "❌ Invalid JSON. Usage: CREATE <filename> <json>" << std::endl;
        continue;
      }
    }
    else if (op == "UPDATE")
    {
      // Format: UPDATE <filename> <json> <id>
      // id is always the last whitespace-separated token
      auto last_space = rest.rfind(' ');
      if (last_space == std::string::npos)
      {
        std::cerr << "❌ Usage: UPDATE <filename> <json> <id>" << std::endl;
        continue;
      }
      std::string json_data = rest.substr(0, last_space);
      std::string id = rest.substr(last_space + 1);
      try
      {
        request["data"] = json::parse(json_data);
      }
      catch (...)
      {
        std::cerr << "❌ Invalid JSON. Usage: UPDATE <filename> <json> <id>" << std::endl;
        continue;
      }
      request["id"] = id;
    }

    if (!sendMessage(sock, request.dump()))
    {
      std::cerr << "❌ Connection lost." << std::endl;
      break;
    }
  }
}
