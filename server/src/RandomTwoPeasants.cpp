#include <algorithm>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "Point.h"
#include "RandomTwoPeasants.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

pl::PointList pl::randomTwoPeasants(pl::PointList &point_list) {
  pl::PointList final_list;

  std::sort(point_list.begin(), point_list.end(), [](auto a, auto b) { return a.x < b.x; });

  auto lowest_x_point = *(point_list.begin()), highest_x_point = *(point_list.end() - 1);

  cgal::Point_2 p = {lowest_x_point.x, lowest_x_point.y}, q = {highest_x_point.x, highest_x_point.y};

  pl::PointList upper_list, lower_list;

  for (auto point : point_list) {
    auto orientation = CGAL::orientation(p, q, {point.x, point.y});

    if (orientation == CGAL::LEFT_TURN)
      lower_list.push_back(point);
    else if (orientation == CGAL::RIGHT_TURN || orientation == CGAL::COLLINEAR)
      upper_list.push_back(point);
  }

  final_list.push_back(lowest_x_point);

  for (auto point : upper_list)
    final_list.push_back(point);

  final_list.push_back(highest_x_point);

  for (auto it = lower_list.end() - 1; it >= lower_list.begin(); it--)
    final_list.push_back(*it);

  final_list.push_back(lowest_x_point);

  return final_list;
}
