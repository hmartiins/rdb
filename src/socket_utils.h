#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstdlib>

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

#endif
