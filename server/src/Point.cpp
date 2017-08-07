#include <iostream>
#include <cmath>
#include <string>

#include "json.hpp"
#include "easylogging++.h"

#include "Point.h"

nlohmann::json pl::to_json(pl::PointList point_list) {
  nlohmann::json obj;

  for (auto point : point_list)
    obj.push_back({{"x", point.x}, {"y", point.y}});

  return obj;
}

std::ostream &pl::operator<<(std::ostream &out, const PointList &point_list) {
  for (auto p : point_list)
    out << p;

  return out;
}
