#ifndef FILTER_H_
#define FILTER_H_

#include <iostream>
#include <tuple>
#include <string>

#include "json.hpp"

#include "Point.h"
#include "Filter.h"

namespace pl {

struct Filter {
  std::string name;
  std::string desc;
  int key;
  std::string type;
  std::string art;
  int val;

  Filter(std::string n, std::string d, int k, std::string t, std::string a, int v)
    : name(n), desc(d), key(k), type(t), art(a), val(v)
    {}

  Filter(nlohmann::json obj)
    : Filter(obj["name"], obj["desc"], obj["key"], obj["type"], obj["art"], obj["val"])
    {}
};

using FilterList = std::vector<Filter>;

std::tuple<FilterList, FilterList> createFilterList(nlohmann::json activated_filters);

nlohmann::json getListOfFilters();

PointList filter(const PointList &point_list, const FilterList &global_filter);


// if A, B or only A have the initial point values aka 0,0 it should
// return always true
template<typename T>
bool isConvexTriangle(const Triangle<T> &triangle) {

  auto a = triangle.B - triangle.A;
  auto b = triangle.C - triangle.B;

  auto quadrant_a = calculateQuadrant(a);
  auto quadrant_b = calculateQuadrant(b);

  auto my = static_cast<double>(a.y)/a.x;
  auto ny = static_cast<double>(b.y)/b.x;

  // TODO
  // initial value for Point 0,0 is not good, because 0,0 is a valid
  // point
  if (triangle.A == Point(0,0))
    return true;

  // TODO
  // it should be possible to calculate that, because this way it is
  // very error prone
  if (quadrant_a == 1)
    return (quadrant_b == 4 || (quadrant_b == 1 && ny < my) || (quadrant_b == 3 && ny > my));

  if (quadrant_a == 2)
    return (quadrant_b == 1 || (quadrant_b == 2 && ny > my) || (quadrant_b == 4 && ny < my));

  if (quadrant_a == 3)
    return (quadrant_b == 2 || (quadrant_b == 3 && ny < my) || (quadrant_b == 1 && ny > my));

  if (quadrant_a == 4)
    return (quadrant_b == 3 || (quadrant_b == 4 && ny > my) || (quadrant_b == 2 && ny < my));
}

}

#endif
