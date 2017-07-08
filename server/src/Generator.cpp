#include <vector>

#include "json.hpp"
#include "easylogging++.h"

#include "Generator.h"
#include "Filter.h"
#include "Point.h"

nlohmann::json pl::getListOfGenerators() {
  nlohmann::json obj = {
    {{"name", "random two peasants"}, {"desc", ""}, {"key", 0}},
    {{"name", "steady growth"}, {"desc", ""}, {"key", 1}},
    {{"name", "two opt"}, {"desc", ""}, {"key", 2}},
    {{"name", "convex layers"}, {"desc", ""}, {"key", 3}},
    {{"name", "convex bottom"}, {"desc", ""}, {"key", 4}},
    {{"name", "space partitioning"}, {"desc", ""}, {"key", 5}},
    {{"name", "permute and reject"}, {"desc", ""}, {"key", 6}}
  };

  return obj;
}

pl::Generator pl::createGenerator(nlohmann::json obj) {
  return pl::Generator(obj);
}

pl::PointList pl::generatePointList(pl::Generator generator, pl::FilterList local_filters) {
  pl::PointList point_list;

  // it would be nice to imlement that without a switch statement!
  // a registration of function on compile time to select on dynamic
  // time would be nice!
  switch (generator.key) {
  case 0:
    point_list = pl::randomTwoPeasants(local_filters);
    break;
  case 1:
    break;
  case 2:
    break;
  case 3:
    break;
  case 4:
    break;
  case 5:
    break;
  case 6:
    break;

    // TODO
    // handle default ;)
  }

  return point_list;
}


pl::PointList pl::randomTwoPeasants(pl::FilterList local_filters) {
  // TODO to implement
  pl::PointList k;
  return k;
}

