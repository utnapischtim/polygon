#ifndef BOUNCINGVERTICESSETTINGS_H_
#define BOUNCINGVERTICESSETTINGS_H_

#include <string>

#include "CommonSetting.h"
#include "SamplingGrid.h"
#include "Filter.h"

namespace pl {

struct BouncingVerticesSettings {
  pl::SamplingGrid sampling_grid;
  int phases;
  double bouncing_radius;
  std::string bouncing_method;

  int reflex_point_count;
  Filter reflex_angle_range;
  Filter convex_angle_range;

  bool animation;
  bool out_every_phase;
  bool keep_orientation;
  bool keep_angle;

  std::string directory_for_every_phase_out;

  BouncingVerticesSettings() :
    sampling_grid{}, phases{}, bouncing_radius{}, bouncing_method{},
    reflex_point_count{}, reflex_angle_range{}, convex_angle_range{},
    animation{}, out_every_phase{}, keep_orientation{}, keep_angle{},
    directory_for_every_phase_out{}
    {}

  BouncingVerticesSettings(const CommonSettingList &common_settings, const pl::FilterList &filters, size_t point_list_size);
};

}

#endif
