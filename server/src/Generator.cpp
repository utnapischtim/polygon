#include <algorithm>
#include <vector>
#include <random>
#include <list>
#include <cstdlib>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>

#include "json.hpp"
#include "easylogging++.h"

#include "Generator.h"
#include "Filter.h"
#include "Point.h"
#include "CommonSetting.h"
#include "SpacePartitioning.h"
#include "random.h"

#ifdef DEBUG
#include "deterministic.h"
#endif

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;
using CGALPoints = std::vector<cgal::Point_2>;

nlohmann::json pl::getListOfGenerators() {
  nlohmann::json obj = {
    // {{"name", "random two peasants"}, {"desc", ""}, {"key", 0}},
    // {{"name", "steady growth"}, {"desc", ""}, {"key", 1}},
    // {{"name", "two opt"}, {"desc", ""}, {"key", 2}},
    // {{"name", "convex layers"}, {"desc", ""}, {"key", 3}},
    {{"name", "convex bottom"}, {"desc", ""}, {"key", 4}},
    {{"name", "space partitioning"}, {"desc", ""}, {"key", 5}},
    // {{"name", "permute and reject"}, {"desc", ""}, {"key", 6}},
    {{"name", "random"}, {"desc", ""}, {"key", 7}}
  };

  return obj;
}

pl::Generator pl::createGenerator(nlohmann::json obj) {
  return pl::Generator(obj);
}

pl::PointList pl::generatePointList(pl::Generator generator, pl::CommonSettingList common_settings, pl::FilterList local_filters) {
#ifdef DEBUG
  pl::PointList random_point_list = pl::det::deterministic(common_settings, {});
#else
  pl::PointList random_point_list = pl::random(common_settings, {});
#endif

  pl::PointList point_list;

  // it would be nice to imlement that without a switch statement!
  // a registration of function on compile time to select on dynamic
  // time would be nice!
  switch (generator.key) {
  case 0:
    point_list = pl::randomTwoPeasants(common_settings, local_filters);
    break;
  case 1:
    break;
  case 2:
    break;
  case 3:
    break;
  case 4:
    point_list = pl::convexBottom(common_settings);
    break;
  case 5:
    point_list = pl::spacePartitioning(random_point_list);
    break;
  case 6:
    break;
  case 7:
    point_list = pl::random(common_settings, local_filters);
    point_list.push_back(point_list[0]);
    break;

    // TODO
    // handle default ;)
  }

  return point_list;
}


pl::PointList pl::randomTwoPeasants(pl::CommonSettingList /*common_settings*/, pl::FilterList /*local_filters*/) {
  // TODO to implement
  pl::PointList k;
  return k;
}

pl::PointList pl::convexBottom(pl::CommonSettingList common_settings) {
  pl::PointList point_list = pl::random(common_settings, {});

  std::sort(point_list.begin(), point_list.end(), [](auto a, auto b) { return a.x < b.x; });
  auto min_x = point_list[0];
  auto max_x = point_list[point_list.size() - 1];

  auto higher_y = min_x.y < max_x.y ? max_x.y : min_x.y;
  auto x_of_lower_y = min_x.y < max_x.y ? min_x.x : min_x.x;

  // get points to create the convex hull bottom
  CGALPoints point_list_y_lower_as_corners = {{min_x.x, min_x.y}, {max_x.x, max_x.y}};
  for (auto point : point_list)
    if (point.y < higher_y)
      point_list_y_lower_as_corners.push_back({point.x, point.y}); // does this copy?

  // to avoid that points with y value between lowest and highest y
  // are not part of the convex bottom hull;
  point_list_y_lower_as_corners.push_back({x_of_lower_y - 1, higher_y});

  CGALPoints convex_bottom_hull;
  CGAL::convex_hull_2(point_list_y_lower_as_corners.begin(),
                      point_list_y_lower_as_corners.end(),
                      std::back_inserter(convex_bottom_hull));

  std::sort(convex_bottom_hull.begin(), convex_bottom_hull.end(), [](auto a, auto b) { return a.x() < b.x(); });

  // remove the point, added before
  convex_bottom_hull.erase(convex_bottom_hull.begin());

  auto it_p_l = point_list.begin();
  auto it_hull = convex_bottom_hull.begin();

  pl::PointList final_list;

  for (; it_p_l != point_list.end(); it_p_l++)
    if (*it_hull != *it_p_l)
      final_list.push_back(*it_p_l);
    else
      it_hull++;

  std::reverse(convex_bottom_hull.begin(), convex_bottom_hull.end());

  for (auto point : convex_bottom_hull)
    final_list.push_back({point.x(), point.y()});

  // the second point of the point_list has to be added to the
  // final_list, to close the polygone. the second and not the first,
  // because the first ist the last from the reversed convex hull
  final_list.push_back({point_list[1].x, point_list[1].y});

  return final_list;
}
