#ifndef BOUNCINGVERTICES_H_
#define BOUNCINGVERTICES_H_

#include <string>

#include "Point.h"
#include "CommonSetting.h"
#include "Filter.h"

namespace pl {

PointList bouncingVertices(const PointList &point_list, const CommonSettingList &common_settings, const FilterList &filter);

struct BouncingVerticesSettings {
  pl::SamplingGrid sampling_grid;
  int phases;
  double bouncing_radius;

  int reflex_point_count;
  Filter reflex_angle_range;
  Filter convex_angle_range;

  bool animation;
  bool out_every_phase;
  bool keep_orientation;
  bool do_orientation_filter;

  std::string directory_for_every_phase_out;

  BouncingVerticesSettings() :
    sampling_grid{}, phases{}, bouncing_radius{},
    reflex_point_count{}, reflex_angle_range{}, convex_angle_range{},
    animation{}, out_every_phase{}, keep_orientation{}, do_orientation_filter{},
    directory_for_every_phase_out{}
    {}

  BouncingVerticesSettings(const CommonSettingList &common_settings, const pl::FilterList &filters, size_t point_list_size);
};

}

#endif
