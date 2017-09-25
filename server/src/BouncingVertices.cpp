#include <tuple>
#include <limits>
#include <cstdlib>
#include <iterator>
#include <vector>
#include <string>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "BouncingVertices.h"
#include "Point.h"
#include "CommonSetting.h"
#include "Filter.h"
#include "random.h"
#include "Output.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

using Segments = std::vector<cgal::Segment_2>;

static Segments::iterator next(Segments &segments, const Segments::iterator &it);
static Segments::iterator prev(Segments &segments, const Segments::iterator &it);

static std::tuple<pl::SamplingGrid, unsigned, double> init(const pl::CommonSettingList &common_settings);
static Segments init(const pl::PointList &point_list);

static cgal::Point_2 createPointInsideArea(const pl::SamplingGrid &sampling_grid, const double radius, const Segments::iterator &sit);

static bool insideOrientationArea(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, const cgal::Segment_2 &e_1_old, const cgal::Segment_2 &e_2_old, const cgal::Segment_2 &next, const cgal::Segment_2 &prev);
static bool isIntersection(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, Segments &segments, const Segments::iterator &sit);

pl::PointList pl::bouncingVertices(const pl::PointList &point_list, const pl::CommonSettingList &common_settings, const pl::FilterList &/*filters*/) {
  pl::PointList final_list;

  Segments segments = init(point_list);
  auto [sampling_grid, phases, radius] = init(common_settings);

  for (size_t phase = 0; phase < phases; ++phase) {
    // not use next or prev because this would cause a endles loop
    for (auto sit = segments.begin(); sit != segments.end(); ++sit) {
      bool
        intersection_occur = false,
        outside_orientation_area = false;

      cgal::Point_2 shifted_point;

      Segments::iterator
        sitp = prev(segments, sit),
        sitn = next(segments, sit),
        sitnn = next(segments, sitn);

      cgal::Segment_2 e_1, e_2, e_1_old = *sit, e_2_old = *sitn;

      do {
        // reset to false, because point search starts new
        intersection_occur = false;

        shifted_point = createPointInsideArea(sampling_grid, radius, sit);

        e_1 = {sit->source(), shifted_point};
        e_2 = {shifted_point, sitn->target()};

        outside_orientation_area = !insideOrientationArea(e_1, e_2, e_1_old, e_2_old, *sitnn, *sitp);

        if (!outside_orientation_area)
          intersection_occur = isIntersection(e_1, e_2, segments, sit);

      } while (outside_orientation_area || intersection_occur);

      // set the new segments;
      *sit = e_1;
      *sitn = e_2;
    }
  }

  pl::convert(segments, final_list);
  return final_list;
}

Segments::iterator next(Segments &segments, const Segments::iterator &it) {
  auto it_n = it + 1;

  if (it_n == segments.end())
    it_n = segments.begin();

  return it_n;
}

Segments::iterator prev(Segments &segments, const Segments::iterator &it) {
  Segments::iterator it_p;

  if (it == segments.begin())
    it_p = segments.end() - 1;
  else
    it_p = it - 1;

  return it_p;
}

std::tuple<pl::SamplingGrid, unsigned, double> init(const pl::CommonSettingList &common_settings) {
    pl::CommonSetting c_s_sampling_grid, c_s_phases, c_s_radius;

  if (auto t = pl::find(common_settings, "sampling grid"))
    c_s_sampling_grid = *t;
  else {
    std::string msg = std::string("essential common setting 'sampling grid' not set to generate random pointList");
    throw std::runtime_error(msg);
  }

  if (auto t = pl::find(common_settings, "phases"))
    c_s_phases = *t;
  else {
    std::string msg = std::string("essential common setting 'nodes' not set to generate random pointList");
    throw std::runtime_error(msg);
  }

  if (auto t = pl::find(common_settings, "radius"))
    c_s_radius = *t;
  else
    c_s_radius = pl::CommonSetting("radius", "", 3, "number", "60");

  // TODO:
  // make it robust!
  pl::SamplingGrid sampling_grid(c_s_sampling_grid);
  unsigned phases = std::stoi(c_s_phases.val);
  double radius = std::stod(c_s_radius.val);

  return {sampling_grid, phases, radius};
}

Segments init(const pl::PointList &point_list) {
  Segments segments;

  for (size_t i = 1, size = point_list.size(); i < size; ++i) {
    cgal::Point_2
      source(point_list[i - 1].x, point_list[i - 1].y),
      target(point_list[i].x, point_list[i].y);
    segments.push_back({source, target});
  }

  // close the polygon
  segments.push_back({segments[segments.size() - 1].target(), segments[0].source()});

  return segments;
}

cgal::Point_2 createPointInsideArea(const pl::SamplingGrid &sampling_grid, const double radius, const Segments::iterator &sit) {
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

bool insideOrientationArea(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, const cgal::Segment_2 &e_1_old, const cgal::Segment_2 &e_2_old, const cgal::Segment_2 &next, const cgal::Segment_2 &prev) {
  return
    CGAL::orientation(prev.source(), e_1.source(), e_1.target()) == CGAL::orientation(prev.source(), e_1_old.source(), e_1_old.target()) &&
    CGAL::orientation(e_1.source(), e_1.target(), e_2.target()) == CGAL::orientation(e_1_old.source(), e_1_old.target(), e_2_old.target()) &&
    CGAL::orientation(e_2.source(), e_2.target(), next.target()) == CGAL::orientation(e_2_old.source(), e_2_old.target(), next.target());
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
bool isIntersection(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, Segments &segments, const Segments::iterator &sit) {
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

