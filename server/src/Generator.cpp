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
#include "random.h"

#ifdef DEBUG
#include "deterministic.h"
#endif


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
