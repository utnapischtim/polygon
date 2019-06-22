#include <iostream>
#include <vector>
#include <string>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <docopt.h>

#include "BouncingVertices.h"
#include "BouncingVerticesSettings.h"
#include "CalculateBouncingRange.h"
#include "CalculateMovingPermit.h"
#include "Point.h"
#include "Output.h"
#include "cgal_helper.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

using Segments = std::vector<cgal::Segment_2>;

static Segments buildSegments(const pl::PointList &point_list);
static void createAnimationOutput(const pl::BouncingVerticesSettings &bvs, const Segments &segments, const int phase);
static void createOutputForPhase(const pl::BouncingVerticesSettings &bvs, const Segments &segments, const int phase);

pl::PointList pl::bouncingVertices(const pl::PointList &point_list, const std::map<std::string, docopt::value> &args) {

  pl::BouncingVerticesSettings bvs(args, point_list.size());
  Segments segments = buildSegments(point_list);

  CalculateBouncingRange cbr{bvs, segments};
  CalculateMovingPermit cmp{bvs, segments};
  int i = 1;
  for (int phase = 0; phase < bvs.phases; ++phase) {
    // not use next or prev because this would cause a endles loop
    for (auto sit = segments.begin(); sit != segments.end(); ++sit) {
      if (bvs.bouncing_method == "check-requirements-after-point-creation")
        cmp.bounce(sit);
      else if (bvs.bouncing_method == "calculate-possible-bouncing-radius-before")
        cbr.bounce(sit);


      createOutputForPhase(bvs, segments, i);
      i += 1;
    }

    if (bvs.animation)
      createAnimationOutput(bvs, segments, phase);

    if (bvs.out_every_phase)
      createOutputForPhase(bvs, segments, phase);
  }

  pl::PointList final_list;
  pl::convert(segments, final_list);

  return final_list;
}

void createAnimationOutput([[maybe_unused]] const pl::BouncingVerticesSettings &bvs, [[maybe_unused]] const Segments &segments, [[maybe_unused]] const int phase) {
#ifdef MAGICK
  std::string filename = "out/animation/" + std::to_string(phase) + ".png";
  pl::PointList list;
  pl::convert(segments, list);
  pl::output(list, "png", filename, bvs.sampling_grid, phase);
#else
  std::cout << "animation feature was not compiled in" << "\n";
  std::exit(-1);
#endif
}

void createOutputForPhase(const pl::BouncingVerticesSettings &bvs, const Segments &segments, const int phase) {
  std::string filename = bvs.directory_for_every_phase_out + "/" + std::to_string(phase) + ".dat";

  pl::PointList list;
  pl::convert(segments, list);
  pl::output(list, "gnuplot", filename);
}

Segments buildSegments(const pl::PointList &point_list) {
  Segments segments;

  for (size_t i = 1, size = point_list.size(); i < size; ++i)
    segments.push_back({point_list[i-1], point_list[i]});

  // close the polygon
  segments.push_back({segments[segments.size() - 1].target(), segments[0].source()});

  return segments;
}

