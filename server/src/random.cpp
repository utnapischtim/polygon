#include <algorithm>
#include <random>
#include <string>

#include <docopt.h>

#include "SamplingGrid.h"
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

pl::PointList pl::random(const std::map<std::string, docopt::value> &args) {
  pl::PointList point_list;

  pl::SamplingGrid sampling_grid(args.at("--sampling-grid").asString());
  unsigned node_count = args.at("--nodes").asLong();

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
