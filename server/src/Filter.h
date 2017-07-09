#ifndef FILTER_H_
#define FILTER_H_

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

}

#endif
