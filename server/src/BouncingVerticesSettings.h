#ifndef BOUNCINGVERTICESSETTINGS_H_
#define BOUNCINGVERTICESSETTINGS_H_

#include <string>

#include <docopt.h>

#include "SamplingGrid.h"
#include "AngleRange.h"

namespace pl {

struct BouncingVerticesSettings {
  pl::SamplingGrid sampling_grid;
  int phases;
  double bouncing_radius;
  std::string bouncing_method;

  int reflex_point_count;
  AngleRange reflex_angle_range;
  AngleRange convex_angle_range;

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

  BouncingVerticesSettings(const std::map<std::string, docopt::value> &args, size_t point_list_size);
};

}

#endif
