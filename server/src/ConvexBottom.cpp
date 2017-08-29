#include <algorithm>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>

#include "ConvexBottom.h"
#include "Point.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

pl::PointList pl::convexBottom(pl::PointList point_list) {
  std::sort(point_list.begin(), point_list.end(), [](auto a, auto b) { return a.x < b.x; });

  std::vector<cgal::Point_2> point_2_list;
  pl::convert(point_2_list, point_list);

  // possible due the sorting above
  auto min_x = point_2_list[0];
  auto max_x = point_2_list[point_2_list.size() - 1];

  // get points to create the bottom convex hull
  std::vector<cgal::Point_2> point_2_list_bottom = {min_x, max_x};
  for (auto point : point_2_list) {
    auto orientation = CGAL::orientation(min_x, max_x, point);

    if (orientation == CGAL::RIGHT_TURN)
      point_2_list_bottom.push_back(point);
  }

  // create the bottom convex hull
  std::vector<cgal::Point_2> convex_bottom_hull;
  CGAL::convex_hull_2(point_2_list_bottom.begin(),
                      point_2_list_bottom.end(),
                      std::back_inserter(convex_bottom_hull));


  pl::PointList final_list;

  auto it_p_l = point_2_list.begin();
  auto it_hull = convex_bottom_hull.begin();

  // build the upper polygon, add points to the final point list,
  // which are not in the convex bottom hull
  for (; it_p_l != point_2_list.end(); it_p_l++)
    if (*it_hull != *it_p_l)
      final_list.push_back({it_p_l->x(), it_p_l->y()});
    else
      it_hull++;

  // add the convex bottom hull to the point list to close the polygon
  std::reverse(convex_bottom_hull.begin(), convex_bottom_hull.end());
  for (auto point : convex_bottom_hull)
    final_list.push_back({point.x(), point.y()});

  final_list.push_back(final_list[0]);

  return final_list;
}
