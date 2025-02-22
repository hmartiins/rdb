#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <getopt.h>
#include "socket_utils.h"

constexpr int BUFFER_SIZE = 1024;
constexpr int MAX_QUEUE = 3;

int setupServerSocket(int port);
int acceptClientConnection(int server_fd);
void handleClientMessages(int client_socket);

int main(int argc, char *argv[])
{
  int port = getPortArguments(argc, argv);
  if (port == -1)
    return EXIT_FAILURE;

  int server_fd = setupServerSocket(port);
  std::cout << "Server running in port " << port << "..." << std::endl;

  int client_socket = acceptClientConnection(server_fd);
  handleClientMessages(client_socket);

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

  std::cout << "Client connected!" << std::endl;
  return new_socket;
}

void handleClientMessages(int client_socket)
{
  char buffer[BUFFER_SIZE] = {0};

  while (true)
  {
    memset(buffer, 0, sizeof(buffer));
    int valread = read(client_socket, buffer, sizeof(buffer));
    if (valread > 0)
    {
      std::cout << "Client: " << buffer << std::endl;
    }

    std::cout << "Server (you): ";
    std::string msg;
    std::getline(std::cin, msg);
    send(client_socket, msg.c_str(), msg.length(), 0);
  }
}
