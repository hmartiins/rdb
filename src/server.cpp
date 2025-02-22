#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8000

int main()
{
  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    perror("Failed to bind");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0)
  {
    perror("Failed to listen");
    exit(EXIT_FAILURE);
  }

  std::cout << "Waiting server connect..." << std::endl;

  if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
  {
    perror("Failed to accept");
    exit(EXIT_FAILURE);
  }

  std::cout << "Client connected!" << std::endl;

  while (true)
  {
    memset(buffer, 0, sizeof(buffer));
    int valread = read(new_socket, buffer, sizeof(buffer));
    if (valread > 0)
    {
      std::cout << "Client: " << buffer << std::endl;
    }

    std::cout << "You: ";
    std::string msg;
    std::getline(std::cin, msg);
    send(new_socket, msg.c_str(), msg.length(), 0);
  }

  return 0;
}
