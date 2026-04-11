#include <gtest/gtest.h>
#include "room_manager.hpp"

#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <fstream>
#include <string>

// Creates /tmp/<name>.json with the given content.
// Returns the path WITHOUT extension (read_json_file appends .json).
static std::string writeTempJson(const std::string &name, const std::string &content)
{
  std::string path = "/tmp/" + name + ".json";
  std::ofstream f(path);
  f << content;
  return "/tmp/" + name;
}

// Returns true if the fd has data available within timeout_ms.
// sendMessage() is synchronous, so data is present as soon as notifyOthers() returns.
static bool hasData(int fd, int timeout_ms = 100)
{
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  struct timeval tv{0, timeout_ms * 1000};
  return select(fd + 1, &fds, nullptr, nullptr, &tv) > 0;
}

// Wraps socketpair(AF_UNIX) for convenience.
// fds[0] is given to the RoomManager (the "client socket").
// fds[1] is kept by the test to observe incoming data.
struct SocketPair
{
  int client; // registered with RoomManager
  int probe;  // read-end for the test

  SocketPair()
  {
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0)
      throw std::runtime_error("socketpair failed");
    client = fds[0];
    probe  = fds[1];
  }

  ~SocketPair()
  {
    close(client);
    close(probe);
  }
};

// --- joinRoom ---

TEST(RoomManager, JoinRoom_ClientReceivesNotification)
{
  RoomManager rm;
  SocketPair sp;
  std::string file = writeTempJson("rm_join", R"({"ok":true})");

  rm.joinRoom(file, sp.client);
  rm.notifyOthers(file, /*sender=*/-1, "update");

  EXPECT_TRUE(hasData(sp.probe));
}

TEST(RoomManager, JoinRoom_MultipleClients_AllReceiveNotification)
{
  RoomManager rm;
  SocketPair a, b, c;
  std::string file = writeTempJson("rm_multi", R"({"ok":true})");

  rm.joinRoom(file, a.client);
  rm.joinRoom(file, b.client);
  rm.joinRoom(file, c.client);
  rm.notifyOthers(file, /*sender=*/-1, "update");

  EXPECT_TRUE(hasData(a.probe));
  EXPECT_TRUE(hasData(b.probe));
  EXPECT_TRUE(hasData(c.probe));
}

TEST(RoomManager, JoinRoom_SameClientTwice_NoDuplicateNotification)
{
  RoomManager rm;
  SocketPair sp;
  std::string file = writeTempJson("rm_dedup", R"({"ok":true})");

  rm.joinRoom(file, sp.client);
  rm.joinRoom(file, sp.client); // inserting the same fd into a set is a no-op
  rm.notifyOthers(file, /*sender=*/-1, "update");

  // Drain exactly one message
  std::string msg;
  ASSERT_TRUE(recvMessage(sp.probe, msg));

  // No second message should arrive
  EXPECT_FALSE(hasData(sp.probe, /*timeout_ms=*/50));
}

// --- leaveAllRooms ---

TEST(RoomManager, LeaveAllRooms_ClientNoLongerReceivesNotification)
{
  RoomManager rm;
  SocketPair sp;
  std::string file = writeTempJson("rm_leave", R"({"ok":true})");

  rm.joinRoom(file, sp.client);
  rm.leaveAllRooms(sp.client);
  rm.notifyOthers(file, /*sender=*/-1, "update");

  EXPECT_FALSE(hasData(sp.probe));
}

TEST(RoomManager, LeaveAllRooms_RemovesFromMultipleRooms)
{
  RoomManager rm;
  SocketPair sp;
  std::string f1 = writeTempJson("rm_leave_r1", R"({"ok":true})");
  std::string f2 = writeTempJson("rm_leave_r2", R"({"ok":true})");

  rm.joinRoom(f1, sp.client);
  rm.joinRoom(f2, sp.client);
  rm.leaveAllRooms(sp.client);

  rm.notifyOthers(f1, -1, "update");
  rm.notifyOthers(f2, -1, "update");

  EXPECT_FALSE(hasData(sp.probe));
}

TEST(RoomManager, LeaveAllRooms_UnknownSocketIsNoOp)
{
  RoomManager rm;
  EXPECT_NO_THROW(rm.leaveAllRooms(9999));
}

// --- notifyOthers ---

TEST(RoomManager, NotifyOthers_RoomDoesNotExist_NoOp)
{
  RoomManager rm;
  EXPECT_NO_THROW(rm.notifyOthers("nonexistent_room", -1, "update"));
}

TEST(RoomManager, NotifyOthers_SenderIsExcluded)
{
  RoomManager rm;
  SocketPair sender, other;
  std::string file = writeTempJson("rm_sender", R"({"ok":true})");

  rm.joinRoom(file, sender.client);
  rm.joinRoom(file, other.client);
  rm.notifyOthers(file, sender.client, "update");

  EXPECT_FALSE(hasData(sender.probe)); // sender gets nothing
  EXPECT_TRUE(hasData(other.probe));   // other client is notified
}

TEST(RoomManager, NotifyOthers_PayloadIsValidLengthPrefixedJson)
{
  RoomManager rm;
  SocketPair sp;
  std::string file = writeTempJson("rm_payload", R"({"value":42})");

  rm.joinRoom(file, sp.client);
  rm.notifyOthers(file, -1, "update");

  std::string raw;
  ASSERT_TRUE(recvMessage(sp.probe, raw));

  auto parsed = json::parse(raw);
  EXPECT_EQ(parsed["event"], "update");
  EXPECT_EQ(parsed["message"], "update");
  EXPECT_TRUE(parsed.contains("data"));
}
