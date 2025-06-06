#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstdlib>
#include <getopt.h>

constexpr int DEFAULT_PORT = 3333;

int createSocket()
{
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock <= 0)
  {
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
  }
  return sock;
}

int getPortArguments(int argc, char *argv[])
{
  int port = DEFAULT_PORT;
  static struct option long_options[] = {
      {"port", required_argument, nullptr, 'p'},
      {nullptr, 0, nullptr, 0},
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "p:", long_options, nullptr)) != -1)
  {
    if (opt == 'p')
    {
      port = std::atoi(optarg);
      if (port <= 0 || port > 65535)
      {
        std::cerr << "Invalid PORT. Selected a value between 1 and 65535." << std::endl;
        return -1;
      }
    }
    else
    {
      std::cerr << "Use: " << argv[0] << " --port <port>" << std::endl;
      return -1;
    }
  }
  return port;
}

#endif
