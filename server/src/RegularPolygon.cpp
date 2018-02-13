#include <algorithm>
#include <cmath>
#include <tuple>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "RegularPolygon.h"
#include "Point.h"
#include "CommonSetting.h"
#include "random.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;


struct RegularPolygonSettings {
  cgal::Point_2 center;

  unsigned node_count;
  double radius;
  double segment_length;
  double winding_number;
  double gamma;
  double rotation_angle;

  RegularPolygonSettings() : center{}, node_count{}, radius{}, segment_length{}, winding_number{}, gamma{}, rotation_angle{} {}
};

static RegularPolygonSettings buildRegularPolygonSettings(const pl::CommonSettingList &common_settings);
static pl::PointList calculateRegularPolygonPoints(const RegularPolygonSettings &rps);

pl::PointList pl::regularPolygon(pl::CommonSettingList &common_settings) {
  RegularPolygonSettings rps = buildRegularPolygonSettings(common_settings);
  pl::PointList final_list = calculateRegularPolygonPoints(rps);

  // set values for potentially other algorithmns who use this
  // algorithm for initialization
  setValue(common_settings, "segment length", std::to_string(rps.segment_length));
  setValue(common_settings, "center", pl::to_string(rps.center));

  // reverse it, to be in same order as the other algorithm, which
  // build there polygon in clockwise direction
  std::reverse(final_list.begin(), final_list.end());

  final_list.push_back(final_list[0]);

  return final_list;
}

// https://de.wikipedia.org/wiki/Regelm%C3%A4%C3%9Figes_Polygon
pl::PointList calculateRegularPolygonPoints(const RegularPolygonSettings &rps) {
  pl::PointList final_list;

  for (size_t seg = 1; seg <= rps.node_count; ++seg) {
    auto x = rps.radius * std::cos(seg * rps.gamma + rps.rotation_angle) + rps.center.x();
    auto y = rps.radius * std::sin(seg * rps.gamma + rps.rotation_angle) + rps.center.y();

    final_list.push_back({x, y});
  }

  return final_list;
}

pl::SamplingGrid getSamplingGrid(const pl::CommonSettingList &common_settings) {
  pl::SamplingGrid sampling_grid;

  if (auto t = pl::find(common_settings, "sampling grid"))
    sampling_grid = {*t};
  else {
    std::string msg = std::string("essential common setting 'sampling grid' not set to generate random pointList");
    throw std::runtime_error(msg);
  }

  return sampling_grid;
}

unsigned getNodeCount(const pl::CommonSettingList &common_settings) {
  unsigned node_count;

  if (auto t = pl::find(common_settings, "nodes"))
    node_count = std::stoi((*t).val);
  else {
    std::string msg = std::string("essential common setting nodes not set to generate random pointList");
    throw std::runtime_error(msg);
  }

  return node_count;
}

double getWindingNumber(const pl::CommonSettingList &common_settings) {
  double winding_number = 1.0;

  if (auto t = pl::find(common_settings, "winding number"))
    winding_number = std::stod((*t).val);

  return winding_number;
}


double calculateSegmentLength(const pl::CommonSettingList &common_settings, double radius, double gamma) {
  double segment_length = 0;

  if (auto t = pl::find(common_settings, "segment length")) {
    segment_length = std::stod((*t).val);
  } else {
    // TODO:
    // maybe there is a better place to do that the segment length is
    // calculated in this function, because it belongs to
    // regularPolygon and it could be possible that this function
    // could be extended to calculated the radius with a segment
    // length and the node counts it is also here, because of the
    // winding_number!!
    segment_length = 2 * radius * std::sin(gamma/2);
  }

  return segment_length;
}


double getRadius(const pl::CommonSettingList &common_settings) {
  // the centre and the radius are also dependent from the segment
  // length, if the segment length is given, then the rest has to be
  // calculated arround those two values!

  double radius = 60; // default value

  if (auto t = pl::find(common_settings, "radius"))
    radius = std::stod((*t).val);

  if (radius < 0)
    throw std::runtime_error("radius should be positive");

  return radius;
}

cgal::Point_2 calculateCenter(const double radius, const pl::SamplingGrid &sampling_grid) {
  // if the radius is set to 0
  // search the centre and calculate the possible radius dependent
  // from the centre, otherwise calculate the centre dependent from
  // radius

  // the center could only be placed at a position where it is enough
  // place for the radius
  double
    x = pl::randomValueOfRange(radius, sampling_grid.width - radius),
    y = pl::randomValueOfRange(radius, sampling_grid.height - radius);

  return {x, y};
}

RegularPolygonSettings buildRegularPolygonSettings(const pl::CommonSettingList &common_settings) {
  RegularPolygonSettings rps;

  pl::SamplingGrid sampling_grid = getSamplingGrid(common_settings);

  rps.winding_number = getWindingNumber(common_settings);
  rps.node_count = getNodeCount(common_settings);
  rps.gamma = 2 * M_PI * rps.winding_number / rps.node_count;
  rps.radius = getRadius(common_settings);
  rps.center = calculateCenter(rps.radius, sampling_grid);

  // this has the effect, that the middle line of the first segment
  // lies on the positive x-axis
  rps.rotation_angle = - rps.gamma / 2;
  rps.segment_length = calculateSegmentLength(common_settings, rps.radius, rps.gamma);

  return rps;
}
