#include <iostream>
#include <cmath>

#include "catch.hpp"
#include "Point.h"

TEST_CASE("test Point") {
  pl::Point<int> a{3, 12}, b{5, 17};

  SECTION("operator- for Point") {
    pl::Vector<int> c = b - a;

    CHECK((c.x == 2 && c.y == 5)); // double paranthesis necessary
                                   // because of && is to complex for
                                   // the macro
  }

  SECTION("operator* for Vector") {
    pl::Vector<int> k{3, 12}, l{5, 17};
    int s = k * l;

    CHECK(s == 219);
  }

  SECTION("length for Vector") {
    double s = length(b-a);

    CHECK((5.385164807130 < s && s < 5.385164807140));
  }

  SECTION("phi") {
    pl::Point<int> A{13,8}, B{17,10}, C{21,9};

    auto d = B - A; // {4, 2}
    auto e = C - B; // {4, -1}
    auto s = d*e; // 14
    auto t = length(d) * length(e); // 18.4390889
    auto r = s / t; // .75925660
    auto phi = std::acos(r); // 0.70862

    REQUIRE((d.x == 4 && d.y == 2));
    REQUIRE((e.x == 4 && e.y == -1));
    REQUIRE(s == 14);
    REQUIRE((18.4390888 < t && t < 18.4390890));
    REQUIRE((.75925659 < r && r < .75925661));

    CHECK((0.70861 < phi && phi < 0.70863));
  }

  SECTION("Triangle init") {
    pl::Triangle<int> triangle;
    triangle.push({13,8});

    CHECK((triangle.A == pl::Point{0, 0}));
    CHECK((triangle.B == pl::Point{0, 0}));
    CHECK((triangle.C == pl::Point{13, 8}));
  }

  SECTION("calculateIncludedAngle") {
    pl::Triangle<int> triangle;
    triangle.push({13,8});
    triangle.push({17,10});
    triangle.push({21,9});

    REQUIRE((triangle.A == pl::Point{13, 8}));
    REQUIRE((triangle.B == pl::Point{17, 10}));
    REQUIRE((triangle.C == pl::Point{21, 9}));

    auto phi = pl::calculateIncludedAngle(triangle);

    CHECK((0.70861 < phi && phi < 0.70863));
  }

  SECTION("calculateQuadrant") {
    pl::Vector<int> v_1{4,4}, v_2{-4,4}, v_3{-4,-4}, v_4{4,-4};

    CHECK(calculateQuadrant(v_1) == 1);
    CHECK(calculateQuadrant(v_2) == 2);
    CHECK(calculateQuadrant(v_3) == 3);
    CHECK(calculateQuadrant(v_4) == 4);
  }
}
