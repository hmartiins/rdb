#include <gtest/gtest.h>
#include "protocol.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <stdexcept>

struct SocketPair
{
  int a, b;

  SocketPair()
  {
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0)
      throw std::runtime_error("socketpair failed");
    a = fds[0];
    b = fds[1];
  }

  ~SocketPair()
  {
    close(a);
    close(b);
  }
};

// --- sendMessage ---

TEST(SendMessage, ReturnsTrueOnSuccess)
{
  SocketPair sp;
  EXPECT_TRUE(sendMessage(sp.a, "hello"));
}

TEST(SendMessage, ReturnsFalseOnClosedSocket)
{
  SocketPair sp;
  close(sp.a);
  sp.a = -1;
  EXPECT_FALSE(sendMessage(-1, "hello"));
}

// --- recvMessage ---

TEST(RecvMessage, RoundTrip_ShortPayload)
{
  SocketPair sp;
  ASSERT_TRUE(sendMessage(sp.a, "hello"));
  std::string out;
  ASSERT_TRUE(recvMessage(sp.b, out));
  EXPECT_EQ(out, "hello");
}

TEST(RecvMessage, RoundTrip_EmptyPayload)
{
  // len == 0 → recvMessage rejects it (protocol enforces len > 0)
  SocketPair sp;
  uint32_t len = htonl(0);
  send(sp.a, &len, sizeof(len), 0);

  std::string out;
  EXPECT_FALSE(recvMessage(sp.b, out));
}

TEST(RecvMessage, RoundTrip_LargePayload)
{
  // Keep under the AF_UNIX default send buffer (~8 KB on macOS) to avoid
  // a deadlock between sendMessage and recvMessage on the same thread.
  SocketPair sp;
  std::string big(4 * 1024, 'x'); // 4 KB
  ASSERT_TRUE(sendMessage(sp.a, big));
  std::string out;
  ASSERT_TRUE(recvMessage(sp.b, out));
  EXPECT_EQ(out, big);
}

TEST(RecvMessage, RoundTrip_JsonPayload)
{
  SocketPair sp;
  std::string json = R"({"op":"READ","filename":"data.json"})";
  ASSERT_TRUE(sendMessage(sp.a, json));
  std::string out;
  ASSERT_TRUE(recvMessage(sp.b, out));
  EXPECT_EQ(out, json);
}

TEST(RecvMessage, ReturnsFalseWhenConnectionClosed)
{
  SocketPair sp;
  close(sp.a);
  sp.a = -1; // write-end closed — read-end sees EOF

  std::string out;
  EXPECT_FALSE(recvMessage(sp.b, out));
}

TEST(RecvMessage, ReturnsFalseWhenPayloadExceedsMaxSize)
{
  SocketPair sp;
  // Send a header claiming size > 10 MB without actually sending that data.
  // recvMessage should reject it before trying to allocate.
  uint32_t oversized = htonl(MAX_MESSAGE_SIZE + 1);
  send(sp.a, &oversized, sizeof(oversized), 0);

  std::string out;
  EXPECT_FALSE(recvMessage(sp.b, out));
}

TEST(RecvMessage, ReturnsFalseOnTruncatedPayload)
{
  SocketPair sp;
  // Claim 100 bytes but only send 10
  uint32_t len = htonl(100);
  send(sp.a, &len, sizeof(len), 0);
  send(sp.a, "0123456789", 10, 0);
  close(sp.a);
  sp.a = -1; // EOF triggers mid-read failure

  std::string out;
  EXPECT_FALSE(recvMessage(sp.b, out));
}

// --- send/recv symmetry across multiple messages ---

TEST(Protocol, MultipleMessagesInSequence)
{
  SocketPair sp;
  std::vector<std::string> msgs = {"first", "second", "third"};

  for (auto &m : msgs)
    ASSERT_TRUE(sendMessage(sp.a, m));

  for (auto &expected : msgs)
  {
    std::string out;
    ASSERT_TRUE(recvMessage(sp.b, out));
    EXPECT_EQ(out, expected);
  }
}
