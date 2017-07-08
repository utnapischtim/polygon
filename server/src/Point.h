#ifndef POINT_H_
#define POINT_H_

#include <vector>

#include "json.hpp"

namespace pl {

template<typename T>
struct Point {
  T x;
  T y;
};

using PointList = std::vector<Point<double>>;

nlohmann::json to_json(PointList point_list);

}

#endif
