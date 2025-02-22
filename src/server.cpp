#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <getopt.h>

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

  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};

  server_fd = createSocket();

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

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

  std::cout << "Servidor rodando na porta " << port << "..." << std::endl;

  if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
  {
    perror("Failed to accept");
    exit(EXIT_FAILURE);
  }

  std::cout << "Cliente conectado!" << std::endl;

  while (true)
  {
    memset(buffer, 0, sizeof(buffer));
    int valread = read(new_socket, buffer, sizeof(buffer));
    if (valread > 0)
    {
      std::cout << "Cliente: " << buffer << std::endl;
    }

    std::cout << "Você: ";
    std::string msg;
    std::getline(std::cin, msg);
    send(new_socket, msg.c_str(), msg.length(), 0);
  }

  return 0;
}
