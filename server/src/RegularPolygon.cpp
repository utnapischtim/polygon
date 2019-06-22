#include <algorithm>
#include <cmath>
#include <tuple>
#include <iostream>
#include <iomanip>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <docopt.h>

#include "RegularPolygon.h"
#include "Point.h"
#include "SamplingGrid.h"
#include "random.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

static pl::PointList calculateRegularPolygonPoints(const pl::RegularPolygonSettings &rps);

pl::PointList pl::regularPolygon(const std::map<std::string, docopt::value> &args) {
  RegularPolygonSettings rps(args);
  pl::PointList final_list = pl::regularPolygon(rps);
  return final_list;
}

pl::PointList pl::regularPolygon(const pl::RegularPolygonSettings &rps) {
  pl::PointList final_list = calculateRegularPolygonPoints(rps);

  // reverse it, to be in same order as the other algorithm, which
  // build there polygon in clockwise direction
  std::reverse(final_list.begin(), final_list.end());
  final_list.push_back(final_list[0]);

  return final_list;
}

// https://de.wikipedia.org/wiki/Regelm%C3%A4%C3%9Figes_Polygon
pl::PointList calculateRegularPolygonPoints(const pl::RegularPolygonSettings &rps) {
  pl::PointList final_list;

  for (size_t seg = 1; seg <= rps.node_count; ++seg) {
    auto x = rps.radius * std::cos(seg * rps.gamma + rps.rotation_angle) + rps.center.x();
    auto y = rps.radius * std::sin(seg * rps.gamma + rps.rotation_angle) + rps.center.y();

    final_list.push_back({x, y});
  }

  return final_list;
}

double calculateSegmentLength(const std::map<std::string, docopt::value> &args, double radius, double gamma) {
  double segment_length = 0;

  if (double length = args.at("--segment-length").asLong(); 0 < length) {
    segment_length = length;
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

pl::RegularPolygonSettings::RegularPolygonSettings(const std::map<std::string, docopt::value> &args)
  : RegularPolygonSettings{}
{
  pl::SamplingGrid sampling_grid(args.at("--sampling-grid").asString());

  winding_number = args.at("--winding-number").asLong();
  node_count = args.at("--nodes").asLong();
  gamma = 2 * M_PI * winding_number / node_count;
  radius = args.at("--radius").asLong();
  center = calculateCenter(radius, sampling_grid);


  // this has the effect, that the middle line of the first segment
  // lies on the positive x-axis
  rotation_angle = - gamma / 2;
  segment_length = calculateSegmentLength(args, radius, gamma);
}

void pl::RegularPolygonSettings::updateNodeCount(const size_t nc) {
  node_count = nc;
  // following two values depend on node_count and have to be updated
  // accordingly
  gamma = 2 * M_PI * winding_number / node_count;
  rotation_angle = - gamma / 2;
}

std::ostream &pl::operator<<(std::ostream &out, const pl::RegularPolygonSettings &rps) {
  out << "  rps.node_count: "     << std::setw(7) << rps.node_count << '\n'
      << "  rps.radius: "         << std::setw(11) << rps.radius << '\n'
      << "  rps.segment_length: " << std::setw(2) << rps.segment_length << '\n'
      << "  rps.winding_number: " << std::setw(2) << rps.winding_number << '\n'
      << "  rps.gamma: "          << std::setw(12) << rps.gamma << '\n'
      << "  rps.rotation_angle: " << std::setw(2) << rps.rotation_angle << '\n'
      << "  rps.center: "         << std::setw(11) << rps.center << '\n';
  return out;
}
