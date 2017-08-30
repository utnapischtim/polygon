#include <algorithm>
#include <string>
#include <tuple>

#include "json.hpp"
#include "easylogging++.h"

#include "Filter.h"
#include "Point.h"

pl::FilterList pl::createFilterList(nlohmann::json activated_filters) {
  pl::FilterList filters;

  for (auto obj : activated_filters)
    filters.push_back({obj});

  return filters;
}

nlohmann::json pl::getListOfFilters() {
  nlohmann::json obj = {
    {{"name", "reflex points"}, {"desc", ""}, {"key", 0}, {"type", "number"}},
    {{"name", "konvex points"}, {"desc", ""}, {"key", 1}, {"type", "number"}},
    {{"name", "reflex edge chain"}, {"desc", ""}, {"key", 2}, {"type", "number"}},
    {{"name", "konvex edge chain"}, {"desc", ""}, {"key", 3}, {"type", "number"}},
    {{"name", "lights to illuminate"}, {"desc", ""}, {"key", 4}, {"type", "number"}}
  };

  return obj;
}

pl::Filter find(pl::FilterList filters, std::string name) {
  return *std::find_if(filters.begin(), filters.end(), [&](auto f) { return f.name == name; });
}
