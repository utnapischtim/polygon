#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string>
#include <tuple>
#include <sstream>

#include <json.hpp>

namespace pl {

std::string to_string(nlohmann::json obj);
std::vector<std::string> split(std::stringstream &ss, char delim);
std::vector<std::string> split(const std::string &s, char delim);

}

#endif
