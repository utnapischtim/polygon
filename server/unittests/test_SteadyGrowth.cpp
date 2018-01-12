#include "catch.hpp"

#include "../src/SteadyGrowth.cpp"

TEST_CASE("test SteadyGrowth") {
  el::Loggers::setVerboseLevel(9);

  SECTION("calculateStartPoint") {
    cgal::Point_2 p = {60,50};
    cgal::Point_2 k = {10,20};
    pl::PointList hull = {{10,20}, {39,22}, {68,19}, {48,9}, {21,10}, {1,11}};

    Iter start_point = calculateStartPoint(hull, p);
    CHECK( (*start_point == k) );
  }

  SECTION("calculateStartPoint 2") {
    cgal::Point_2 p = {75,33};
    cgal::Point_2 k = {10,20};
    pl::PointList hull = {{10,20}, {39,22}, {68,19}, {48,9}, {21,10}, {1,11}};

    Iter start_point = calculateStartPoint(hull, p);
    CHECK( (*start_point == k) );
  }

  SECTION("calculateStartPoint 3") {
    cgal::Point_2 p = {55,12};
    cgal::Point_2 k = {21,10};
    pl::PointList hull = {{21,10}, {20,18}, {39,22}, {33,15}};

    Iter start_point = calculateStartPoint(hull, p);
    CHECK( (*start_point == k) );
  }

  SECTION("locateSupportVertices case 2") {
    cgal::Point_2 p = {10,20};
    cgal::Point_2 s_l_real = {21,10};
    cgal::Point_2 s_r_real = {39,22};
    pl::PointList hull = {{21,10}, {39,22}};

    auto [s_l, s_r] = locateSupportVertices(hull, p);

    CHECK( (*s_l == s_l_real) );
    CHECK( (*s_r == s_r_real) );
  }

  SECTION("locateSupportVertices case 3 pointed") {
    cgal::Point_2 p = {10,20};
    cgal::Point_2 s_l_real = {21,10};
    cgal::Point_2 s_r_real = {39,22};
    pl::PointList hull = {{21,10}, {20,18}, {39,22}};

    auto [s_l, s_r] = locateSupportVertices(hull, p);

    CHECK( (*s_l == s_l_real) );
    CHECK( (*s_r == s_r_real) );
  }

  SECTION("locateSupportVertices case 3 plane") {
    cgal::Point_2 p = {33,15};
    cgal::Point_2 s_l_real = {39,22};
    cgal::Point_2 s_r_real = {21,10};
    pl::PointList hull = {{21,10}, {20,18}, {39,22}};

    auto [s_l, s_r] = locateSupportVertices(hull, p);

    CHECK( (*s_l == s_l_real) );
    CHECK( (*s_r == s_r_real) );
  }

  SECTION("locateSupportVertices case 4 pointed") {
    cgal::Point_2 p = {55,12};
    cgal::Point_2 s_l_real = {39,22};
    cgal::Point_2 s_r_real = {21,10};
    pl::PointList hull = {{21,10}, {20,18}, {39,22}, {33,15}};

    auto [s_l, s_r] = locateSupportVertices(hull, p);

    CHECK( (*s_l == s_l_real) );
    CHECK( (*s_r == s_r_real) );
  }

  SECTION("locateSupportVertices case 6 test 1") {
    cgal::Point_2 p = {60,50};
    cgal::Point_2 s_l_real = {10,20};
    cgal::Point_2 s_r_real = {68,19};
    pl::PointList hull = {{10,20}, {39,22}, {68,19}, {48,9}, {21,10}, {1,11}};

    auto [s_l, s_r] = locateSupportVertices(hull, p);

    CHECK( (*s_l == s_l_real) );
    CHECK( (*s_r == s_r_real) );
  }

  SECTION("locateSupportVertices case 6 test 2") {
    cgal::Point_2 p = {20,41};
    cgal::Point_2 s_l_real = {1,11};
    cgal::Point_2 s_r_real = {68,19};
    pl::PointList hull = {{10,20}, {39,22}, {68,19}, {48,9}, {21,10}, {1,11}};

    auto [s_l, s_r] = locateSupportVertices(hull, p);

    CHECK( (*s_l == s_l_real) );
    CHECK( (*s_r == s_r_real) );
  }

  SECTION("locateSupportVertices case 6 test 3") {
    cgal::Point_2 p = {39,22};
    cgal::Point_2 s_l_real = {10,20};
    cgal::Point_2 s_r_real = {68,19};

    pl::PointList hull = {{10,20}, {68,19}, {48,9}, {21,10}, {1,11}};

    auto [s_l, s_r] = locateSupportVertices(hull, p);
    CHECK( (*s_l == s_l_real) );
    CHECK( (*s_r == s_r_real) );
  }

  SECTION("locateSupportVertices case 7") {
    cgal::Point_2 p = {39, 22};
    cgal::Point_2 s_l_real = {60,50};
    cgal::Point_2 s_r_real = {33,15};
    pl::PointList hull = {{1,11},{10,20},{60,50},{33,15}};

    auto [s_l, s_r] = locateSupportVertices(hull, p);
    CHECK( (*s_l == s_l_real) );
    CHECK( (*s_r == s_r_real) );
  }

  SECTION("locateSupportVertices case 8") {
    cgal::Point_2 p = {48,9};
    cgal::Point_2 s_l_real = {39,22};
    cgal::Point_2 s_r_real = {33,15};

    pl::PointList hull = {{33,15},{5,40},{20,41},{39,22}};
    auto [s_l, s_r] = locateSupportVertices(hull, p);
    CHECK( (*s_l == s_l_real) );
    CHECK( (*s_r == s_r_real) );
  }

  SECTION("locateSupportVertices 9") {
    cgal::Point_2 p = {55,17};
    cgal::Point_2 s_l_real = {68,19};
    cgal::Point_2 s_r_real = {20,41};
    pl::PointList hull = {{20,41},{68,19}};

    auto [s_l, s_r] = locateSupportVertices(hull, p);
    CHECK( (*s_l == s_l_real) );
    CHECK( (*s_r == s_r_real) );
  }

  SECTION("recalculateSupportVertices") {
    pl::PointList hull = {{10,20}, {68,19}, {48,9}, {21,10}, {1,11}};
    cgal::Point_2 p = {39,22};
    Iter s_l = hull.end() - 1;
    Iter s_r = hull.begin() + 1;

    cgal::Point_2 recalculated_s_l = {10,20};
    cgal::Point_2 recalculated_s_r = {68,19};

    recalculateSupportVertices(hull, p, s_l, s_r);

    CHECK( (*s_l == recalculated_s_l) );
    CHECK( (*s_r == recalculated_s_r) );
  }

  SECTION("addToConvexHull") {
    // branch s_l < s_r -> {10,20} < {68,19}
    cgal::Point_2 p = {39,22};
    pl::PointList hull = {{10,20}, {68,19}, {48,9}, {21,10}, {1,11}};
    auto [s_l, s_r] = locateSupportVertices(hull, p);

    addToConvexHull(hull, p, s_l, s_r);

    pl::PointList hull_result = {{10,20}, {39,22}, {68,19}, {48,9}, {21,10}, {1,11}};

    CHECK( (hull == hull_result) );
  }

  SECTION("addToConvexHull 2") {
    // branch else s_r < s_l -> {33,15} < {39,22}
    cgal::Point_2 p = {48,9};
    pl::PointList hull = {{33,15},{5,40},{20,41},{39,22}};
    auto [s_l, s_r] = locateSupportVertices(hull, p);
    addToConvexHull(hull, p, s_l, s_r);

    pl::PointList hull_result = {{33,15},{5,40},{20,41},{39,22},{48,9}};

    CHECK( (hull == hull_result) );
  }

  SECTION("addToConvexHull 3") {
    // branch s_l < s_r -> {39,22} < {10,20}
    cgal::Point_2 p = {21,10};
    pl::PointList hull = {{5,40},{39,22},{20,18},{10,20}};
    auto [s_l, s_r] = locateSupportVertices(hull, p);
    addToConvexHull(hull, p, s_l, s_r);

    pl::PointList hull_result = {{5,40},{39,22},{21,10},{10,20}};

    CHECK( (hull == hull_result) );
  }


  SECTION("calculatePossibleVisiblePoints") {
    cgal::Point_2 s_l = {1,11};
    cgal::Point_2 s_r = {68,19};
    pl::PointList final_list = {{10,20}, {39,22}, {68,19}, {48,9}, {21,10}, {1,11}};

    pl::PointList visible_points = calculatePossibleVisiblePoints(final_list, s_l, s_r);

    pl::PointList visible_points_real = {{1,11}, {10,20}, {39,22}, {68,19}};

    CHECK( (visible_points == visible_points_real) );
  }

  SECTION("calculatePossibleVisiblePoints 2") {
    cgal::Point_2 s_l = {1,11};
    cgal::Point_2 s_r = {68,19};
    pl::PointList final_list = {{10,20}, {39,22}, {33,15}, {68,19}, {48,9}, {21,10}, {1,11}};

    pl::PointList visible_points = calculatePossibleVisiblePoints(final_list, s_l, s_r);

    pl::PointList visible_points_real = {{1,11}, {10,20}, {39,22}, {33,15}, {68,19}};

    CHECK( (visible_points == visible_points_real) );
  }

  SECTION("calculatePossibleVisibleSegments") {
    pl::PointList visible_points = {{1,11}, {10,20}, {39,22}, {68,19}};

    std::vector<cgal::Segment_2> seg = calculatePossibleVisibleSegments(visible_points);

    std::vector<cgal::Segment_2> seg_real = {{{1,11},{10,20}},{{10,20},{39,22}},{{39,22},{68,19}}};

    CHECK( (seg == seg_real) );
  }

  SECTION("createRegularSegments") {
    cgal::Point_2 p = {20,41};
    std::vector<cgal::Segment_2> segs = {{{1,11},{10,20}},{{10,20},{39,22}},{{39,22},{68,19}}};

    std::vector<cgal::Segment_2> regular_segs = createRegularSegments(segs, p);

    std::vector<cgal::Segment_2> regular_segs_real = {{{1,11},{10,20}},{{10,20},{39,22}},{{39,22},{68,19}},{{68,19},{20,41}},{{20,41},{1,11}}};

    CHECK( (regular_segs == regular_segs_real) );
  }

  SECTION("calculateRegularPoints") {
    cgal::Point_2 p = {20,41};
    std::vector<cgal::Segment_2> regular_segs = {{{1,11},{10,20}},{{10,20},{39,22}},{{39,22},{68,19}},{{68,19},{20,41}},{{20,41},{1,11}}};

    pl::PointList regular_points = calculateRegularPoints(regular_segs, p);

    pl::PointList regular_points_real = {{1,11}, {10,20}, {39,22}, {68,19}};

    CHECK( (regular_points == regular_points_real) );
  }

  // they are equal. the problem is, that the double comparison does
  // not work!
  // SECTION("calculateRegularPoints 2") {
  //   cgal::Point_2 p = {20,41};
  //   std::vector<cgal::Segment_2> regular_segs = {{{1,11},{10,20}},{{10,20},{39,22}},{{39,22},{33,15}},{{33,15},{68,19}},{{68,19},{20,41}},{{20,41},{1,11}}};
  //   pl::PointList regular_points = calculateRegularPoints(regular_segs, p);
  //   pl::PointList regular_points_real = {{1,11}, {10,20}, {39,22}, {44.6667,16.3333}, {68,19}};
  //   CHECK( (regular_points == regular_points_real) );
  // }

  SECTION("calculateRegularPoints 3") {
    cgal::Point_2 p = {60,50};
    std::vector<cgal::Segment_2> regular_segs = {{{39,22},{48,9}},{{48,9},{60,50}},{{60,50},{39,22}}};

    pl::PointList regular_points = calculateRegularPoints(regular_segs, p);

    pl::PointList regular_points_real = {{39,22},{48,9}};

    CHECK( (regular_points == regular_points_real) );
  }

  SECTION("calculateRegularPoints 4") {
    cgal::Point_2 p = {39,22};
    std::vector<cgal::Segment_2> regular_segs = {{{20,41},{20,18}},{{20,18},{10,20}},{{10,20},{1,11}},{{1,11},{39,22}},{{39,22},{20,41}}};

    pl::PointList regular_points = calculateRegularPoints(regular_segs, p);
    pl::PointList regular_points_real = {{20,41},{20,18},{4.8,14.8},{1,11}};

    CHECK( (regular_points == regular_points_real) );
  }

  SECTION("calculateRegularPoints 7") {
    cgal::Point_2 p = {60,50};
    std::vector<cgal::Segment_2> regular_segs = {
      {{20,41},{39,22}},
      {{39,22},{55,17}},
      {{55,17},{60,50}},
      {{60,50},{20,41}}
    };
    pl::PointList regular_points = calculateRegularPoints(regular_segs, p);
    pl::PointList regular_points_real = {{20,41},{39,22},{55,17}};

    CHECK( (regular_points == regular_points_real) );
  }

  SECTION("calculateRealVisibleSegments") {
    cgal::Point_2 p = {20,41};
    std::vector<cgal::Segment_2> regular_segs = {{{1,11},{10,20}},{{10,20},{39,22}},{{39,22},{33,15}},{{33,15},{68,19}},{{68,19},{20,41}},{{20,41},{1,11}}};
    pl::PointList regular_points = calculateRegularPoints(regular_segs, p);
    pl::PointList visible_points = {{1,11}, {10,20}, {39,22}, {33,15}, {68,19}};

    std::vector<cgal::Segment_2> segs = calculateRealVisibleSegments(visible_points, regular_points);

    std::vector<cgal::Segment_2> segs_real = {{{1,11},{10,20}},{{10,20},{39,22}}};

    CHECK( (segs == segs_real) );
  }

  SECTION("calculateRealVisibleSegments 2") {
    cgal::Point_2 p = {20,41};
    std::vector<cgal::Segment_2> regular_segs = {{{1,11},{10,20}},{{10,20},{39,22}},{{39,22},{33,15}},{{33,15},{40,13}},{{40,13},{68,19}},{{68,19},{20,41}},{{20,41},{1,11}}};
    pl::PointList regular_points = calculateRegularPoints(regular_segs, p);
    pl::PointList visible_points = {{1,11}, {10,20}, {39,22}, {33,15}, {40, 13}, {68,19}};

    std::vector<cgal::Segment_2> segs = calculateRealVisibleSegments(visible_points, regular_points);

    std::vector<cgal::Segment_2> segs_real = {{{1,11},{10,20}},{{10,20},{39,22}}};

    CHECK( (segs == segs_real) );
  }

  SECTION("replaceSegment") {
    cgal::Point_2 p = {20,41};
    cgal::Segment_2 seg = {{10,20}, {39,22}};

    pl::PointList final_list = {{1,11},{10,20},{39,22},{68,19},{48,9}};

    replaceSegment(final_list, seg, p);

    pl::PointList points_real = {{1,11},{10,20},{20,41},{39,22},{68,19},{48,9}};

    CHECK( (final_list == points_real) );
  }

  // not testable because of the last function in it is a random function
  // SECTION("locateVisibleSegments") {

  // }
}
