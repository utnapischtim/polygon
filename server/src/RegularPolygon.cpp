#include <algorithm>
#include <cmath>
#include <tuple>

#include "RegularPolygon.h"
#include "Point.h"
#include "CommonSetting.h"
#include "random.h"


static std::tuple<pl::SamplingGrid, unsigned, double, double> init(const pl::CommonSettingList &common_settings);

pl::PointList pl::regularPolygon(const pl::CommonSettingList &common_settings) {
  pl::PointList final_list;

  auto [sampling_grid, node_count, radius, segment_length] = init(common_settings);

  // if the radius is set to 0
  // search the centre and calculate the possible radius dependent
  // from the centre, otherwise calculate the centre dependent from
  // radius

  // the center could only be placed at a position where it is enough
  // place for the radius
  pl::Point<double> center = {pl::randomValueOfRange(radius, sampling_grid.width-radius), pl::randomValueOfRange(radius, sampling_grid.height-radius)};

  // the centre and the radius are also dependent from the segment
  // length, if the segment length is given, then the rest has to be
  // calculated arround those two values!

  // https://de.wikipedia.org/wiki/Regelm%C3%A4%C3%9Figes_Polygon

  double t = 0; // rotation angle
  double m = 1.0; // winding number
  auto w = 2 * M_PI * m/node_count; // center angle

  for (size_t k = 1; k <= node_count; ++k) {
    auto x = radius * std::cos(k*w+t) + center.x;
    auto y = radius * std::sin(k*w+t) + center.y;

    final_list.push_back({x, y});
  }

  // reverse it, to be in same order as the other algorithm, which
  // build there polygon in clockwise direction
  std::reverse(final_list.begin(), final_list.end());

  final_list.push_back(final_list[0]);


  return final_list;
}

std::tuple<pl::SamplingGrid, unsigned, double, double> init(const pl::CommonSettingList &common_settings) {
  pl::CommonSetting c_s_sampling_grid, c_s_nodes, c_s_radius, c_s_segment_length;

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

  if (auto t = pl::find(common_settings, "radius"))
    c_s_radius = *t;
  else
    c_s_radius = pl::CommonSetting("radius", "", 3, "number", "60");

  if (auto t = pl::find(common_settings, "segment length"))
    c_s_segment_length = *t;
  else
    c_s_segment_length = pl::CommonSetting("segment length", "", 4, "number", "20");

  // TODO:
  // make it robust
  pl::SamplingGrid sampling_grid(c_s_sampling_grid);
  unsigned node_count = std::stoi(c_s_nodes.val);
  double radius = std::stod(c_s_radius.val);
  double segment_length = std::stod(c_s_segment_length.val);

  if (radius < 0)
    throw std::runtime_error("radius should be positive");

  return {sampling_grid, node_count, radius, segment_length};
}

