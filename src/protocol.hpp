#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdint>
#include <string>

constexpr uint32_t MAX_MESSAGE_SIZE = 10 * 1024 * 1024; // 10 MB

// Sends: [4-byte length (network order)] + [payload]
inline bool sendMessage(int sock, const std::string &payload)
{
  uint32_t len = htonl(static_cast<uint32_t>(payload.size()));
  if (send(sock, &len, sizeof(len), 0) != sizeof(len))
    return false;

  size_t total = 0;
  while (total < payload.size())
  {
    ssize_t sent = send(sock, payload.c_str() + total, payload.size() - total, 0);
    if (sent <= 0)
      return false;
    total += sent;
  }
  return true;
}

// Reads a full message, blocking until all bytes arrive or connection closes
inline bool recvMessage(int sock, std::string &out)
{
  uint32_t len_net = 0;
  size_t received = 0;
  while (received < sizeof(len_net))
  {
    ssize_t r = recv(sock, reinterpret_cast<char *>(&len_net) + received,
             sizeof(len_net) - received, 0);
    if (r <= 0)
      return false;
    received += r;
  }

  uint32_t len = ntohl(len_net);
  if (len == 0 || len > MAX_MESSAGE_SIZE)
    return false;

  out.resize(len);
  received = 0;
  while (received < len)
  {
    ssize_t r = recv(sock, &out[received], len - received, 0);
    if (r <= 0)
      return false;
    received += r;
  }
  return true;
}
