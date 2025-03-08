#include <iostream>
#include <sstream>
#include <vector>

std::vector<std::string> split(const std::string &str, char delimiter)
{
  std::vector<std::string> result;
  std::string token;
  std::istringstream tokenStream(str);

  while (std::getline(tokenStream, token, delimiter))
  {
    result.push_back(token);
  }

  return result;
}