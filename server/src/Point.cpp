#include <iostream>
#include <cmath>
#include <string>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "json.hpp"

#include "Point.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

nlohmann::json pl::to_json(pl::PointList point_list) {
  nlohmann::json obj;

  for (auto point : point_list)
    obj.push_back({{"x", point.x()}, {"y", point.y()}});

  return obj;
}

void pl::convert(const std::vector<cgal::Segment_2> &segments, PointList &point_list) {
  std::transform(segments.begin(),
                 segments.end(),
                 std::back_inserter(point_list),
                 [](auto &s) { return s.source(); });
                 //[](auto &s) { return pl::Point(s.source().x(), s.source().y()); });

  point_list.push_back(point_list[0]);
}


std::ostream &pl::operator<<(std::ostream &out, const PointList &point_list) {
  for (auto p : point_list)
    out << p;

  return out;
}

std::string pl::to_string(const cgal::Point_2 &p) {
  return std::to_string(p.x()) + "," + std::to_string(p.y());
}
