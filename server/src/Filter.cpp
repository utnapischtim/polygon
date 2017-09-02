#include <algorithm>
#include <string>
#include <optional>

#include "json.hpp"

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
    {{"name", "convex points"}, {"desc", ""}, {"key", 1}, {"type", "number"}},
    {{"name", "reflex edge chain"}, {"desc", ""}, {"key", 2}, {"type", "number"}},
    {{"name", "convex edge chain"}, {"desc", ""}, {"key", 3}, {"type", "number"}},
    {{"name", "lights to illuminate"}, {"desc", ""}, {"key", 4}, {"type", "number"}}
  };

  return obj;
}

std::optional<pl::Filter> pl::find(const pl::FilterList &filters, std::string name) {
  auto it = std::find_if(filters.begin(), filters.end(), [&](auto &f) { return f.name == name; });

  if (it == filters.end())
    return {};

  return *it;
}
