#ifndef FIXLOCALORIENTATION_H_
#define FIXLOCALORIENTATION_H_

#include <iostream>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <docopt.h>

#include "Point.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

namespace pl {

struct FixLocalOrientationSettings {
  int reflex_point_count;
  int reflex_chain_max_count;
  std::vector<int> reflex_point_count_per_segment;
  double convex_stretch;
  double iota;
  cgal::Vector_2 distance_to_center_of_reflex_circle;

  FixLocalOrientationSettings() : reflex_point_count{}, reflex_chain_max_count{}, reflex_point_count_per_segment{}, convex_stretch{}, iota{}, distance_to_center_of_reflex_circle{} {}
  FixLocalOrientationSettings(const std::map<std::string, docopt::value> &args);
};

PointList fixLocalOrientation(const std::map<std::string, docopt::value> &args);

std::ostream &operator<<(std::ostream &out, const FixLocalOrientationSettings &flos);

}

#endif
