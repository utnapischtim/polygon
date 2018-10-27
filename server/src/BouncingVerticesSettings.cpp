#include <string>
#include <filesystem>

#include "easylogging++.h"

#include "BouncingVerticesSettings.h"
#include "CommonSetting.h"
#include "SamplingGrid.h"
#include "Filter.h"

namespace fs = std::filesystem;

static pl::SamplingGrid getSamplingGrid(const pl::CommonSettingList &common_settings) {
  pl::SamplingGrid sampling_grid{100,80};

  if (auto t = pl::find(common_settings, "sampling grid"))
    sampling_grid = {*t};

  return sampling_grid;
}

static int getPhases(const pl::CommonSettingList &common_settings) {
  int phases = 10;

  if (auto t = pl::find(common_settings, "phases"))
    phases = std::stoi((*t).val);

  return phases;
}

static double getBouncingRadius(const pl::CommonSettingList &common_settings) {
  double radius = 60;

  if (auto t = pl::find(common_settings, "bouncing radius"))
    radius = std::stod((*t).val);

  if (auto t = pl::find(common_settings, "segment length"); t && radius < 1)
    radius = std::stod((*t).val) / 2;

  return radius;
}

static bool getAnimation(const pl::CommonSettingList &common_settings) {
  bool animation = false;

  if (auto t = pl::find(common_settings, "animation"))
    animation  = std::stoi((*t).val) == 1;

  return animation;
}

static bool getOutEveryPhase(const pl::CommonSettingList &common_settings) {
  bool out_every_phase = false;

  if (auto t = pl::find(common_settings, "out every phase"))
    out_every_phase = std::stoi((*t).val) == 1;

  return out_every_phase;
}

static int getReflexPointCount(const pl::FilterList &filters) {
  int reflex_point_count = 0;

  if (auto t = pl::find(filters, "reflex points"))
    reflex_point_count = (*t).val;

  return reflex_point_count;
}

static pl::Filter getReflexAngleRange(const pl::FilterList &filters) {
  pl::Filter reflex_angle_range;

  if (auto t = pl::find(filters, "reflex angle range"))
    reflex_angle_range = *t;

  return reflex_angle_range;
}

static pl::Filter getConvexAngleRange(const pl::FilterList &filters) {
  pl::Filter convex_angle_range;

  if (auto t = pl::find(filters, "convex angle range"))
    convex_angle_range = *t;

  return convex_angle_range;
}

static std::string createEveryPhaseDir(const int node_count, const int reflex_point_count) {
  // create for a directory for this node reflex points combo
  std::string directory = "out/bouncing-vertices-" + std::to_string(node_count) + "-" + std::to_string(reflex_point_count) + "-0";

  if (fs::is_directory(directory))
    std::cout << "directory: " << directory << " does just exists and this would cause to override old outputs" << "\n";
  else
    fs::create_directory(directory);

  return directory;
}

pl::BouncingVerticesSettings::BouncingVerticesSettings(const pl::CommonSettingList &common_settings, const pl::FilterList &filters, size_t point_list_size) : BouncingVerticesSettings{} {
  VLOG(3) << "BouncingVerticesSettings";

  sampling_grid = getSamplingGrid(common_settings);
  phases = getPhases(common_settings);
  bouncing_radius = getBouncingRadius(common_settings);
  //bouncing_method = "check-requirements-after-point-creation";
  bouncing_method = "calculate-possible-bouncing-radius-before";

  reflex_point_count = getReflexPointCount(filters);
  reflex_angle_range = getReflexAngleRange(filters);
  convex_angle_range = getConvexAngleRange(filters);

  animation = getAnimation(common_settings);
  out_every_phase = getOutEveryPhase(common_settings);
  keep_orientation = reflex_point_count > -1;
  keep_angle = (reflex_angle_range.lower_bound > 0 && reflex_angle_range.upper_bound > 0) || (convex_angle_range.lower_bound > 0 && convex_angle_range.upper_bound > 0);

  directory_for_every_phase_out = out_every_phase ? createEveryPhaseDir(point_list_size, reflex_point_count) : "";
}
