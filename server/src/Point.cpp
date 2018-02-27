#include <iostream>
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

  point_list.push_back(point_list[0]);
}

void pl::convert(const PointList &point_list, std::vector<cgal::Segment_2> &segments) {
  for (size_t i = 1, size = point_list.size(); i < size; ++i)
    segments.push_back({point_list[i - 1], point_list[i]});

  // close the polygon
  segments.push_back({point_list[point_list.size() - 1], point_list[0]});
}


std::ostream &pl::operator<<(std::ostream &out, const PointList &point_list) {
  for (auto p : point_list)
    out << p;

  return out;
}

std::string pl::to_string(const cgal::Point_2 &p) {
  return std::to_string(p.x()) + "," + std::to_string(p.y());
}
