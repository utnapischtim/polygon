#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <vector>

#include "json.hpp"

#include "Filter.h"

namespace pl {

template<typename T>
struct Point {
  T x;
  T y;
};

using PointList = std::vector<Point<double>>;

struct Generator {
  std::string name;
  std::string desc;
  int key;
};


nlohmann::json getListOfGenerators();
Generator createGenerator(nlohmann::json obj);

nlohmann::json generatePointList(nlohmann::json opts);

PointList randomTwoPeasants(FilterList activatedFilters);

}

#endif
