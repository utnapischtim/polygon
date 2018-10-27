#include <algorithm>
#include <random>
#include <string>

#include "CommonSetting.h"
#include "SamplingGrid.h"
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
  pl::PointList point_list;
  pl::CommonSetting c_s_sampling_grid, c_s_nodes;

  if (auto t = pl::find(common_settings, "sampling grid"))
    c_s_sampling_grid = *t;
  else {
    std::string msg = std::string("essential common setting 'sampling grid' not set to generate random pointList");
    throw std::runtime_error(msg);
  }

  if (auto t = pl::find(common_settings, "nodes"))
    c_s_nodes = *t;
  else {
    std::string msg = std::string("essential common setting 'nodes not set to generate random pointList");
    throw std::runtime_error(msg);
  }

  // TODO:
  // make it robust
  pl::SamplingGrid sampling_grid(c_s_sampling_grid);
  unsigned node_count = std::stoi(c_s_nodes.val);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<>
    dis_width(0, sampling_grid.width),
    dis_height(0, sampling_grid.height);

  for (size_t i = 0; i < node_count; ++i) {
    auto x = dis_width(gen);
    auto y = dis_height(gen);

    point_list.push_back({x, y});
  }

  return point_list;
}
