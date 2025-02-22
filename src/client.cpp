#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8000

int main()
{
  int sock = 0;
  struct sockaddr_in serv_addr;
  char buffer[1024] = {0};

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
  {
    perror("Invalid IP Address or Address not supported");
    exit(EXIT_FAILURE);
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    perror("Failed to connect");
    exit(EXIT_FAILURE);
  }

  std::cout << "Connected to Server" << std::endl;

  while (true)
  {
    std::cout << "You: ";
    std::string msg;
    std::getline(std::cin, msg);
    send(sock, msg.c_str(), msg.length(), 0);

    int valread = read(sock, buffer, sizeof(buffer));
    if (valread > 0)
    {
      std::cout << "Server: " << buffer << std::endl;
    }
  }

  return 0;
}
