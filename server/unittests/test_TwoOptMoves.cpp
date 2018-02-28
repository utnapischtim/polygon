#include "easylogging++.h"
#include "catch.hpp"

#include "../src/TwoOptMoves.cpp"

bool map_compare(const auto &lhs, const auto &rhs) {
  auto pred = [](const auto& a, const auto &b) {
    return a.first == b.first && a.second == b.second;
  };
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
}

TEST_CASE("test TwoOptMoves") {
  el::Loggers::setVerboseLevel(9);

  SECTION("to_key") {
    cgal::Segment_2
      s_1 = {{10,20}, {39,22}},
      s_2 = {{39,22}, {68,19}};

    std::string key = to_key(s_1, s_2);
    std::string real = "10.000000,20.000000-39.000000,22.000000-39.000000,22.000000-68.000000,19.000000";

    CHECK( (key == real) );
  }

  SECTION("calcualteIntersection real world example") {
    std::vector<cgal::Segment_2> segments = {
      {{329.41, 755.9}, {1233.45, 71.1812}},   // 0
      {{1233.45, 71.1812}, {736.707, 67.7942}},// 1
      {{736.707, 67.7942}, {844.895, 122.46}}, // 2
      {{844.895, 122.46}, {158.626, 338.007}}, // 3
      {{158.626, 338.007}, {233.614, 369.587}},// 4
      {{233.614, 369.587}, {263.873, 619.035}},// 5
      {{263.873, 619.035}, {189.972, 603.056}},// 6
      {{189.972, 603.056}, {80.2051, 153.456}},// 7
      {{80.2051, 153.456}, {961.668, 612.329}},// 8
      {{961.668, 612.329}, {329.41, 755.9}}};  // 9

    auto intersection = calculateIntersections(segments);

    Intersections real_inters;
    real_inters[to_key(segments[0], segments[8])] = {segments[0], segments[8]};
    real_inters[to_key(segments[3], segments[8])] = {segments[3], segments[8]};

    CHECK( (map_compare(intersection, real_inters)) );
  }

  SECTION("calculateIntersections") {
    //pl::PointList point_list = {{1,11},{33,15},{10,20},{21,10},{39,22},{48,9},{20,41}};
    std::vector<cgal::Segment_2> segments = {
      {{1,11},{33,15}}, // 0
      {{33,15},{10,20}},// 1
      {{10,20},{21,10}},// 2
      {{21,10},{39,22}},// 3
      {{39,22},{48,9}}, // 4
      {{48,9},{20,41}}, // 5
      {{20,41},{1,11}}  // 6
    };

    auto intersection = calculateIntersections(segments);

    Intersections real_inters;
    real_inters[to_key(segments[0], segments[2])] = {segments[0], segments[2]};
    real_inters[to_key(segments[0], segments[3])] = {segments[0], segments[3]};
    real_inters[to_key(segments[3], segments[5])] = {segments[3], segments[5]};
    real_inters[to_key(segments[1], segments[3])] = {segments[1], segments[3]};

    CHECK( (map_compare(intersection, real_inters)) );
  }

  SECTION("resolve Intersections real world example") {
    pl::PointList point_list = {{329.41, 755.9}, {1233.45, 71.1812}, {736.707, 67.7942}, {844.895, 122.46}, {158.626, 338.007}, {233.614, 369.587}, {263.873, 619.035}, {189.972, 603.056}, {80.2051, 153.456}, {961.668, 612.329}};
    std::vector<cgal::Segment_2> segments;
    pl::convert(point_list, segments);
    auto intersections = calculateIntersections(segments);
    resolveIntersections(segments, intersections);
    pl::PointList final_list = calculateFinalList(segments);

    CHECK( (final_list.size() == (point_list.size() + 1)) );
    CHECK( (calculateIntersections(segments).size() == 0) );
  }

  SECTION("resolveIntersections") {
    pl::PointList point_list = {{1,11},{33,15},{10,20},{21,10},{39,22},{48,9},{20,41}};
    std::vector<cgal::Segment_2> segments;
    pl::convert(point_list, segments);
    auto intersections = calculateIntersections(segments);

    resolveIntersections(segments, intersections);

    pl::PointList final_list = calculateFinalList(segments);

    // ATTENTION
    // equality is not possible to check because of the random
    // selector in the resolveIntersections method
    // pl::PointList real_list = {{1,11},{10,20},{21,10},{33,15},{48,9},{39,22},{20,41},{1,11}};
    // CHECK( (final_list == real_list) );

    // but we could check that all elements are used, and the size is
    // correct ...
    CHECK( (final_list.size() == (point_list.size() + 1)) );
    // ... and we have a simple polygon!
    CHECK( (calculateIntersections(segments).size() == 0) );
  }

  SECTION("calculateFinalList source->target") {
    std::vector<cgal::Segment_2> segments = {{{10,20}, {39,22}},
                                             {{39,22}, {68,19}},
                                             {{68,19}, {48,9}},
                                             {{48,9}, {21,10}},
                                             {{21,10}, {1,11}},
                                             {{1,11}, {10,20}}};
    pl::PointList real = {{10,20}, {39,22}, {68,19}, {48,9}, {21,10}, {1,11}, {10,20}};

    pl::PointList list = calculateFinalList(segments);

    for (const auto &l : list)
      VLOG(3) << "l: " << l;

    CHECK( (real == list) );
  }

  SECTION("calculateFinalList misc") {
    std::vector<cgal::Segment_2> segments = {{{10,20}, {39,22}},
                                             {{1,11}, {21,10}},
                                             {{68,19}, {48,9}},
                                             {{48,9}, {21,10}},
                                             {{68,19}, {39,22}},
                                             {{1,11}, {10,20}}};
    pl::PointList real = {{10,20}, {39,22}, {68,19}, {48,9}, {21,10}, {1,11}, {10,20}};

    pl::PointList list = calculateFinalList(segments);

    CHECK( (real == list) );
  }

  SECTION("calculateFinalList twisted") {
    std::vector<cgal::Segment_2> segments = {{{10,20}, {39,22}},
                                             {{68,19}, {39,22}},
                                             {{48,9}, {68,19}},
                                             {{21,10}, {48,9}},
                                             {{1,11}, {21,10}},
                                             {{10,20}, {1,11}}};
    pl::PointList real = {{10,20}, {39,22}, {68,19}, {48,9}, {21,10}, {1,11}, {10,20}};

    pl::PointList list = calculateFinalList(segments);

    CHECK( (real == list) );
  }

  SECTION("calculateFinalList hm") {
    std::vector<cgal::Segment_2> segments = {{{1,11},{10,20}},
                                             {{33,15},{21,10}},
                                             {{10,20},{21,10}},
                                             {{20,41},{39,22}},
                                             {{39,22},{48,9}},
                                             {{20,41},{1,11}},
                                             {{48,9},{33,15}}};

    pl::PointList real = {{1,11},{10,20},{21,10},{33,15},{48,9},{39,22},{20,41},{1,11}};
    pl::PointList list = calculateFinalList(segments);

    CHECK( (real == list) );
  }
}
