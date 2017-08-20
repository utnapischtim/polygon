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

void pl::convert(std::vector<cgal::Point_2> &point_2_list, const PointList &point_list) {
  std::transform(point_list.begin(),
                 point_list.end(),
                 std::back_inserter(point_2_list),
                 [](auto p) { return cgal::Point_2(p.x, p.y); });
}


std::ostream &pl::operator<<(std::ostream &out, const PointList &point_list) {
  for (auto p : point_list)
    out << p;

  return out;
}
