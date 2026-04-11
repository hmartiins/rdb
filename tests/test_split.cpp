#include <gtest/gtest.h>
#include "utils/split.hpp"

TEST(Split, BasicDelimiter)
{
  auto result = split("a,b,c", ',');
  ASSERT_EQ(result.size(), 3);
  EXPECT_EQ(result[0], "a");
  EXPECT_EQ(result[1], "b");
  EXPECT_EQ(result[2], "c");
}

TEST(Split, SingleToken)
{
  auto result = split("hello", ',');
  ASSERT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], "hello");
}

TEST(Split, EmptyString)
{
  auto result = split("", ',');
  EXPECT_TRUE(result.empty());
}

TEST(Split, LeadingDelimiter)
{
  auto result = split(",a,b", ',');
  ASSERT_EQ(result.size(), 3);
  EXPECT_EQ(result[0], "");
  EXPECT_EQ(result[1], "a");
  EXPECT_EQ(result[2], "b");
}

TEST(Split, TrailingDelimiterIsIgnored)
{
  // std::getline ignores trailing delimiter — no empty token at the end
  auto result = split("a,b,", ',');
  ASSERT_EQ(result.size(), 2);
  EXPECT_EQ(result[0], "a");
  EXPECT_EQ(result[1], "b");
}

TEST(Split, ConsecutiveDelimiters)
{
  auto result = split("a,,b", ',');
  ASSERT_EQ(result.size(), 3);
  EXPECT_EQ(result[0], "a");
  EXPECT_EQ(result[1], "");
  EXPECT_EQ(result[2], "b");
}

TEST(Split, DifferentDelimiter)
{
  auto result = split("one two three", ' ');
  ASSERT_EQ(result.size(), 3);
  EXPECT_EQ(result[0], "one");
  EXPECT_EQ(result[1], "two");
  EXPECT_EQ(result[2], "three");
}
