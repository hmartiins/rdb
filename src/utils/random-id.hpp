#include <random>
#include <sstream>

std::string generate_random_id(size_t length = 16)
{
  static const char charset[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

  std::string id;
  for (size_t i = 0; i < length; ++i)
  {
    id += charset[dist(generator)];
  }
  return id;
}