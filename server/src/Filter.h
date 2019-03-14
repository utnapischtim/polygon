#ifndef FILTER_H_
#define FILTER_H_

#include <iostream>
#include <tuple>
#include <string>
#include <map>

#include <json.hpp>
#include <docopt.h>

#include "Point.h"
#include "Filter.h"

namespace pl {

struct Filter {
  std::string arg;
  std::string name;
  std::string desc;
  int key;
  std::string type;
  int val;
  int lower_bound;
  int upper_bound;

  Filter()
    : arg{}, name{}, desc{}, key{}, type{}, val{}, lower_bound{}, upper_bound{}
    {}

  Filter(std::string n, int lower, int upper)
    : Filter()
    {
      name = n;
      lower_bound = lower;
      upper_bound = upper;
    }

  Filter(std::string a, std::string n, std::string d, int k, std::string t, int v)
    : arg(a), name(n), desc(d), key(k), type(t), val(v), lower_bound(0), upper_bound(0)
    {}

  Filter(nlohmann::json obj)
    : Filter(obj["arg"], obj["name"], obj["desc"], obj["key"], obj["type"], -1)
    {
      if (0 < obj.count("val"))
        val = obj["val"];
    }

  double lower_bound_radian() const {
    return lower_bound * (M_PI / 180);
  }

  double upper_bound_radian() const {
    return upper_bound * (M_PI / 180);
  }
};

Filter operator-(const double lhs, const Filter &rhs);

using FilterList = std::vector<Filter>;

FilterList createFilterList(nlohmann::json activated_filters);
FilterList createFilterList(std::map<std::string, docopt::value> args);

nlohmann::json getListOfFilters();
void printFilters();

std::optional<Filter> find(const FilterList &filters, std::string name);

}

#endif
