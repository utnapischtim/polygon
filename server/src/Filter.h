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
  int val;

  Filter(std::string n, std::string d, int k, std::string t, int v)
    : name(n), desc(d), key(k), type(t), val(v)
    {}

  Filter(nlohmann::json obj)
    : Filter(obj["name"], obj["desc"], obj["key"], obj["type"], obj["val"])
    {}
};

using FilterList = std::vector<Filter>;

FilterList createFilterList(nlohmann::json activated_filters);

nlohmann::json getListOfFilters();

std::optional<Filter> find(const FilterList &filters, std::string name);

}

#endif
