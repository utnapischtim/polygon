#include <algorithm>
#include <random>

#include "CommonSetting.h"
#include "Filter.h"
#include "Point.h"
#include "random.h"


double pl::randomValueOfRange(double start, double end) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(start, end);

  return dis(gen);
}

int pl::randomValueOfRange(int start, int end) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(start, end);

  return dis(gen);
}

unsigned randomValueOfRange(unsigned start, unsigned end) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(start, end);

  return dis(gen);
}

pl::PointList pl::random(pl::CommonSettingList common_settings, pl::FilterList /*local_filters*/) {
  // TODO to implement
  pl::PointList point_list;
  auto c_s_sampling_grid = pl::find(common_settings, "sampling grid");
  pl::SamplingGrid sampling_grid(c_s_sampling_grid);

  auto c_s_nodes = pl::find(common_settings, "nodes");

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<>
    dis_width(0, sampling_grid.width),
    dis_height(0, sampling_grid.height);

  for (int i = 0; i < std::stoi(c_s_nodes.val); ++i) {
    auto x = dis_width(gen);
    auto y = dis_height(gen);

    point_list.push_back({x, y});
  }

  return point_list;
}
