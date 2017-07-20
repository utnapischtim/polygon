#include <tuple>

#include "json.hpp"
#include "easylogging++.h"

#include "Filter.h"
#include "Point.h"

std::tuple<pl::FilterList, pl::FilterList> pl::createFilterList(nlohmann::json activated_filters) {
  pl::FilterList local_filters, global_filters;

  for (auto obj : activated_filters) {
    Filter filter(obj);
    if (obj["art"] == "local")
      local_filters.push_back(filter);
    else
      global_filters.push_back(filter);
  }

  return {local_filters, global_filters};
}

nlohmann::json pl::getListOfFilters() {
  nlohmann::json obj = {
    // {{"name", "reflex points"}, {"desc", ""}, {"key", 0}, {"type", "number"}, {"art", "local"}},
    // {{"name", "konvex points"}, {"desc", ""}, {"key", 1}, {"type", "number"}, {"art", "local"}},
    // {{"name", "reflex edge chain"}, {"desc", ""}, {"key", 2}, {"type", "number"}, {"art", "global"}},
    // {{"name", "konvex edge chain"}, {"desc", ""}, {"key", 3}, {"type", "number"}, {"art", "global"}},
    // {{"name", "lights to illuminate"}, {"desc", ""}, {"key", 4}, {"type", "number"}, {"art", "global"}}
  };

  return obj;
}

pl::PointList pl::filter(const pl::PointList &point_list, const pl::FilterList &global_filter) {
  return point_list;
}

