#include <limits>
#include <cstdlib>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "catch.hpp"

#include "../src/cgal_helper.cpp"
#include "../src/BouncingVerticesSettings.cpp"
#include "../src/CalculateBouncingRange.cpp"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

// 2018-10-25 22:31:30,989 VER-3 [default] calculateBouncingInterval random_angle: 2.75119 bouncing_point: 1283.01 710.277
// 2018-10-25 22:31:30,989 VER-3 [default] calculateIntersectionFreeRange
// 2018-10-25 22:31:30,989 VER-3 [default] SamplingGrid intersection width: 1500 height: 800 line: -0.380562 -0.924756 1125.65
// 2018-10-25 22:31:30,989 VER-3 [default] SamplingGrid intersection segment: 1500 800 0 800
// 2018-10-25 22:31:30,990 VER-3 [default] SamplingGrid intersection inter: 1013.88 800
// 2018-10-25 22:31:30,990 VER-3 [default] SamplingGrid intersection hm: 0 -1 800
// 2018-10-25 22:31:30,990 VER-3 [default] SamplingGrid intersection point: 1013.88 800
// 2018-10-25 22:31:30,990 VER-3 [default] SamplingGrid intersection segment: 1500 0 1500 800
// 2018-10-25 22:31:30,990 VER-3 [default] SamplingGrid intersection hm: -1 0 1500
// 2018-10-25 22:31:30,990 VER-3 [default] SamplingGrid intersection point: 1500 599.949 <------ why no inter
// 2018-10-25 22:31:30,990 VER-3 [default] SamplingGrid intersection segment: 0 0 1500 0
// 2018-10-25 22:31:30,990 VER-3 [default] SamplingGrid intersection hm: 0 1 -0
// 2018-10-25 22:31:30,990 VER-3 [default] SamplingGrid intersection point: 2957.86 0
// 2018-10-25 22:31:30,990 VER-3 [default] SamplingGrid intersection segment: 0 800 0 0
// 2018-10-25 22:31:30,990 VER-3 [default] SamplingGrid intersection hm: 1 0 -0
// 2018-10-25 22:31:30,990 VER-3 [default] SamplingGrid intersection point: -0 1217.24
// intersection with border does have more then 2 or less then 2 intersections


// https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon almost_equal
bool compare(const cgal::Line_2 &a, const cgal::Line_2 &b) {
  auto epsilon = 1.0e-10;

  bool a_equal = std::abs(a.a() - b.a()) <= epsilon;
  bool b_equal = std::abs(a.b() - b.b()) <= epsilon;
  bool c_equal = std::abs(a.c() - b.c()) <= epsilon;

  bool both_vertical = a.b() <= epsilon && b.b() <= epsilon;
  bool both_horizontal = a.a() <= epsilon && b.a() <= epsilon;

  return (a_equal && b_equal && c_equal) || both_vertical || both_horizontal;
}

// bool compare(const cgal::Point_2 &a, const cgal::Point_2 &b, const double epsilon = 1.0e-10) {
//   bool x_equal = std::abs(a.x() - b.x()) <= epsilon;
//   bool y_equal = std::abs(a.y() - b.y()) <= epsilon;

//   return x_equal && y_equal;
// }

bool compare(const cgal::Vector_2 &a, const cgal::Vector_2 &b) {
  auto epsilon = 1.0e-10;

  bool x_equal = std::abs(a.x() - b.x()) <= epsilon;
  bool y_equal = std::abs(a.y() - b.y()) <= epsilon;

  return x_equal && y_equal;
}

bool compare(const cgal::Segment_2 &a, const cgal::Segment_2 &b) {
  return compare(a.source(), b.source(), 1.0e-4) && compare(a.target(), b.target(), 1.0e-4);
}

TEST_CASE("test compare function") {
  SECTION("test compare two points int") {
    cgal::Point_2 A(2,1), B(2,1);

    CHECK( compare(A, B) );
  }

  SECTION("test compare two points beeing false int") {
    cgal::Point_2 A(2,1), B(2,2);

    CHECK( !compare(A, B) );
  }

  SECTION("test compare two points double") {
    cgal::Point_2 A(2.0,1.0), B(2.0,1.0);

    CHECK( compare(A, B) );
  }

  SECTION("test compare two points beeing false double") {
    cgal::Point_2 A(2.0,1.0), B(2.0,1.1);

    CHECK( !compare(A, B) );
  }

  SECTION("test compare two lines int") {
    cgal::Point_2 A(2,1), B(3,2), C(2,1), D(3,2);
    cgal::Line_2 ab(A, B), cd(C, D);

    CHECK( compare(ab, cd) );
  }

  SECTION("test compare two lines double") {
    cgal::Point_2 A(2.0,1.0), B(2.0,1.1), C(2.0,1.0), D(2.0,1.1);
    cgal::Line_2 ab(A, B), cd(C, D);

    CHECK( compare(ab, cd) );
  }

  SECTION("test compare two lines false int") {
    cgal::Point_2 A(2,1), B(2,1), C(4,5), D(10,3);
    cgal::Line_2 ab(A, B), cd(C, D);

    CHECK( !compare(ab, cd) );
  }

  SECTION("test compare two lines false double") {
    cgal::Point_2 A(2.0,1.0), B(2.0,1.1), C(4.0,5.0), D(10.0,3.0);
    cgal::Line_2 ab(A, B), cd(C, D);

    CHECK( !compare(ab, cd) );
  }

  SECTION("") {
    cgal::Point_2 A(2,1), B(2, 4);

    cgal::Line_2 ab(A, B);
    cgal::Line_2 abc(-3, 0, -6);

    CHECK( (compare(ab, abc)) );
  }
}

TEST_CASE("test intersection function") {
  cgal::Point_2 A(2,2), B(3,4), C(9,6), D(12,1), E(8,2), F(5,1), G(5,7);

  SECTION("test intersection with vertical line") {
    std::vector<cgal::Point_2> poly = {A, B, C, D};
    cgal::Line_2 line = {F, G};

    cgal::Segment_2 real = {{5, 14/3.0},{5,1.7}};
    cgal::Segment_2 inter = intersection(poly, line);

    CHECK( compare(real, inter) );
  }

  SECTION("test compare two points beeing false int") {
    std::vector<cgal::Point_2> poly = {A, B, C, D};
    cgal::Line_2 line = {A, E};

    cgal::Segment_2 real = {{2,2},{11.4,2}};
    cgal::Segment_2 inter = intersection(poly, line);

    CHECK( compare(real, inter) );
  }
}

TEST_CASE("test CalculateBouncingRange rotate") {
  pl::BouncingVerticesSettings bvs{};
  std::vector<cgal::Segment_2> segments;
  pl::CalculateBouncingRange cbr{bvs, segments};

  SECTION("rotate by angle pi/2") {
    double right_angle = M_PI / 2.0;
    cgal::Point_2 A(2,1), B(5,2), C(1, 4);

    cgal::Line_2 line_real(A, C);
    cgal::Line_2 line_rotated = cbr.rotate(A, B, right_angle);

    // equality operator not working as expected
    CHECK( compare(line_real, line_rotated) );
  }

  SECTION("rotate by angle pi/2") {
    double right_angle = M_PI / 2.0;
    cgal::Point_2 A(2,1), B(5,1), C(2, 4);

    cgal::Line_2 line_real(A, C);
    cgal::Line_2 line_rotated = cbr.rotate(A, B, right_angle);

    // equality operator not working as expected
    CHECK( compare(line_real, line_rotated) );
  }

  SECTION("rotate by angle pi") {
    double angle = M_PI;
    cgal::Point_2 A(2,1), B(5,1), C(-2, 1);

    cgal::Line_2 line_real(A, C);
    cgal::Line_2 line_rotated = cbr.rotate(A, B, angle);

    // equality operator not working as expected
    CHECK( compare(line_real, line_rotated) );
  }

  SECTION("rotate by angle pi/4") {
    double angle = M_PI / 4;
    cgal::Point_2 A(10,10), B(50,10), C(10, 20);

    cgal::Line_2 line_real(A, C);
    cgal::Line_2 line_rotated = cbr.rotate(A, B, angle);

    // equality operator not working as expected
    CHECK( compare(line_real, line_rotated) );
  }


  SECTION("calculateRandomLine") {
    double angle = M_PI / 2.0;
    cgal::Point_2 A(2,1), B(2,4);

    cbr.bouncing_point = A;

    cgal::Line_2 line_real(A, B);
    cgal::Line_2 random_line = cbr.calculateRandomLine(angle);

    CHECK( compare(line_real, random_line) );
  }

  SECTION("calculateRandomLine") {
    double angle = M_PI / 4.0;
    cgal::Point_2 A(10,10), B(20,10);

    cbr.bouncing_point = A;

    cgal::Line_2 line_real(A, B);
    cgal::Line_2 random_line = cbr.calculateRandomLine(angle);

    CHECK( compare(line_real, random_line) );
  }

  SECTION("calculateBisectingLine") {
    cgal::Point_2 A(2,1), B(6,1), C(4,1), D(4,3);

    cgal::Line_2 line_real(C, D);
    cgal::Line_2 bisecting_line = cbr.calculateBisectingLine(A, B);

    CHECK( compare(line_real, bisecting_line) );
  }

  SECTION("calculateIntersectionWithRandomLine") {
    cgal::Point_2 A(2,1), B(6,-3), C(4,1), D(4,3);
    cbr.bouncing_point = C;
    cbr.random_line = cbr.calculateRandomLine(M_PI / 4);

    cgal::Point_2 inter_real(3,0);
    cgal::Point_2 intersection = cbr.calculateIntersectionWithRandomLine(A, B, 0);

    CHECK( compare(inter_real, intersection) );
  }
}

// TEST_CASE("calculateIntersectionFreeRange") {
//   cgal::Point_2 A(237.135,284.134), B(215.339, 254.134),  C(180.072,242.675), D(144.805,254.134), E(123.009, 284.134), F(123.009, 321.216), G(144.805, 351.216), H(180.072, 362.675), I(215.339, 351.216), J(237.135,321.216);
//   pl::BouncingVerticesSettings bvs{};
//   bvs.sampling_grid = {1500,800};
//   bvs.convex_angle_range = pl::Filter("convex", 0, 180);
//   bvs.reflex_angle_range = pl::Filter("reflex", 180, 360);

//   std::vector<cgal::Segment_2> segments = {{A,B},{B,C},{C,D},{D,E},{E,F},{F,G},{G,H},{H,I},{I,J},{J,A}};
//   pl::CalculateBouncingRange cbr{bvs, segments};

//   SECTION("line 1") {
//     cbr.bouncing_point = B;
//     cbr.random_line = cbr.calculateRandomLine(2.2385);

//     cgal::Segment_2 segment_real;
//     cgal::Segment_2 allowed_segment = cbr.calculateIntersectionFreeRange();

//     VLOG(3) << "allowed_segment: " << allowed_segment;
//   }

// }

// TEST_CASE("calculateIntersectionFreeRange") {
//   cgal::Point_2 A(1007.94, 330.606), B(986.143, 300.606), C(950.876, 289.147), D(915.609, 300.606), E(893.813, 330.606), F(893.813, 367.688), G(915.609, 397.688), H(950.876, 409.147), I(986.143, 397.688), J(1007.94, 367.688);

//   pl::BouncingVerticesSettings bvs{};
//   bvs.sampling_grid = {1500,800};
//   bvs.convex_angle_range = pl::Filter("convex", 0, 180);
//   bvs.reflex_angle_range = pl::Filter("reflex", 180, 360);

//   std::vector<cgal::Segment_2> segments = {{A,B},{B,C},{C,D},{D,E},{E,F},{F,G},{G,H},{H,I},{I,J},{J,A}};
//   pl::CalculateBouncingRange cbr{bvs, segments};

//   SECTION("line 1") {
//     cbr.bouncing_point = B;
//     cbr.random_line = cbr.calculateRandomLine(4.03306);

//     cgal::Segment_2 segment_real;
//     cgal::Segment_2 allowed_segment = cbr.calculateIntersectionFreeRange();

//     VLOG(3) << "allowed_segment: " << allowed_segment;
//   }

// }



TEST_CASE("calculateIntersectionFreeRange") {
  cgal::Point_2 A(2,2),B(3,4),C(9,6),D(12,1),E(8,2),F(5,1);

  pl::BouncingVerticesSettings bvs{};
  bvs.sampling_grid = {13,10};
  bvs.convex_angle_range = pl::Filter("convex", 170, 180);
  bvs.reflex_angle_range = pl::Filter("reflex", 180, 190);

  std::vector<cgal::Segment_2> segments = {{A,B},{B,C},{C,D},{D,E},{E,F},{F,A}};
  pl::CalculateBouncingRange cbr{bvs, segments};

  SECTION("vertical random line") {
    cbr.bouncing_point = F;
    cbr.random_line = cbr.calculateRandomLine(M_PI / 2);

    cgal::Segment_2 segment_real = {{5,0},{5,14/3.0}};
    cgal::Segment_2 allowed_segment = cbr.calculateIntersectionFreeRange();

    CHECK( segment_real == allowed_segment );
  }

  SECTION("vertical random line") {
    cbr.bouncing_point = E;
    cbr.random_line = cbr.calculateRandomLine(M_PI / 2);

    cgal::Segment_2 segment_real = {{8,0},{8,17/3.0}};
    cgal::Segment_2 allowed_segment = cbr.calculateIntersectionFreeRange();

    CHECK( segment_real == allowed_segment );
  }

  SECTION("vertical random line") {
    cbr.bouncing_point = E;
    cbr.random_line = cbr.calculateRandomLine(0);

    cgal::Segment_2 segment_real = {{2,2},{11.4,2}};
    cgal::Segment_2 allowed_segment = cbr.calculateIntersectionFreeRange();

    CHECK( segment_real == allowed_segment );
  }

  SECTION("vertical random line") {
    cbr.bouncing_point = E;
    cbr.random_line = cbr.calculateRandomLine(degreeToRadian(30));

    cgal::Segment_2 segment_real = {{4.5359,0},{10.5252,3.45794}};
    cgal::Segment_2 allowed_segment = cbr.calculateIntersectionFreeRange();

    CHECK( compare(segment_real, allowed_segment) );
  }
}

TEST_CASE("calculateOrientationStability") {
  cgal::Point_2 A(2,2),B(3,4),C(9,6),D(12,1),E(8,2),F(5,1);

  pl::BouncingVerticesSettings bvs{};
  bvs.sampling_grid = {20,10};
  bvs.convex_angle_range = pl::Filter("convex", 170, 180);
  bvs.reflex_angle_range = pl::Filter("reflex", 180, 190);

  std::vector<cgal::Segment_2> segments = {{A,B},{B,C},{C,D},{D,E},{E,F},{F,A}};
  pl::CalculateBouncingRange cbr{bvs, segments};

  SECTION("vertical random line through point F") {
    cbr.bouncing_point = F;
    cbr.random_line = cbr.calculateRandomLine(M_PI / 2);
    cbr.prev_segment = cgal::Segment_2(D, E);
    cbr.next_segment = cgal::Segment_2(A, B);
    cbr.before_point = E;
    cbr.after_point = A;

    cgal::Segment_2 segment_real = {{5,2},{5,0}};
    cgal::Segment_2 allowed_segment = cbr.calculateOrientationStability();

    CHECK( segment_real == allowed_segment );
  }

  SECTION("vertical random line through point E") {
    cbr.bouncing_point = E;
    cbr.random_line = cbr.calculateRandomLine(M_PI / 2);
    cbr.prev_segment = cgal::Segment_2(C, D);
    cbr.next_segment = cgal::Segment_2(F, A);
    cbr.before_point = D;
    cbr.after_point = F;

    cgal::Segment_2 segment_real = {{8,1},{8,23/3.0}};
    cgal::Segment_2 allowed_segment = cbr.calculateOrientationStability();

    CHECK( segment_real == allowed_segment );
  }
}

TEST_CASE("calculateSmallestBouncingInterval") {
  cgal::Point_2 A(2,2),B(3,4),C(9,6),D(12,1),E(8,2),F(5,1);

  pl::BouncingVerticesSettings bvs{};
  bvs.sampling_grid = {20,10};

  std::vector<cgal::Segment_2> segments = {{A,B},{B,C},{C,D},{D,E},{E,F},{F,A}};
  pl::CalculateBouncingRange cbr{bvs, segments};

  SECTION("test 1") {
    cbr.bouncing_point = E;
    std::vector<cgal::Segment_2> to_test_segments = {{{8,1},{8,23/3.0}}, {{8,0},{8,17/3.0}}};

    cgal::Segment_2 allowed_real = {{8,1},{8,17/3.0}};
    cgal::Segment_2 allowed_segment = cbr.calculateSmallestBouncingInterval(to_test_segments);

    CHECK( allowed_real == allowed_segment );
  }
}


// TEST_CASE("test BouncingVertices insideAngleRange ") {
//   pl::Filter convex_angle_range("convex angle range", 0, 180), reflex_angle_range("reflex angle range", 180, 300);
//   pl::FilterList filters = {convex_angle_range, reflex_angle_range};
//   pl::BouncingVerticesSettings bvs;
//   bvs.reflex_angle_range = reflex_angle_range;
//   bvs.convex_angle_range = convex_angle_range;

//   SECTION("insideAngleRange convex 0..180 reflex 180..360") {
//     cgal::Point_2 p1(10,10), p2(10,22), p3(22,32), p4(32,54), p5(50,61);
//     cgal::Segment_2 prev(p1, p2), e_1_old(p2, p3), e_2_old(p3, p4), next(p4, p5);

//     cgal::Point_2 bounced(26,31);
//     cgal::Segment_2 e_1(p2, bounced), e_2(bounced, p4);

//     CHECK( insideAngleRange(e_1, e_2, e_1_old, e_2_old, next, prev, bvs) );
//   }

//   // SECTION("insideAngleRange convex 0..180 reflex 180..360 false of p3 converts to convex point") {
//   //   cgal::Point_2 p1(10,10), p2(10,22), p3(22,32), p4(32,54), p5(50,61);
//   //   cgal::Segment_2 prev(p1, p2), e_1_old(p2, p3), e_2_old(p3, p4), next(p4, p5);

//   //   cgal::Point_2 bounced(5, 40);
//   //   cgal::Segment_2 e_1(p2, bounced), e_2(bounced, p4);

//   //   CHECK_FALSE( insideAngleRange(e_1, e_2, e_1_old, e_2_old, next, prev, bvs) );
//   // }

//   // SECTION("insideAngleRange convex 0..180 reflex 180..360") {
//   //   cgal::Point_2 p1(10,10), p2(10,22), p3(22,32), p4(32,54), p5(50,61);
//   //   cgal::Segment_2 prev(p1, p2), e_1_old(p2, p3), e_2_old(p3, p4), next(p4, p5);

//   //   cgal::Point_2 bounced(50, 54);
//   //   cgal::Segment_2 e_1(p2, bounced), e_2(bounced, p4);

//   //   CHECK( insideAngleRange(e_1, e_2, e_1_old, e_2_old, next, prev, bvs) );
//   // }

//   SECTION("insideAngleRange convex 0..180 reflex 180..300 false of p3 convert to point with more then the possible 300°") {
//     cgal::Point_2 p1(10,10), p2(10,22), p3(22,32), p4(32,54), p5(50,61);
//     cgal::Segment_2 prev(p1, p2), e_1_old(p2, p3), e_2_old(p3, p4), next(p4, p5);

//     cgal::Point_2 bounced(50, 54);
//     cgal::Segment_2 e_1(p2, bounced), e_2(bounced, p4);

//     CHECK_FALSE( insideAngleRange(e_1, e_2, e_1_old, e_2_old, next, prev, bvs) );
//   }

//   SECTION("insideAngleRange convex 0..180 reflex 180..300") {
//     cgal::Point_2 p1(10,10), p2(10,22), p3(22,32), p4(32,54), p5(50,61);
//     cgal::Segment_2 prev(p1, p2), e_1_old(p2, p3), e_2_old(p3, p4), next(p4, p5);

//     cgal::Point_2 bounced(50, 54);
//     cgal::Segment_2 e_1(p2, bounced), e_2(bounced, p4);

//     CHECK_FALSE( insideAngleRange(e_1, e_2, e_1_old, e_2_old, next, prev, bvs) );
//   }
// }

// TEST_CASE("test BouncingVertices calculate angle") {
//   SECTION("calculateAngle") {
//     cgal::Point_2 p1(10,22), p2(22,32), p3(32,54);
//     cgal::Segment_2 e_1(p1, p2), e_2(p2, p3);

//     double angle = 360 - calculateAngle(e_1, e_2);
//     CHECK( (205.74 < angle && angle < 205.76) );
//   }

//   SECTION("calculateAngle") {
//     cgal::Point_2 p1(10,22), p2(26,31), p3(32,54);
//     cgal::Segment_2 e_1(p1, p2), e_2(p2, p3);

//     double angle = 360 - calculateAngle(e_1, e_2);
//     CHECK( (226.020 < angle && angle < 226.022) );
//   }
// }
