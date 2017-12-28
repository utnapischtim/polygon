#include "catch.hpp"

#include "../src/SteadyGrowth.cpp"

TEST_CASE("test SteadyGrowth") {
  el::Loggers::setVerboseLevel(9);

  SECTION("calculateNearestPoint") {
    cgal::Point_2 p = {10,20};
    cgal::Point_2 k = {20, 18};
    pl::PointList hull = {{20,18}, {39,22}, {21,10}};

    Iter nearest_point = calculateNearestPoint(hull, p);
    CHECK( (*nearest_point == k) );
  }

  SECTION("calculateNearestPoint 2") {
    cgal::Point_2 p = {20,41};
    cgal::Point_2 k = {10,20};
    pl::PointList hull = {{10,20}, {39,22}, {1,11}, {21,10}};

    Iter nearest_point = calculateNearestPoint(hull, p);
    CHECK( (*nearest_point == k) );
  }

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
    cgal::Point_2 p = {39,22};
    pl::PointList hull = {{10,20}, {68,19}, {48,9}, {21,10}, {1,11}};
    auto [s_l, s_r] = locateSupportVertices(hull, p);

    addToConvexHull(hull, p, s_l, s_r);

    pl::PointList hull_result = {{10,20}, {39,22}, {68,19}, {48,9}, {21,10}, {1,11}};

    bool is_equal = true;
    for (size_t i = 0; i < hull_result.size(); ++i)
      is_equal = is_equal && hull[i] == hull_result[i];

    CHECK( (is_equal) );
  }
}
