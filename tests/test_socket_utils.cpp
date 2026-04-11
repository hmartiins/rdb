#include <gtest/gtest.h>
#include "socket_utils.hpp"

#include <unistd.h>

// getopt_long uses global state — reset before each call
static void resetGetopt()
{
  optind = 1;
  opterr = 0;
}

// --- createSocket ---

TEST(CreateSocket, ReturnsValidFileDescriptor)
{
  int fd = createSocket();
  EXPECT_GT(fd, 0);
  close(fd);
}

// --- getPortArguments ---

TEST(GetPortArguments, DefaultPortWhenNoArgs)
{
  resetGetopt();
  char prog[] = "test";
  char *argv[] = {prog, nullptr};
  EXPECT_EQ(getPortArguments(1, argv), DEFAULT_PORT);
}

TEST(GetPortArguments, ValidPortLongFlag)
{
  resetGetopt();
  char prog[] = "test";
  char flag[] = "--port";
  char value[] = "8080";
  char *argv[] = {prog, flag, value, nullptr};
  EXPECT_EQ(getPortArguments(3, argv), 8080);
}

TEST(GetPortArguments, ValidPortShortFlag)
{
  resetGetopt();
  char prog[] = "test";
  char flag[] = "-p";
  char value[] = "4000";
  char *argv[] = {prog, flag, value, nullptr};
  EXPECT_EQ(getPortArguments(3, argv), 4000);
}

TEST(GetPortArguments, MinimumValidPort)
{
  resetGetopt();
  char prog[] = "test";
  char flag[] = "--port";
  char value[] = "1";
  char *argv[] = {prog, flag, value, nullptr};
  EXPECT_EQ(getPortArguments(3, argv), 1);
}

TEST(GetPortArguments, MaximumValidPort)
{
  resetGetopt();
  char prog[] = "test";
  char flag[] = "--port";
  char value[] = "65535";
  char *argv[] = {prog, flag, value, nullptr};
  EXPECT_EQ(getPortArguments(3, argv), 65535);
}

TEST(GetPortArguments, PortZeroIsInvalid)
{
  resetGetopt();
  char prog[] = "test";
  char flag[] = "--port";
  char value[] = "0";
  char *argv[] = {prog, flag, value, nullptr};
  EXPECT_EQ(getPortArguments(3, argv), -1);
}

TEST(GetPortArguments, PortAbove65535IsInvalid)
{
  resetGetopt();
  char prog[] = "test";
  char flag[] = "--port";
  char value[] = "65536";
  char *argv[] = {prog, flag, value, nullptr};
  EXPECT_EQ(getPortArguments(3, argv), -1);
}

TEST(GetPortArguments, UnknownFlagIsInvalid)
{
  resetGetopt();
  char prog[] = "test";
  char flag[] = "--unknown";
  char *argv[] = {prog, flag, nullptr};
  EXPECT_EQ(getPortArguments(2, argv), -1);
}
