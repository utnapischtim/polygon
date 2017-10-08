#include <iostream>
#include <cmath>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "catch.hpp"

#include "Filter.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

double calculateAngle(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2) {
  cgal::Vector_2
    u = e_1.source() - e_1.target(),
    v = e_2.target() - e_2.source();

  // angle = arccos((u * v) / (||u|| * ||v||))
  double radian_angle = std::acos((u*v) / (std::sqrt(u.squared_length()) * std::sqrt(v.squared_length())));

  return (radian_angle * 180) / M_PI;
}

bool insideAngleRange(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, const cgal::Segment_2 &e_1_old, const cgal::Segment_2 &e_2_old, const cgal::Segment_2 &next, const cgal::Segment_2 &prev, const pl::FilterList &filters) {
  bool is_inside = true;

  pl::Filter reflex_angle_range, convex_angle_range;

  if (auto t = pl::find(filters, "reflex angle range"))
    reflex_angle_range = *t;

  if (auto t = pl::find(filters, "convex angle range"))
    convex_angle_range = *t;

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

    // std::cout << "lower: " << lower_bound << " upper: " << upper_bound << " " << " angle: " << angle << " ";

    return lower_bound < angle && angle < upper_bound;
  };

  // the old orientation gives the boundary
  CGAL::Orientation old_orientation;
  double angle;

  // std::cout << prev.source() << " " << prev.target() << " " << e_1.target() << " ";
  old_orientation = CGAL::orientation(prev.source(), prev.target(), e_1_old.target());
  angle = calculateAngle(prev, e_1);

  // the new orientation gives is necessary to calculate the correct
  // new angle
  if (CGAL::left_turn(prev.source(), prev.target(), e_1.target()))
    angle = 360 - angle;

  is_inside = isInBoundaries(old_orientation, angle);

  // std::cout << "orientation: " << (old_orientation == CGAL::RIGHT_TURN ? "right_turn" : "left_turn") << " angle: " << angle << " is_inside: " << is_inside << "\n";

  if (is_inside) {
    // std::cout << e_1.source() << " " << e_1.target() << " " << e_2.target() << " ";
    old_orientation = CGAL::orientation(e_1_old.source(), e_1_old.target(), e_2_old.target());
    angle = calculateAngle(e_1, e_2);

    if (CGAL::left_turn(e_1.source(), e_1.target(), e_2.target()))
      angle = 360 - angle;

    is_inside = isInBoundaries(old_orientation, angle);
    // std::cout << "orientation: " << (old_orientation == CGAL::RIGHT_TURN ? "right_turn" : "left_turn") << " angle: " << angle << " is_inside: " << is_inside << "\n";
  }

  if (is_inside) {
    // std::cout << e_2.source() << " " << e_2.target() << " " << next.target() << " ";
    old_orientation = CGAL::orientation(e_2_old.source(), e_2_old.target(), next.target());
    angle = calculateAngle(e_2, next);

    if (CGAL::left_turn(e_2.source(), e_2.target(), next.target()))
      angle = 360 - angle;

    is_inside = isInBoundaries(old_orientation, angle);
    // std::cout << "orientation: " << (old_orientation == CGAL::RIGHT_TURN ? "right_turn" : "left_turn") << " angle: " << angle << " is_inside: " << is_inside << "\n";
  }

  return is_inside;
}




TEST_CASE("test BouncingVertices insideAngleRange ") {
  SECTION("insideAngleRange convex 0..180 reflex 180..360") {
    cgal::Point_2 p1(10,10), p2(10,22), p3(22,32), p4(32,54), p5(50,61);
    cgal::Segment_2 prev(p1, p2), e_1_old(p2, p3), e_2_old(p3, p4), next(p4, p5);

    cgal::Point_2 bounced(26,31);
    cgal::Segment_2 e_1(p2, bounced), e_2(bounced, p4);

    pl::Filter convex_angle_range("convex angle range", 0, 180), reflex_angle_range("reflex angle range", 180, 300);
    pl::FilterList filters = {convex_angle_range, reflex_angle_range};

    CHECK( insideAngleRange(e_1, e_2, e_1_old, e_2_old, next, prev, filters) );
  }

  SECTION("insideAngleRange convex 0..180 reflex 180..360 false of p3 converts to convex point") {
    cgal::Point_2 p1(10,10), p2(10,22), p3(22,32), p4(32,54), p5(50,61);
    cgal::Segment_2 prev(p1, p2), e_1_old(p2, p3), e_2_old(p3, p4), next(p4, p5);

    cgal::Point_2 bounced(5, 40);
    cgal::Segment_2 e_1(p2, bounced), e_2(bounced, p4);

    pl::Filter convex_angle_range("convex angle range", 0, 180), reflex_angle_range("reflex angle range", 180, 300);
    pl::FilterList filters = {convex_angle_range, reflex_angle_range};

    CHECK_FALSE( insideAngleRange(e_1, e_2, e_1_old, e_2_old, next, prev, filters) );
  }

  SECTION("insideAngleRange convex 0..180 reflex 180..360") {
    cgal::Point_2 p1(10,10), p2(10,22), p3(22,32), p4(32,54), p5(50,61);
    cgal::Segment_2 prev(p1, p2), e_1_old(p2, p3), e_2_old(p3, p4), next(p4, p5);

    cgal::Point_2 bounced(50, 54);
    cgal::Segment_2 e_1(p2, bounced), e_2(bounced, p4);

    pl::Filter convex_angle_range("convex angle range", 0, 180), reflex_angle_range("reflex angle range", 180, 360);
    pl::FilterList filters = {convex_angle_range, reflex_angle_range};

    CHECK( insideAngleRange(e_1, e_2, e_1_old, e_2_old, next, prev, filters) );
  }

  SECTION("insideAngleRange convex 0..180 reflex 180..300 false of p3 convert to point with more then the possible 300°") {
    cgal::Point_2 p1(10,10), p2(10,22), p3(22,32), p4(32,54), p5(50,61);
    cgal::Segment_2 prev(p1, p2), e_1_old(p2, p3), e_2_old(p3, p4), next(p4, p5);

    cgal::Point_2 bounced(50, 54);
    cgal::Segment_2 e_1(p2, bounced), e_2(bounced, p4);

    pl::Filter convex_angle_range("convex angle range", 0, 180), reflex_angle_range("reflex angle range", 180, 300);
    pl::FilterList filters = {convex_angle_range, reflex_angle_range};

    CHECK_FALSE( insideAngleRange(e_1, e_2, e_1_old, e_2_old, next, prev, filters) );
  }

  SECTION("insideAngleRange convex 0..180 reflex 180..300") {
    cgal::Point_2 p1(10,10), p2(10,22), p3(22,32), p4(32,54), p5(50,61);
    cgal::Segment_2 prev(p1, p2), e_1_old(p2, p3), e_2_old(p3, p4), next(p4, p5);

    cgal::Point_2 bounced(50, 54);
    cgal::Segment_2 e_1(p2, bounced), e_2(bounced, p4);

    pl::Filter convex_angle_range("convex angle range", 0, 180), reflex_angle_range("reflex angle range", 180, 300);
    pl::FilterList filters = {convex_angle_range, reflex_angle_range};

    CHECK_FALSE( insideAngleRange(e_1, e_2, e_1_old, e_2_old, next, prev, filters) );
  }
}

TEST_CASE("test BouncingVertices calculate angle") {
  SECTION("calculateAngle") {
    cgal::Point_2 p1(10,22), p2(22,32), p3(32,54);
    cgal::Segment_2 e_1(p1, p2), e_2(p2, p3);

    double angle = 360 - calculateAngle(e_1, e_2);
    CHECK( (205.74 < angle && angle < 205.76) );
  }

  SECTION("calculateAngle") {
    cgal::Point_2 p1(10,22), p2(26,31), p3(32,54);
    cgal::Segment_2 e_1(p1, p2), e_2(p2, p3);

    double angle = 360 - calculateAngle(e_1, e_2);
    CHECK( (226.020 < angle && angle < 226.022) );
  }
}
