#include "json.hpp"

#include "Filter.h"

pl::FilterList pl::createFilterList(nlohmann::json activatedFilterKeys) {
  return {};
}

nlohmann::json pl::getListOfFilters() {
  nlohmann::json obj = {
    {{"name", "reflex points"}, {"desc", ""}, {"key", 0}, {"type", "number"}},
    {{"name", "konvex points"}, {"desc", ""}, {"key", 1}, {"type", "number"}},
    {{"name", "reflex edge chain"}, {"desc", ""}, {"key", 2}, {"type", "number"}},
    {{"name", "konvex edge chain"}, {"desc", ""}, {"key", 3}, {"type", "number"}}
  };

  return obj;
}
