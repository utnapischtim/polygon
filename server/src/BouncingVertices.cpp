#include <tuple>
#include <limits>
#include <cstdlib>
#include <iterator>
#include <vector>
#include <string>
#include <cmath>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <Magick++.h>

#include "BouncingVertices.h"
#include "Point.h"
#include "CommonSetting.h"
#include "Filter.h"
#include "random.h"
#include "Output.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

using Segments = std::vector<cgal::Segment_2>;

const size_t MAX_CYCLES = 1000;

static Segments::iterator next(Segments &segments, const Segments::iterator &it);
static Segments::iterator prev(Segments &segments, const Segments::iterator &it);

static std::tuple<pl::SamplingGrid, unsigned, double, bool> init(const pl::CommonSettingList &common_settings);
static Segments init(const pl::PointList &point_list);

static cgal::Point_2 createPointInsideArea(const pl::SamplingGrid &sampling_grid, const double radius, const Segments::iterator &sit);

static bool insideOrientationArea(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, const cgal::Segment_2 &e_1_old, const cgal::Segment_2 &e_2_old, const cgal::Segment_2 &next, const cgal::Segment_2 &prev);
static bool insideAngleRange(const cgal::Segment_2 &e_1_new, const cgal::Segment_2 &e_2_new, const cgal::Segment_2 &e_1_old, const cgal::Segment_2 &e_2_old, const cgal::Segment_2 &next, const cgal::Segment_2 &prev, const pl::FilterList &filters);
static bool isIntersection(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, Segments &segments, const Segments::iterator &sit);

static double calculateAngle(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2);

pl::PointList pl::bouncingVertices(const pl::PointList &point_list, const pl::CommonSettingList &common_settings, const pl::FilterList &filters) {
  pl::PointList final_list;

  Segments segments = init(point_list);
  auto [sampling_grid, phases, bouncing_radius, animation] = init(common_settings);

  // mainly the orientation filter is done, because of the reflex
  // points, because they are more interessting, then convex points.
  // it could be possible that in future there has to be a distinction
  // between convex and reflex orientation, but for that, the
  // insideOrientationArea has to be rewritten too!
  bool do_orientation_filter = pl::find(filters, "reflex points").value().val > -1;

  for (size_t phase = 0; phase < phases; ++phase) {
    // not use next or prev because this would cause a endles loop
    for (auto sit = segments.begin(); sit != segments.end(); ++sit) {
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

        shifted_point = createPointInsideArea(sampling_grid, bouncing_radius, sit);

        e_1 = {sit->source(), shifted_point};
        e_2 = {shifted_point, sitn->target()};

        if (do_orientation_filter)
          outside_orientation_area = !insideOrientationArea(e_1, e_2, e_1_old, e_2_old, *sitnn, *sitp);

        // the angle should only be checked, if there are reflex nodes
        // to protect and if the point is not outside of the
        // orientation area!
        if (!outside_orientation_area)
          out_of_angle_range = !insideAngleRange(e_1, e_2, e_1_old, e_2_old, *sitnn, *sitp, filters);

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

    if (animation) {
      std::string filename = "out/animation/" + std::to_string(phase) + ".png";
      pl::PointList list;
      pl::convert(segments, list);
      pl::output(list, "png", filename, sampling_grid, phase);
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

std::tuple<pl::SamplingGrid, unsigned, double, bool> init(const pl::CommonSettingList &common_settings) {
  pl::CommonSetting c_s_sampling_grid, c_s_phases, c_s_radius, c_s_animation;

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

  if (auto t = pl::find(common_settings, "bouncing radius"))
    c_s_radius = *t;
  else
    c_s_radius = pl::CommonSetting("bouncing radius", "", 3, "number", "60");

  if (auto t = pl::find(common_settings, "animation"))
    c_s_animation = *t;
  else
    c_s_animation = pl::CommonSetting("animation", "", 6, "number", "0");

  // TODO:
  // make it robust!
  pl::SamplingGrid sampling_grid(c_s_sampling_grid);
  unsigned phases = std::stoi(c_s_phases.val);
  double radius = std::stod(c_s_radius.val);
  bool animation = std::stoi(c_s_animation.val) == 1;

  if (auto t = pl::find(common_settings, "segment length"); t && radius < 1)
    radius = std::stod(t->val) / 2;

  return {sampling_grid, phases, radius, animation};
}

Segments init(const pl::PointList &point_list) {
  Segments segments;

  for (size_t i = 1, size = point_list.size(); i < size; ++i)
    segments.push_back({point_list[i-1], point_list[i]});

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

bool insideAngleRange(const cgal::Segment_2 &e_1_new, const cgal::Segment_2 &e_2_new, const cgal::Segment_2 &e_1_old, const cgal::Segment_2 &e_2_old, const cgal::Segment_2 &next, const cgal::Segment_2 &prev, const pl::FilterList &filters) {
  pl::Filter reflex_angle_range, convex_angle_range;

  if (auto t = pl::find(filters, "reflex angle range"))
    reflex_angle_range = *t;

  if (auto t = pl::find(filters, "convex angle range"))
    convex_angle_range = *t;

  bool keep_orientation = pl::find(filters, "reflex points").value().val > -1;

  // it could only be left or right turn, because
  // insideOrientationArea allows only that!
  auto isInBoundaries = [&](CGAL::Orientation orientation, double angle) {
    double lower_bound, upper_bound;

    if (orientation == CGAL::LEFT_TURN) {
      lower_bound = reflex_angle_range.lower_bound;
      upper_bound = reflex_angle_range.upper_bound;
    } else {
      lower_bound = convex_angle_range.lower_bound;
      upper_bound = convex_angle_range.upper_bound;
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
  if (keep_orientation) {
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

double calculateAngle(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2) {
  cgal::Vector_2
    u = e_1.source() - e_1.target(),
    v = e_2.target() - e_2.source();

  // angle = arccos((u * v) / (||u|| * ||v||))
  double radian_angle = std::acos((u*v) / (std::sqrt(u.squared_length()) * std::sqrt(v.squared_length())));

  return (radian_angle * 180) / M_PI;
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

