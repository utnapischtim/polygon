#include "catch.hpp"
#include "Filter.h"
#include "Point.h"

TEST_CASE("test Filter") {


  SECTION("isConvexTriangle") {
    pl::Triangle<int> triangle;

    SECTION("one point") {
      triangle.push({13,8});

      REQUIRE((triangle.A == pl::Point{0, 0}));
      REQUIRE((triangle.B == pl::Point{0, 0}));
      REQUIRE((triangle.C == pl::Point{13, 8}));

      CHECK(isConvexTriangle(triangle));
    }

    SECTION("two points") {
      triangle.push({13,8});
      triangle.push({17,10});

      REQUIRE((triangle.A == pl::Point{0, 0}));
      REQUIRE((triangle.B == pl::Point{13, 8}));
      REQUIRE((triangle.C == pl::Point{17, 10}));

      CHECK(isConvexTriangle(triangle));
    }

    SECTION("three points") {
      triangle.push({13,8});
      triangle.push({17,10});
      triangle.push({21,9});

      REQUIRE((triangle.A == pl::Point{13, 8}));
      REQUIRE((triangle.B == pl::Point{17, 10}));
      REQUIRE((triangle.C == pl::Point{21, 9}));

      CHECK(isConvexTriangle(triangle));
    }

    SECTION("three points false") {
      triangle.push({13,8});
      triangle.push({17,10});
      triangle.push({21,13});

      // problem!!! should be false, but could not be!
      CHECK_FALSE(isConvexTriangle(triangle));
    }

    SECTION("") {
      triangle.push({13,8});
      triangle.push({17,10});
      triangle.push({18,11});

      CHECK_FALSE(isConvexTriangle(triangle));
    }

    SECTION("") {
      triangle.push({13,8});
      triangle.push({17,10});
      triangle.push({21,11});

      CHECK(isConvexTriangle(triangle));
    }

    SECTION("") {
      triangle.push({13,8});
      triangle.push({17,10});
      triangle.push({14,8});

      CHECK(isConvexTriangle(triangle));
    }

    SECTION("") {
      triangle.push({15,12});
      triangle.push({17,10});
      triangle.push({13,7});

      CHECK(isConvexTriangle(triangle));
    }
  }
}
