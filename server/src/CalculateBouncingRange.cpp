#include <vector>
#include <array>
#include <iterator>
#include <cmath>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_circular_kernel_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/iterator.h>

#include "easylogging++.h"

#include "CalculateBouncingRange.h"
#include "BouncingVerticesSettings.h"
#include "Filter.h"
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
cgal::Point_2 pl::CalculateBouncingRange::calculateIntersectionWithRandomLine(const cgal::Segment_2 &seg, const double angle_in_radian) {
  const auto line = rotate(seg.source(), seg.target(), angle_in_radian);
  // TODO angle == 90 leads to no intersection;
  const auto intersection = CGAL::intersection(line, random_line);
  return boost::get<cgal::Point_2>(*intersection);
}

// this is the allowed moving segment constructed by the segment and the allowed
// range, and its projection to the random_line
cgal::Segment_2 pl::CalculateBouncingRange::calculateAllowedMovingSegmentByAngleRange(const cgal::Segment_2 &seg, const pl::Filter range) {
  cgal::Point_2
    lower_point = calculateIntersectionWithRandomLine(seg, range.lower_bound_radian()),
    upper_point = calculateIntersectionWithRandomLine(seg, range.upper_bound_radian());

  return {lower_point, upper_point};
}

cgal::Segment_2 pl::CalculateBouncingRange::calculatePreservedAngleRangeInBouncingPoint() {
  VLOG(3) << "calculatePreservedAngleRangeInBouncingPoint";
  const auto bisecting_line = calculateBisectingLine(before_point, after_point);
  const auto mid_point = CGAL::midpoint(before_point, after_point);
  const auto orientation = CGAL::orientation(before_point, after_point, bouncing_point);

  const std::string state = orientation == CGAL::RIGHT_TURN ? "convex" : "reflex";
  const auto angle_range = state == "convex" ? bvs.convex_angle_range : bvs.reflex_angle_range;

  const auto point_lower = calculatePointWithAngle(before_point, mid_point, bisecting_line, angle_range.lower_bound);
  const auto point_upper = calculatePointWithAngle(before_point, mid_point, bisecting_line, angle_range.upper_bound);

  return {point_lower, point_upper};
}

cgal::Segment_2 pl::CalculateBouncingRange::blend(const cgal::Segment_2 &allowed_seg_prev, const cgal::Segment_2 &allowed_seg_next) {
  cgal::Segment_2 allowed_segment;

  if (auto overlap_optional = CGAL::intersection(allowed_seg_prev, allowed_seg_next); overlap_optional) {
    if (const cgal::Segment_2* overlap = boost::get<cgal::Segment_2>(&*overlap_optional))
      allowed_segment = *overlap;
    else {
      std::cout << "blend does not could create a allowed segment with: " << allowed_seg_prev << " : " << allowed_seg_next << "\n";
      std::exit(-1);
    }
  }

  return allowed_segment;
}

// this function calculates the allowed segment to preserve the angle on the
// points before and after the bouncing point. to calculate the allowed segment
// the segments before and after are rotated and projected to the randomline
cgal::Segment_2 pl::CalculateBouncingRange::calculatePreservedAngleRangeAroundBouncingPoint() {
  VLOG(3) << "calculatePreservedAngleRangeAroundBouncingPoint";

  // orientation_prev == RIGHT_TURN => convex, LEFT_TURN => reflex
  // orientation_next == LEFT_TURN => convex, RIGHT_TURN => reflex
  const auto
    orientation_prev = CGAL::orientation(prev_segment.source(), prev_segment.target(), bouncing_point),
    orientation_next = CGAL::orientation(next_segment.target(), next_segment.source(), bouncing_point);

  const std::string
    state_prev = orientation_prev == CGAL::RIGHT_TURN ? "convex" : "reflex",
    state_next = orientation_next == CGAL::LEFT_TURN ? "convex" : "reflex";

  const pl::Filter
    range_prev = state_prev == "convex" ? bvs.convex_angle_range : bvs.reflex_angle_range,
    range_next = 2*M_PI - (state_next == "convex" ? bvs.convex_angle_range : bvs.reflex_angle_range);

  const cgal::Segment_2
    allowed_segment_prev = calculateAllowedMovingSegmentByAngleRange(prev_segment, range_prev),
    allowed_segment_next = calculateAllowedMovingSegmentByAngleRange(next_segment, range_next);

  return blend(allowed_segment_prev, allowed_segment_next);
}

cgal::Point_2 pl::CalculateBouncingRange::calculatePointWithAngle(const cgal::Point_2 &A, const cgal::Point_2 &B, const cgal::Line_2 &bisecting_line, const double angle) {
  VLOG(3) << "calculatePointWithAngle A: " << A << " B: " << B << " bisecting_line: " << bisecting_line << " angle: " << angle;

  auto rotationAngleCalculatedInDependenceToApertureAngle = [](const double a){
    return 2*M_PI - (M_PI / 2 - degreeToRadian(a));
  };

  const auto rotation_angle = rotationAngleCalculatedInDependenceToApertureAngle(angle);
  VLOG(3) << "  rotation_angle: " << rotation_angle;
  const auto line = rotate(A, B, rotation_angle);
  VLOG(3) << "  line: " << line;
  const auto center = boost::get<cgal::Point_2>(*CGAL::intersection(line, bisecting_line));
  VLOG(3) << "  center: " << center;
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

cgal::Segment_2 pl::CalculateBouncingRange::calculateOrientationStability() {
  cgal::Point_2 left_point, right_point;
  auto [point_1, point_2] = bvs.sampling_grid.intersection(random_line);

  if (CGAL::left_turn(before_point, bouncing_point, point_1)) {
    left_point = point_1;
    right_point = point_2;
  } else {
    left_point = point_2;
    right_point = point_1;
  }

  std::array lines = {cgal::Line_2(prev_segment), cgal::Line_2(next_segment), cgal::Line_2(before_point, after_point)};
  for (auto line : lines) {
    if (const auto inter = CGAL::intersection(line, random_line); inter) {
      // TODO: try catch
      if (CGAL::orientation(before_point, bouncing_point, left_point) == CGAL::orientation(before_point, bouncing_point, boost::get<cgal::Point_2>(*inter))) {
        if (CGAL::squared_distance(bouncing_point, boost::get<cgal::Point_2>(*inter)) < CGAL::squared_distance(bouncing_point, left_point))
          left_point = boost::get<cgal::Point_2>(*inter);
      } else if (CGAL::orientation(before_point, bouncing_point, right_point) == CGAL::orientation(before_point, bouncing_point, boost::get<cgal::Point_2>(*inter))) {
        if (CGAL::squared_distance(bouncing_point, boost::get<cgal::Point_2>(*inter)) < CGAL::squared_distance(bouncing_point, right_point))
          right_point = boost::get<cgal::Point_2>(*inter);
      } else
        VLOG(3) << "calculateOrientationStability missed some case";
    }
  }

  return {left_point, right_point};
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

  for (auto it = std::next(allowed_segments.begin(), 1); it != allowed_segments.end(); it++) {
    const auto
      orientation_allowed_source = CGAL::orientation(before_point, bouncing_point, allowed_segment.source()),
      orientation_allowed_target = CGAL::orientation(before_point, bouncing_point, allowed_segment.target()),
      orientation_segment_source = CGAL::orientation(before_point, bouncing_point, it->source()),
      orientation_segment_target = CGAL::orientation(before_point, bouncing_point, it->target());

    if (orientation_segment_source == orientation_allowed_source && CGAL::squared_distance(it->source(), bouncing_point) < CGAL::squared_distance(allowed_segment.source(), bouncing_point))
      allowed_segment = {it->source(), allowed_segment.target()};

    if (orientation_segment_source == orientation_allowed_target && CGAL::squared_distance(it->source(), bouncing_point) < CGAL::squared_distance(allowed_segment.target(), bouncing_point))
      allowed_segment = {allowed_segment.source(), it->source()};

    if (orientation_segment_target == orientation_allowed_source && CGAL::squared_distance(it->target(), bouncing_point) < CGAL::squared_distance(allowed_segment.source(), bouncing_point))
      allowed_segment = {it->target(), allowed_segment.target()};

    if (orientation_segment_target == orientation_allowed_target && CGAL::squared_distance(it->target(), bouncing_point) < CGAL::squared_distance(allowed_segment.target(), bouncing_point))
      allowed_segment = {allowed_segment.source(), it->target()};
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

  allowed_segments.push_back(calculateIntersectionFreeRange());

  if (bvs.keep_angle) {
    allowed_segments.push_back(calculatePreservedAngleRangeAroundBouncingPoint());
    allowed_segments.push_back(calculatePreservedAngleRangeInBouncingPoint());
  }

  //calculate smallest/widest by orientation stability
  if (bvs.keep_orientation)
    allowed_segments.push_back(calculateOrientationStability());

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
  VLOG(3) << "CalculateBouncingRange::bounce \n\n";
  const auto allowed_segment = calculateBouncingInterval(sit);
  VLOG(3) << "bounce allowed_segment: " << allowed_segment;
  const auto shifted_point = calculateShiftedPoint(allowed_segment);

  Segments::iterator sitn = pl::next(segments, sit);

  VLOG(3) << "bounce shifted_poind: " << shifted_point;

  *sit = {sit->source(), shifted_point};
  *sitn = {shifted_point, sitn->target()};
}
