#include <sstream>
#include <tuple>
#include <string>
#include <algorithm>

#include "json.hpp"

#include "utilities.h"

std::string pl::to_string(nlohmann::json obj) {
  std::stringstream ss;

  ss << obj;

  return ss.str();
}

// template<class... Ts>
// std::tuple<Ts...> pl::split(std::stringstream &ss, char delim = ' ') {
//   std::string item;
//   std::vector<std::string> tokens;
//   while (std::getline(ss, item, delim))
//     tokens.push_back(item);
//   return {tokens...}; // does not compile, i don't know how to do it
// }

std::vector<std::string> pl::split(std::stringstream &ss, char delim = ' ') {
  std::string item;
  std::vector<std::string> tokens;

  while (std::getline(ss, item, delim))
    tokens.push_back(item);

  return tokens;
}

std::vector<std::string> pl::split(const std::string &s, char delim = ' ') {
  std::stringstream ss{s};
  return split(ss, delim);
}

