#include <iostream>
#include <tuple>
#include <limits>
#include <cstdlib>
#include <iterator>
#include <vector>
#include <string>
#include <cmath>
#include <experimental/filesystem>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "BouncingVertices.h"
#include "Point.h"
#include "CommonSetting.h"
#include "Filter.h"
#include "random.h"
#include "Output.h"

namespace fs = std::experimental::filesystem;

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

using Segments = std::vector<cgal::Segment_2>;
using SegmentsIterator = Segments::iterator;

const size_t MAX_CYCLES = 1000;

static Segments buildSegments(const pl::PointList &point_list);
static void runBouncing(const pl::BouncingVerticesSettings &bvs, Segments &segments, SegmentsIterator &sit);
static void createAnimationOutput(const pl::BouncingVerticesSettings &bvs, const Segments &segments, const int phase);
static void createOutputForPhase(const pl::BouncingVerticesSettings &bvs, const Segments &segments, const int phase);

pl::PointList pl::bouncingVertices(const pl::PointList &point_list, const pl::CommonSettingList &common_settings, const pl::FilterList &filters) {
  pl::BouncingVerticesSettings bvs(common_settings, filters, point_list.size());
  Segments segments = buildSegments(point_list);

  for (int phase = 0; phase < bvs.phases; ++phase) {
    // not use next or prev because this would cause a endles loop
    for (auto sit = segments.begin(); sit != segments.end(); ++sit)
      runBouncing(bvs, segments, sit);

    if (bvs.animation)
      createAnimationOutput(bvs, segments, phase);

    if (bvs.out_every_phase)
      createOutputForPhase(bvs, segments, phase);
  }

  pl::PointList final_list;
  pl::convert(segments, final_list);

  return final_list;
}

static Segments::iterator next(Segments &segments, const Segments::iterator &it) {
  auto it_n = it + 1;

  if (it_n == segments.end())
    it_n = segments.begin();

  return it_n;
}

static Segments::iterator prev(Segments &segments, const Segments::iterator &it) {
  Segments::iterator it_p;

  if (it == segments.begin())
    it_p = segments.end() - 1;
  else
    it_p = it - 1;

  return it_p;
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

static std::string createEveryPhaseDir(const int node_count, const int reflex_point_count) {
  // create for a directory for this node reflex points combo
  std::string directory = "out/bouncing-vertices-" + std::to_string(node_count) + "-" + std::to_string(reflex_point_count) + "-0";

  if (fs::is_directory(directory))
    std::cout << "directory: " << directory << " does just exists and this would cause to override old outputs" << "\n";
  else
    fs::create_directory(directory);

  return directory;
}

static cgal::Point_2 createPointInsideArea(const pl::SamplingGrid &sampling_grid, const double radius, const Segments::iterator &sit) {
  cgal::Point_2 shifted_point;

  do {
    cgal::Point_2 velocity = {pl::randomValueOfRange(-radius, radius), pl::randomValueOfRange(-radius, radius)};

    // check if the shifted_point of the current segment is inside
    // of the boundary box, the shifted_point is also the source
    // of the next segment, therefore the next segment does not
    // have to be checked.
    shifted_point = {sit->target().x() + velocity.x(), sit->target().y() + velocity.y()};
  } while (sampling_grid.isOutOfArea(shifted_point));

  return shifted_point;
}

static bool insideOrientationArea(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, const cgal::Segment_2 &e_1_old, const cgal::Segment_2 &e_2_old, const cgal::Segment_2 &next, const cgal::Segment_2 &prev) {
  return
    CGAL::orientation(prev.source(), e_1.source(), e_1.target()) == CGAL::orientation(prev.source(), e_1_old.source(), e_1_old.target()) &&
    CGAL::orientation(e_1.source(), e_1.target(), e_2.target()) == CGAL::orientation(e_1_old.source(), e_1_old.target(), e_2_old.target()) &&
    CGAL::orientation(e_2.source(), e_2.target(), next.target()) == CGAL::orientation(e_2_old.source(), e_2_old.target(), next.target());
}

static double calculateAngle(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2) {
  cgal::Vector_2
    u = e_1.source() - e_1.target(),
    v = e_2.target() - e_2.source();

  // angle = arccos((u * v) / (||u|| * ||v||))
  double radian_angle = std::acos((u*v) / (std::sqrt(u.squared_length()) * std::sqrt(v.squared_length())));

  return (radian_angle * 180) / M_PI;
}

static bool insideAngleRange(const cgal::Segment_2 &e_1_new, const cgal::Segment_2 &e_2_new, const cgal::Segment_2 &e_1_old, const cgal::Segment_2 &e_2_old, const cgal::Segment_2 &next, const cgal::Segment_2 &prev, const pl::BouncingVerticesSettings &bvs) {
  // it could only be left or right turn, because
  // insideOrientationArea allows only that!
  auto isInBoundaries = [&](CGAL::Orientation orientation, double angle) {
    double lower_bound, upper_bound;

    if (orientation == CGAL::LEFT_TURN) {
      lower_bound = bvs.reflex_angle_range.lower_bound;
      upper_bound = bvs.reflex_angle_range.upper_bound;
    } else {
      lower_bound = bvs.convex_angle_range.lower_bound;
      upper_bound = bvs.convex_angle_range.upper_bound;
    }

    return lower_bound < angle && angle < upper_bound;
  };

  auto isInside = [&](const cgal::Point_2 &p1, const cgal::Point_2 &p2, const cgal::Point_2 &p3, const cgal::Segment_2 &first, const cgal::Segment_2 &second) {
    CGAL::Orientation orientation = CGAL::orientation(p1, p2, p3);
    double angle = calculateAngle(first, second);
    if (CGAL::left_turn(p1, p2, p3))
      angle = 360 - angle;
    return isInBoundaries(orientation, angle);
  };


  bool is_inside;
  cgal::Segment_2 e_1, e_2;
  if (bvs.keep_orientation) {
    e_1 = e_1_old;
    e_2 = e_2_old;
  } else {
    e_1 = e_1_new;
    e_2 = e_2_new;
  }

  is_inside = isInside(prev.source(), prev.target(), e_1.target(), prev, e_1_new) &&
              isInside(e_1.source(), e_1.target(), e_2.target(), e_1_new, e_2_new) &&
              isInside(e_2.source(), e_2.target(), next.target(), e_2_new, next);

  return is_inside;
}

// *ssit != old_e_1 && *ssit != old_e_2, the new segments
//   should not be checked about intersection with the old,
//   because per def they should intersect
// ssit->target() != e_1.source() && ssit->source() != e_2.target()
//   the binding points between the segments does not count
//   as intersections. and this construct should work in
//   both directions, clockwise and anti-clockwise
// but all elements which are not e_1 respectively e_2
//   should be checked. because if not a intersection
//   between e_1 respectively e_2 could be overlooked
// ssit->target != e_1.source() is not enough, because e_1
//   could also lie on ssit and this is also not acceptable
//   therefore if those two are connected, than the
//   intersection_occur variable has to be set with the two
//   collinearity checks
// ssit->source != e_2.target the same as with e_1
static bool isIntersection(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, Segments &segments, const Segments::iterator &sit) {
  bool intersection_occur = false;

  cgal::Segment_2 old_e_1 = *sit, old_e_2 = *next(segments, sit);

  for (auto ssit = segments.begin(); ssit != segments.end() && !intersection_occur; ++ssit)
    if (*ssit != old_e_1 && *ssit != old_e_2) {
      if (ssit->target() == e_1.source())
        intersection_occur = intersection_occur || CGAL::orientation(e_1.target(), (*ssit).source(), e_1.source()) == CGAL::COLLINEAR || CGAL::orientation((*ssit).source(), e_1.target(), e_1.source()) == CGAL::COLLINEAR;
      else
        intersection_occur = intersection_occur || CGAL::do_intersect(*ssit, e_1);

      if (ssit->source() == e_2.target())
        intersection_occur = intersection_occur || CGAL::orientation(e_2.source(), e_2.target(), (*ssit).target()) == CGAL::COLLINEAR || CGAL::orientation(e_2.source(), (*ssit).target(), e_2.target()) == CGAL::COLLINEAR;
      else
        intersection_occur = intersection_occur || CGAL::do_intersect(*ssit, e_2);
    }

  return intersection_occur;
}


void runBouncing(const pl::BouncingVerticesSettings &bvs, Segments &segments, SegmentsIterator &sit) {
  bool
    intersection_occur = false,
    outside_orientation_area = false,
    out_of_angle_range = false;

  cgal::Point_2 shifted_point;

  Segments::iterator
    sitp = prev(segments, sit),
    sitn = next(segments, sit),
    sitnn = next(segments, sitn);

  cgal::Segment_2 e_1, e_2, e_1_old = *sit, e_2_old = *sitn;

  size_t count_cycles = 0;

  do {
    // reset to false, because point search starts new
    intersection_occur = false;
    outside_orientation_area = false;
    out_of_angle_range = false;

    shifted_point = createPointInsideArea(bvs.sampling_grid, bvs.bouncing_radius, sit);

    e_1 = {sit->source(), shifted_point};
    e_2 = {shifted_point, sitn->target()};

    if (bvs.do_orientation_filter)
      outside_orientation_area = !insideOrientationArea(e_1, e_2, e_1_old, e_2_old, *sitnn, *sitp);

    // the angle should only be checked, if there are reflex nodes
    // to protect and if the point is not outside of the
    // orientation area!
    if (!outside_orientation_area)
      out_of_angle_range = !insideAngleRange(e_1, e_2, e_1_old, e_2_old, *sitnn, *sitp, bvs);

    if (!outside_orientation_area && !out_of_angle_range)
      intersection_occur = isIntersection(e_1, e_2, segments, sit);

    // this construct has the benefit, that the search does not
    // run in an endless loop if it not find a valid point, and
    // skip this point and redo it on the next phase
    if (MAX_CYCLES < count_cycles) {
      outside_orientation_area = false;
      out_of_angle_range = false;
      intersection_occur = false;
    }
    else
      count_cycles += 1;

  } while (outside_orientation_area || out_of_angle_range || intersection_occur);

  if (count_cycles <= MAX_CYCLES) {
    // set the new segments;
    *sit = e_1;
    *sitn = e_2;
  }
}

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

pl::BouncingVerticesSettings::BouncingVerticesSettings(const pl::CommonSettingList &common_settings, const pl::FilterList &filters, size_t point_list_size) : BouncingVerticesSettings{} {
  sampling_grid = getSamplingGrid(common_settings);
  phases = getPhases(common_settings);
  bouncing_radius = getBouncingRadius(common_settings);

  reflex_point_count = getReflexPointCount(filters);
  reflex_angle_range = getReflexAngleRange(filters);
  convex_angle_range = getConvexAngleRange(filters);

  animation = getAnimation(common_settings);
  out_every_phase = getOutEveryPhase(common_settings);
  keep_orientation = reflex_point_count > -1;
  do_orientation_filter = reflex_point_count > -1;

  directory_for_every_phase_out = out_every_phase ? createEveryPhaseDir(point_list_size, reflex_point_count) : "";

}
