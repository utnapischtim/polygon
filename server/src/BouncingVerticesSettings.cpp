#include <string>
#include <filesystem>

#include <docopt.h>

#include "BouncingVerticesSettings.h"
#include "SamplingGrid.h"
#include "AngleRange.h"

namespace fs = std::filesystem;

static std::string createEveryPhaseDir(const int node_count, const int reflex_point_count) {
  // create for a directory for this node reflex points combo
  std::string directory = "out/bouncing-vertices-" + std::to_string(node_count) + "-" + std::to_string(reflex_point_count) + "-0";

  if (fs::is_directory(directory))
    std::cout << "directory: " << directory << " does just exists and this would cause to override old outputs" << "\n";
  else
    fs::create_directory(directory);

  return directory;
}

pl::BouncingVerticesSettings::BouncingVerticesSettings(const std::map<std::string, docopt::value> &args, size_t point_list_size) : BouncingVerticesSettings{} {

  sampling_grid = SamplingGrid(args.at("--sampling-grid").asString());
  phases = args.at("--phases").asLong();
  bouncing_radius = args.at("--bouncing-radius").asLong();
  bouncing_method = args.at("--bouncing-method").asString();

  reflex_point_count = args.at("--reflex-points").asLong();
  reflex_angle_range = AngleRange(args.at("--reflex-angle-range").asString());
  convex_angle_range = AngleRange(args.at("--convex-angle-range").asString());

  animation = args.at("--animation").asLong() == 1;
  out_every_phase = args.at("--out-every-phase").asLong() == 1;

  keep_orientation = reflex_point_count > -1;
  keep_angle = (180 < reflex_angle_range.lower_bound && reflex_angle_range.upper_bound < 360) && (0 < convex_angle_range.lower_bound && convex_angle_range.upper_bound < 180);

  directory_for_every_phase_out = out_every_phase ? createEveryPhaseDir(point_list_size, reflex_point_count) : "";
}
