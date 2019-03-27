#ifndef FIXLOCALORIENTATION_H_
#define FIXLOCALORIENTATION_H_

#include <iostream>

#include "CommonSetting.h"
#include "Filter.h"

namespace pl {

struct FixLocalOrientationSettings {
  int reflex_point_count;
  int reflex_chain_max_count;
  std::vector<int> reflex_point_count_per_segment;
  double convex_stretch;
  double iota;
  cgal::Vector_2 distance_to_center_of_reflex_circle;

  FixLocalOrientationSettings() : reflex_point_count{}, reflex_chain_max_count{}, reflex_point_count_per_segment{}, convex_stretch{}, iota{}, distance_to_center_of_reflex_circle{} {}
  FixLocalOrientationSettings(const CommonSettingList &common_settings, const FilterList &filters);
};

PointList fixLocalOrientation(CommonSettingList &common_settings, const FilterList &filters);

std::ostream &operator<<(std::ostream &out, const FixLocalOrientationSettings &flos);

}

#endif
