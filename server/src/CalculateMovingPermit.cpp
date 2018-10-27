#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "BouncingVerticesSettings.h"
#include "CalculateMovingPermit.h"
#include "cgal_helper.h"
#include "random.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

using Segments = std::vector<cgal::Segment_2>;
using SegmentsIterator = Segments::iterator;

const size_t MAX_CYCLES = 1000;

cgal::Point_2 pl::CalculateMovingPermit::createPointInsideArea(const Segments::iterator &sit) {
  cgal::Point_2 shifted_point;
  const double radius = bvs.bouncing_radius;

  do {
    cgal::Point_2 velocity = {pl::randomValueOfRange(-radius, radius), pl::randomValueOfRange(-radius, radius)};

    // check if the shifted_point of the current segment is inside
    // of the boundary box, the shifted_point is also the source
    // of the next segment, therefore the next segment does not
    // have to be checked.
    shifted_point = {sit->target().x() + velocity.x(), sit->target().y() + velocity.y()};
  } while (bvs.sampling_grid.isOutOfArea(shifted_point));

  return shifted_point;
}

bool pl::CalculateMovingPermit::insideOrientationArea(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, const cgal::Segment_2 &e_1_old, const cgal::Segment_2 &e_2_old, const cgal::Segment_2 &next, const cgal::Segment_2 &prev) {
  return
    CGAL::orientation(prev.source(), e_1.source(), e_1.target()) == CGAL::orientation(prev.source(), e_1_old.source(), e_1_old.target()) &&
    CGAL::orientation(e_1.source(), e_1.target(), e_2.target()) == CGAL::orientation(e_1_old.source(), e_1_old.target(), e_2_old.target()) &&
    CGAL::orientation(e_2.source(), e_2.target(), next.target()) == CGAL::orientation(e_2_old.source(), e_2_old.target(), next.target());
}

double pl::CalculateMovingPermit::calculateAngle(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2) {
  cgal::Vector_2
    u = e_1.source() - e_1.target(),
    v = e_2.target() - e_2.source();

  // angle = arccos((u * v) / (||u|| * ||v||))
  double radian_angle = std::acos((u*v) / (std::sqrt(u.squared_length()) * std::sqrt(v.squared_length())));

  return (radian_angle * 180) / M_PI;
}

bool pl::CalculateMovingPermit::insideAngleRange(const cgal::Segment_2 &e_1_new, const cgal::Segment_2 &e_2_new, const cgal::Segment_2 &e_1_old, const cgal::Segment_2 &e_2_old, const cgal::Segment_2 &next, const cgal::Segment_2 &prev) {
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
bool pl::CalculateMovingPermit::isIntersection(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, const Segments::iterator &sit) {
  bool intersection_occur = false;

  cgal::Segment_2 old_e_1 = *sit, old_e_2 = *pl::next(segments, sit);

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

void pl::CalculateMovingPermit::bounce(SegmentsIterator &sit) {
  bool
    intersection_occur = false,
    outside_orientation_area = false,
    out_of_angle_range = false;

  cgal::Point_2 shifted_point;

  Segments::iterator
    sitp = pl::prev(segments, sit),
    sitn = pl::next(segments, sit),
    sitnn = pl::next(segments, sitn);

  cgal::Segment_2 e_1, e_2, e_1_old = *sit, e_2_old = *sitn;

  size_t count_cycles = 0;

  do {
    // reset to false, because point search starts new
    intersection_occur = false;
    outside_orientation_area = false;
    out_of_angle_range = false;

    shifted_point = createPointInsideArea(sit);

    e_1 = {sit->source(), shifted_point};
    e_2 = {shifted_point, sitn->target()};

    if (bvs.keep_orientation)
      outside_orientation_area = !insideOrientationArea(e_1, e_2, e_1_old, e_2_old, *sitnn, *sitp);

    if (bvs.keep_angle)
      out_of_angle_range = !insideAngleRange(e_1, e_2, e_1_old, e_2_old, *sitnn, *sitp);

    if (!outside_orientation_area && !out_of_angle_range)
      intersection_occur = isIntersection(e_1, e_2, sit);

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
