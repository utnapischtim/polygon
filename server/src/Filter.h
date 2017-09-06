#ifndef FILTER_H_
#define FILTER_H_

#include <iostream>
#include <tuple>
#include <string>
#include <map>

#include "json.hpp"
#include "docopt.h"

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

  Filter(std::string a, std::string n, std::string d, int k, std::string t, int v)
    : arg(a), name(n), desc(d), key(k), type(t), val(v)
    {}

  Filter(nlohmann::json obj)
    : Filter(obj["arg"], obj["name"], obj["desc"], obj["key"], obj["type"], -1)
    {
      if (0 < obj.count("val"))
        val = obj["val"];
    }
};

using FilterList = std::vector<Filter>;

FilterList createFilterList(nlohmann::json activated_filters);
FilterList createFilterList(std::map<std::string, docopt::value> args);

nlohmann::json getListOfFilters();
void printFilters();

std::optional<Filter> find(const FilterList &filters, std::string name);

}

#endif
