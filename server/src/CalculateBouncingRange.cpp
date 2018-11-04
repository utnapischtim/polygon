#include <vector>
#include <array>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_circular_kernel_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/iterator.h>

#include "easylogging++.h"

#include "CalculateBouncingRange.h"
#include "BouncingVerticesSettings.h"
#include "random.h"
#include "cgal_helper.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

static double degreeToRadian(const double degree) {
  return degree * (M_PI / 180);
}

static bool compare(const cgal::Point_2 &a, const cgal::Point_2 &b, const double epsilon = 1.0e-10) {
  bool x_equal = std::abs(a.x() - b.x()) <= epsilon;
  bool y_equal = std::abs(a.y() - b.y()) <= epsilon;

  return x_equal && y_equal;
}

static cgal::Segment_2 intersection(std::vector<cgal::Point_2> poly, cgal::Line_2 line) {
  cgal::Point_2 source, target;
  bool is_source = false;
  std::size_t size = poly.size();
  poly.push_back(poly[0]);

  for (std::size_t position = 0; position < size; position += 1)
    if (auto inter = CGAL::intersection(cgal::Segment_2(poly[position], poly[position+1]), line); inter) {
      if (is_source && !compare(boost::get<cgal::Point_2>(*inter), source))
        target = boost::get<cgal::Point_2>(*inter);
      else {
        source = boost::get<cgal::Point_2>(*inter);
        is_source = true;
      }
    }
  return {source, target};
}

double pl::CalculateBouncingRange::getRandomAngleToBounce([[maybe_unused]] Segments::iterator &sit) {
  // ATTENTION
  // there is a bug. it is possible to get a angle and in series the line where
  // the previous or next segment lies

  return pl::randomValueOfRange(0.0, 2 * M_PI);
}

// rotate around point A
cgal::Line_2 pl::CalculateBouncingRange::rotate(const cgal::Point_2 &A, const cgal::Point_2 &B, const double angle_in_radian) {
  const auto ab = cgal::Vector_2(A, B);
  const auto rotate_func = CGAL::Aff_transformation_2<cgal>(CGAL::ROTATION, std::sin(angle_in_radian), std::cos(angle_in_radian));
  const auto ab_rotated = rotate_func(ab);
  return cgal::Line_2(A, ab_rotated);
}


// cgal::Line_2 rotate(const cgal::Segment_2 &s, const double angle) {
//   return rotate(s.source(), s.target(), angle);
// }

cgal::Line_2 pl::CalculateBouncingRange::calculateRandomLine(const double angle_in_radian) {
  const auto temporary_point_for_random_line = bouncing_point + cgal::Vector_2(1, 0);
  return rotate(bouncing_point, temporary_point_for_random_line, angle_in_radian);
}

cgal::Line_2 pl::CalculateBouncingRange::calculateBisectingLine(const cgal::Point_2 &A, const cgal::Point_2 &B) {
  const auto right_angle = M_PI / 2;
  const auto middle_point = CGAL::midpoint(A, B);
  return rotate(middle_point, B, right_angle);
}

// A has to be the point near to the random line
// the rotation is always done clockwise
cgal::Point_2 pl::CalculateBouncingRange::calculateIntersectionWithRandomLine(const cgal::Point_2 &A, const cgal::Point_2 &B, const double angle_in_radian) {
  const auto line = rotate(A, B, angle_in_radian);
  // TODO angle == 90 leads to no intersection;
  const auto intersection = CGAL::intersection(line, random_line);
  return boost::get<cgal::Point_2>(*intersection);
}

// this is the allowed moving segment constructed by the segment and the allowed
// range, and its projection to the random_line
cgal::Segment_2 pl::CalculateBouncingRange::calculateAllowedMovingSegment(const cgal::Segment_2 &seg, const double lower, const double upper) {
  cgal::Point_2 point_1 = calculateIntersectionWithRandomLine(seg.source(), seg.target(), lower);
  cgal::Point_2 point_2 = calculateIntersectionWithRandomLine(seg.source(), seg.target(), upper);

  cgal::Point_2 near_bouncing_point, far_bouncing_point;

  if (CGAL::squared_distance(bouncing_point, point_1) < CGAL::squared_distance(bouncing_point, point_2)) {
    near_bouncing_point = point_1;
    far_bouncing_point = point_2;
  }
  else {
    near_bouncing_point = point_2;
    far_bouncing_point = point_1;
  }

  return {near_bouncing_point, far_bouncing_point};
}

// source() is always near to bouncing point
// it should also work with check if bouncing point is inside circle with center
// on prev, next segments
cgal::Segment_2 pl::CalculateBouncingRange::blend(const cgal::Segment_2 &allowed_seg_prev, const cgal::Segment_2 &allowed_seg_next) {
  cgal::Point_2 near, far;

  VLOG(3) << "blend prev: " << allowed_seg_prev << " next: " << allowed_seg_next;

  if (allowed_seg_prev.source().y() < allowed_seg_next.source().y())
    near = allowed_seg_next.source();
  else
    near = allowed_seg_prev.source();

  if (allowed_seg_prev.target().y() < allowed_seg_next.target().y())
    far = allowed_seg_prev.target();
  else
    far = allowed_seg_next.target();

  return {near, far};
}

// A has to be the point near to the random line
//cgal::Segment_2 pl::CalculateBouncingRange::calculateAllowedConvexReflexPrevNextSegment(const cgal::Segment_2 &prev_segment, const cgal::Segment_2 &next_segment) {
cgal::Segment_2 pl::CalculateBouncingRange::calculateAllowedConvexReflexPrevNextSegment() {

  // state_prev == RIGHT_TURN => convex, LEFT_TURN => reflex
  // state_next == LEFT_TURN => convex, RIGHT_TURN => reflex
  const auto state_prev = CGAL::orientation(prev_segment.source(), prev_segment.target(), bouncing_point);
  const auto state_next = CGAL::orientation(next_segment.target(), next_segment.source(), bouncing_point);

  std::map<std::string, std::map<std::string, cgal::Segment_2>> allowed_segments;

  allowed_segments["prev"]["convex"] = calculateAllowedMovingSegment(prev_segment, bvs.convex_angle_range.lower_bound_radian(), bvs.convex_angle_range.upper_bound_radian());
  allowed_segments["prev"]["reflex"] = calculateAllowedMovingSegment(prev_segment, bvs.reflex_angle_range.lower_bound_radian(), bvs.reflex_angle_range.upper_bound_radian());

  allowed_segments["next"]["convex"] = calculateAllowedMovingSegment(next_segment.opposite(), 2*M_PI - bvs.convex_angle_range.lower_bound_radian(), 2*M_PI - bvs.convex_angle_range.upper_bound_radian());
  allowed_segments["next"]["reflex"] = calculateAllowedMovingSegment(next_segment.opposite(), 2*M_PI - bvs.reflex_angle_range.lower_bound_radian(), 2*M_PI - bvs.reflex_angle_range.upper_bound_radian());


  cgal::Segment_2 allowed_segment = blend(allowed_segments["prev"][state_prev == CGAL::RIGHT_TURN ? "convex" : "reflex"], allowed_segments["next"][state_next == CGAL::LEFT_TURN ? "convex" : "reflex"]);

  return allowed_segment;
}

cgal::Point_2 pl::CalculateBouncingRange::calculatePointWithAngle(const cgal::Point_2 &A, const cgal::Point_2 &B, const cgal::Line_2 &bisecting_line, const double angle) {
  const auto rotation_angle = 2*M_PI - (M_PI / 2 - degreeToRadian(angle));
  const auto line = rotate(A, B, rotation_angle);
  const auto center = boost::get<cgal::Point_2>(*CGAL::intersection(line, bisecting_line));

  using CircK = CGAL::Exact_circular_kernel_2;
  using Pt2 = CGAL::Point_2<CircK>;
  using Circle_2 = CGAL::Circle_2<CircK>;
  using Line_2 = CGAL::Line_2<CircK>;
  using IsectOutput = std::pair<CGAL::Circular_arc_point_2<CircK>, unsigned>;
  std::vector<IsectOutput> intersections;
  using Dispatcher = CGAL::Dispatch_output_iterator<CGAL::cpp11::tuple<IsectOutput>,
                                                    CGAL::cpp11::tuple<std::back_insert_iterator<std::vector<IsectOutput>>>>;
  Dispatcher disp = CGAL::dispatch_output<IsectOutput>(std::back_inserter(intersections));

  // TODO
  // there should be a better approach to convert
  // Exact_predicates_inexact_constructions_kernel to Exact_circular_kernel_2
  Pt2
    point_0(random_line.point(0).x(), random_line.point(0).y()),
    point_1(random_line.point(1).x(), random_line.point(1).y()),
    c(center.x(), center.y());
  Line_2 random_line_with_circular_k(point_0, point_1);
  Circle_2 circle(c, CGAL::squared_distance({A.x(), A.y()}, c));

  CGAL::intersection(circle, random_line_with_circular_k, disp);
  const auto orientation_of_bouncing_point = CGAL::orientation(A, B, bouncing_point);

  cgal::Point_2 point;

  for (const auto intersection : intersections) {
    cgal::Point_2 p(CGAL::to_double(intersection.first.x()), CGAL::to_double(intersection.first.y()));
    if (orientation_of_bouncing_point == CGAL::orientation(A, B, p))
      point = p;
  }

  return point;
}

cgal::Segment_2 pl::CalculateBouncingRange::calculateAllowedMovingSegment(const cgal::Point_2 &A, const cgal::Point_2 &B, const pl::Filter &angle_range) {
  const auto bisecting_line = calculateBisectingLine(A, B);
  const auto mid_point = CGAL::midpoint(A, B);

  const auto point_lower = calculatePointWithAngle(A, mid_point, bisecting_line, angle_range.lower_bound);
  const auto point_upper = calculatePointWithAngle(A, mid_point, bisecting_line, angle_range.upper_bound);

  cgal::Point_2 point_near_mid_point, point_far_mid_point;

  if (CGAL::squared_distance(point_lower, mid_point) < CGAL::squared_distance(point_upper, mid_point)) {
    point_near_mid_point = point_lower;
    point_far_mid_point = point_upper;
  } else {
    point_near_mid_point = point_upper;
    point_far_mid_point = point_lower;
  }

  return {point_near_mid_point, point_far_mid_point};
}

cgal::Segment_2 pl::CalculateBouncingRange::calculateOrientationStability() {
  VLOG(3) << "calculateOrientationStability prev_segment: " << prev_segment << " next_segment: " << next_segment;
  cgal::Line_2 prev_line(prev_segment), next_line(next_segment);
  // VLOG(3) << "calculateOrientationStability prev_line: " << prev_line << " next_line: " << next_line;
  const auto orientation_bouncing_point = CGAL::orientation(before_point, after_point, bouncing_point);

  // VLOG(3) << "calculateOrientationStability orientation_bouncing_point: " << orientation_bouncing_point;

  if (auto inter = CGAL::intersection(prev_line, next_line);
      inter && orientation_bouncing_point == CGAL::orientation(before_point, after_point, boost::get<cgal::Point_2>(*inter))) {
    // VLOG(3) << "calculateOrientationStability inter: " << boost::get<cgal::Point_2>(*inter);
    return intersection({before_point, after_point, boost::get<cgal::Point_2>(*inter)}, random_line);
  } else {
    // VLOG(3) << "calculateOrientationStability else";
    const auto inter_prev = bvs.sampling_grid.intersection(cgal::Line_2(prev_segment));
    const auto orientation_inter_prev = CGAL::orientation(before_point, after_point, std::get<0>(inter_prev));

    const auto inter_next = bvs.sampling_grid.intersection(cgal::Line_2(next_segment));
    const auto orientation_inter_next = CGAL::orientation(before_point, after_point, std::get<0>(inter_next));

    std::vector<cgal::Point_2> polygon = {before_point, after_point};

    cgal::Point_2
      point_from_next = orientation_bouncing_point == orientation_inter_next ? std::get<0>(inter_next) : std::get<1>(inter_next),
      point_from_prev = orientation_bouncing_point == orientation_inter_prev ? std::get<0>(inter_prev) : std::get<1>(inter_prev);

    polygon.push_back(point_from_next);

    // it is possible that the previous and next segments doesn't intersect the
    // same border, therefore the corner between them is necessary. it is
    // possible to have more then one, but it should always be a right turn
    cgal::Line_2 line(point_from_prev, point_from_next);
    if (!line.is_vertical() && !line.is_horizontal())
      for (auto point : bvs.sampling_grid.points())
        if (CGAL::right_turn(point_from_prev, point_from_next, point))
          polygon.push_back(point);

    polygon.push_back(point_from_prev);

    return intersection(polygon, random_line);
  }
}

cgal::Segment_2 pl::CalculateBouncingRange::calculateIntersectionFreeRange() {
  cgal::Point_2 upper_point, lower_point;

  auto left_or_below = [&](auto &point) {
    bool is_left = random_line.is_horizontal() && point.x() <= bouncing_point.x();
    bool is_below = !random_line.is_horizontal() && point.y() <= bouncing_point.y();
    return is_left || is_below;
  };

  auto update_interval = [&](auto &point) {
    if (left_or_below(point) && CGAL::squared_distance(bouncing_point, point) < CGAL::squared_distance(bouncing_point, lower_point))
      lower_point = point;
    else if (!left_or_below(point) && CGAL::squared_distance(bouncing_point, point) < CGAL::squared_distance(bouncing_point, upper_point))
      upper_point = point;
  };

  auto intersection_after_point_on_ray = [&](auto &a, auto &b, auto &c) {
    if (a.x() < b.x()) {
      if (a.y() < b.y())
        return b.x() < c.x() && b.y() < c.y();
      else
        return b.x() < c.x() && c.y() < b.y();
    } else {
      if (a.y() < b.y())
        return c.x() < b.x() && b.y() < c.y();
      else
        return c.x() < b.x() && c.y() < b.y();
    }
  };

  auto [point_1, point_2] = bvs.sampling_grid.intersection(random_line);
  if (left_or_below(point_1)) {
    lower_point = point_1;
    upper_point = point_2;
  } else {
    lower_point = point_2;
    upper_point = point_1;
  }

  for (const auto segment : segments) {
    if (const auto inter = CGAL::intersection(segment, random_line); inter) {
      const auto inter_point = boost::get<cgal::Point_2>(*inter);
      if (!compare(inter_point, bouncing_point))
        update_interval(inter_point);
    }

    for (const auto point_around : std::array{before_point, after_point}) {
      if (const auto inter = CGAL::intersection(cgal::Ray_2(point_around, segment.target()), random_line); inter) {
        try {
          const auto inter_point = boost::get<cgal::Point_2>(*inter);

          if (!compare(bouncing_point, inter_point) && intersection_after_point_on_ray(point_around, segment.target(), inter_point))
            update_interval(inter_point);
        } catch (boost::bad_get &e) {
          // nothing happend, because this is only the case if the ray is on the
          // random _line, and in this case no update should occur!
        }
      }
    }
  }

  return {lower_point, upper_point};
}

cgal::Segment_2 pl::CalculateBouncingRange::calculateSmallestBouncingInterval(const Segments &allowed_segments) {
  cgal::Segment_2 allowed_segment = allowed_segments[0];

  for (auto segment : allowed_segments) {
    if (CGAL::squared_distance(segment.source(), bouncing_point) < CGAL::squared_distance(allowed_segment.source(), bouncing_point))
      allowed_segment = {segment.source(), allowed_segment.target()};

    if (CGAL::squared_distance(segment.target(), bouncing_point) < CGAL::squared_distance(allowed_segment.target(), bouncing_point))
      allowed_segment = {allowed_segment.source(), segment.target()};
  }

  return allowed_segment;
}

cgal::Segment_2 pl::CalculateBouncingRange::calculateBouncingInterval(SegmentsIterator &sit) {
  const auto random_angle = getRandomAngleToBounce(sit);
  bouncing_point = sit->target();
  random_line = calculateRandomLine(random_angle);
  before_point = sit->source();
  after_point = pl::next(segments, sit)->target();
  prev_segment = *pl::prev(segments, sit);
  next_segment = *pl::next(segments, pl::next(segments, sit));

  std::vector<cgal::Segment_2> allowed_segments;

  VLOG(3) << "calculateBouncingInterval random_angle: " << random_angle << " bouncing_point: " << bouncing_point << " random_line: " << random_line;

  // calculate intersection with other segments
  const auto allowed_intersection_free_segment = calculateIntersectionFreeRange();
  allowed_segments.push_back(allowed_intersection_free_segment);

  // if (bvs.keep_angle) {
  //   // allowed segments from the angle of the point A, and B, the points before
  //   // and after the bouncing point
  //   const auto allowed_convex_reflex_prev_next_segment = calculateAllowedConvexReflexPrevNextSegment(prev_segment, next_segment);
  //   allowed_segments.push_back(allowed_convex_reflex_prev_next_segment);

  //   // allowed reflex and convex segment in bouncing point
  //   const auto allowed_reflex_bouncing_point_segment = calculateAllowedMovingSegment(A, B, bvs.reflex_angle_range);
  //   allowed_segments.push_back(allowed_reflex_bouncing_point_segment);

  //   const auto allowed_convex_bouncing_point_segment = calculateAllowedMovingSegment(A, B, bvs.convex_angle_range);
  //   allowed_segments.push_back(allowed_convex_bouncing_point_segment);
  // }

  // calculate smallest/widest by orientation stability
  // if (bvs.keep_orientation) {
  //   const auto allowed_orientation_stability_segment = calculateOrientationStability();
  //   allowed_segments.push_back(allowed_orientation_stability_segment);
  // }

  return calculateSmallestBouncingInterval(allowed_segments);
}

cgal::Point_2 pl::CalculateBouncingRange::calculateShiftedPoint(const cgal::Segment_2 &allowed_segment) {
  double squared_length = allowed_segment.squared_length();

  // ATTENTION if it is necessary d could be found with more effort!
  const double d = pl::randomValueOfRange(0.0, squared_length);
  const auto k = d / squared_length;

  const auto A = allowed_segment.source(), B = allowed_segment.target();

  // cgal::Point_2 does not have a operator*(double, Point_2);
  cgal::Point_2 shifted_point = {k * A.x() + (1-k) * B.x(), k * A.y() + (1-k) * B.y()};
  return shifted_point;
}

void pl::CalculateBouncingRange::bounce(SegmentsIterator &sit) {
  VLOG(3) << "\n\n";
  const auto allowed_segment = calculateBouncingInterval(sit);
  VLOG(3) << "bounce allowed_segment: " << allowed_segment;
  const auto shifted_point = calculateShiftedPoint(allowed_segment);

  Segments::iterator sitn = pl::next(segments, sit);

  VLOG(3) << "bounce shifted_poind: " << shifted_point;

  *sit = {sit->source(), shifted_point};
  *sitn = {shifted_point, sitn->target()};
}
