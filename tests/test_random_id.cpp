#include <gtest/gtest.h>
#include "utils/random-id.hpp"

#include <algorithm>
#include <string>

TEST(GenerateRandomId, DefaultLengthIs16)
{
  std::string id = generate_random_id();
  EXPECT_EQ(id.size(), 16);
}

TEST(GenerateRandomId, CustomLength)
{
  EXPECT_EQ(generate_random_id(8).size(), 8);
  EXPECT_EQ(generate_random_id(32).size(), 32);
  EXPECT_EQ(generate_random_id(1).size(), 1);
}

TEST(GenerateRandomId, ZeroLengthReturnsEmpty)
{
  std::string id = generate_random_id(0);
  EXPECT_TRUE(id.empty());
}

TEST(GenerateRandomId, OnlyAlphanumericCharacters)
{
  std::string id = generate_random_id(100);
  EXPECT_TRUE(std::all_of(id.begin(), id.end(), [](char c) {
    return std::isalnum(static_cast<unsigned char>(c));
  }));
}

TEST(GenerateRandomId, ConsecutiveCallsProduceDifferentIds)
{
  std::string id1 = generate_random_id();
  std::string id2 = generate_random_id();
  EXPECT_NE(id1, id2);
}
