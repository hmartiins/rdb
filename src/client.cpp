#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <getopt.h>
#include <netinet/in.h>
#include "socket_utils.h"

constexpr int BUFFER_SIZE = 1024;

int setupServerConnection(int port);
void handleServerCommunication(int sock);

int main(int argc, char *argv[])
{
  int port = getPortArguments(argc, argv);
  if (port == -1)
    return EXIT_FAILURE;

  int sock = setupServerConnection(port);
  std::cout << "Server connected!" << std::endl;

  handleServerCommunication(sock);

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

  return sock;
}

void handleServerCommunication(int sock)
{
  char buffer[BUFFER_SIZE] = {0};

  while (true)
  {
    std::cout << "Client (you): ";
    std::string msg;
    std::getline(std::cin, msg);

    send(sock, msg.c_str(), msg.length(), 0);

    int valread = read(sock, buffer, sizeof(buffer));
    if (valread > 0)
    {
      std::cout << "Server: " << buffer << std::endl;
    }
  }
}
