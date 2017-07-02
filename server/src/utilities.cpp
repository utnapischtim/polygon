#include <sstream>

#include "json.hpp"

#include "utilities.h"

std::string pl::to_string(nlohmann::json obj) {
  std::stringstream ss;

  ss << obj;

  return ss.str();
}
