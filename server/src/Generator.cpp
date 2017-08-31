#include <algorithm>
#include <vector>
#include <random>
#include <list>
#include <cstdlib>

#include "json.hpp"
#include "easylogging++.h"

#include "Generator.h"
#include "Filter.h"
#include "Point.h"
#include "CommonSetting.h"
#include "SpacePartitioning.h"
#include "ConvexBottom.h"
#include "SteadyGrowth.h"
#include "TwoOptMoves.h"
#include "RandomTwoPeasants.h"
#include "BouncingVertices.h"
#include "RegularPolygon.h"
#include "random.h"

#ifdef DEBUG
#include "deterministic.h"
#endif


nlohmann::json pl::getListOfGenerators() {
  nlohmann::json obj = {
    {{"name", "random two peasants"}, {"desc", ""}, {"key", 0}},
    {{"name", "steady growth"}, {"desc", ""}, {"key", 1}},
    {{"name", "two opt moves"}, {"desc", ""}, {"key", 2}},
    // {{"name", "convex layers"}, {"desc", ""}, {"key", 3}},
    {{"name", "convex bottom"}, {"desc", ""}, {"key", 4}},
    {{"name", "space partitioning"}, {"desc", ""}, {"key", 5}},
    // {{"name", "permute and reject"}, {"desc", ""}, {"key", 6}},
    {{"name", "random"}, {"desc", ""}, {"key", 7}},
    {{"name", "bouncing vertices"}, {"desc", ""}, {"key", 8}},
    {{"name", "regular polygon"}, {"desc", ""}, {"key", 9}}
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
    point_list = pl::randomTwoPeasants(random_point_list);
    break;
  case 1:
    point_list = pl::steadyGrowth(random_point_list);
    break;
  case 2: {
    size_t counter = 0;
    for (bool exit = false; !exit;)
      try {
        point_list = pl::twoOptMoves(random_point_list);
        exit = true;
      } catch (const std::runtime_error &e) {
        random_point_list = pl::random(common_settings, {});
        counter += 1;
      }
    VLOG(2) << "Generator generatePointList two opt moves counter: " << counter;
  }
    break;
  case 3:
    break;
  case 4:
    point_list = pl::convexBottom(random_point_list);
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
  case 8:

#ifdef DEBUG
    point_list = pl::det::deterministic(common_settings, {});
#else
    // TODO would be nice to have a choice here!
    //point_list = pl::convexBottom(random_point_list);
    point_list = pl::regularPolygon(common_settings);
#endif

    // remove the last point, it is the same as the beginning.
    point_list.pop_back();

    point_list = pl::bouncingVertices(point_list, common_settings, local_filters);
    break;
  case 9:
    point_list = pl::regularPolygon(common_settings);
    break;
    // TODO
    // handle default ;)
  }

  return point_list;
}
