#ifndef REGULARPOLYGON_H_
#define REGULARPOLYGON_H_

#include <iostream>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <docopt.h>

#include "Point.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

namespace pl {

struct RegularPolygonSettings {
  cgal::Point_2 center;

  unsigned node_count;
  double radius;
  double segment_length;
  double winding_number;
  double gamma;
  double rotation_angle;

  RegularPolygonSettings() : center{}, node_count{}, radius{}, segment_length{}, winding_number{}, gamma{}, rotation_angle{} {}
  RegularPolygonSettings(const std::map<std::string, docopt::value> &args);

  void updateNodeCount(size_t nc);
};

PointList regularPolygon(const std::map<std::string, docopt::value> &args);
PointList regularPolygon(const RegularPolygonSettings &rps);

std::ostream &operator<<(std::ostream &out, const RegularPolygonSettings &rps);

}

#endif
