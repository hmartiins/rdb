#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <getopt.h>
#include <netinet/in.h>

#include "socket_utils.h"

int main(int argc, char *argv[])
{
  int port = 3333;

  static struct option long_options[] = {
      {"port", required_argument, 0, 'p'},
      {0, 0, 0, 0}};

  int opt;
  while ((opt = getopt_long(argc, argv, "p:", long_options, NULL)) != -1)
  {
    switch (opt)
    {
    case 'p':
      port = atoi(optarg);
      if (port <= 0 || port > 65535)
      {
        std::cerr << "Porta inválida! Escolha um valor entre 1 e 65535." << std::endl;
        return EXIT_FAILURE;
      }
      break;
    default:
      std::cerr << "Uso: " << argv[0] << " --port <porta>" << std::endl;
      return EXIT_FAILURE;
    }
  }

  int sock = 0;
  struct sockaddr_in serv_addr;
  char buffer[1024] = {0};

  sock = createSocket();

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

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
