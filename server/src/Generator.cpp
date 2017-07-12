#include <vector>
#include <random>

#include "json.hpp"
#include "easylogging++.h"

#include "Generator.h"
#include "Filter.h"
#include "Point.h"
#include "CommonSetting.h"

nlohmann::json pl::getListOfGenerators() {
  nlohmann::json obj = {
    // {{"name", "random two peasants"}, {"desc", ""}, {"key", 0}},
    // {{"name", "steady growth"}, {"desc", ""}, {"key", 1}},
    // {{"name", "two opt"}, {"desc", ""}, {"key", 2}},
    // {{"name", "convex layers"}, {"desc", ""}, {"key", 3}},
    // {{"name", "convex bottom"}, {"desc", ""}, {"key", 4}},
    // {{"name", "space partitioning"}, {"desc", ""}, {"key", 5}},
    // {{"name", "permute and reject"}, {"desc", ""}, {"key", 6}},
    {{"name", "random"}, {"desc", ""}, {"key", 7}}
  };

  return obj;
}

pl::Generator pl::createGenerator(nlohmann::json obj) {
  return pl::Generator(obj);
}

pl::PointList pl::generatePointList(pl::Generator generator, pl::CommonSettingList common_settings, pl::FilterList local_filters) {
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
    break;
  case 5:
    break;
  case 6:
    break;
  case 7:
    point_list = pl::random(common_settings, local_filters);
    break;

    // TODO
    // handle default ;)
  }

  return point_list;
}


pl::PointList pl::randomTwoPeasants(pl::CommonSettingList common_settings, pl::FilterList local_filters) {
  // TODO to implement
  pl::PointList k;
  return k;
}

pl::PointList pl::random(pl::CommonSettingList common_settings, pl::FilterList local_filters) {
  // TODO to implement
  pl::PointList point_list;
  auto c_s_sampling_grid = pl::find(common_settings, "sampling grid");
  pl::SamplingGrid sampling_grid(c_s_sampling_grid);

  auto c_s_nodes = pl::find(common_settings, "nodes");

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<>
    dis_width(0, sampling_grid.width),
    dis_height(0, sampling_grid.height);


  for (int i = 0; i < std::stoi(c_s_nodes.val); ++i) {
    auto x = dis_width(gen);
    auto y = dis_height(gen);

    point_list.push_back({x, y});
  }

  // to close the polygon
  point_list.push_back(point_list[0]);

  return point_list;
}
