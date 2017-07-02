#include "json.hpp"

#include "Generator.h"

nlohmann::json pl::getListOfGenerators() {
  nlohmann::json obj = {
    {{"name", "random two peasants"}, {"desc", ""}, {"key", 0}},
    {{"name", "steady growth"}, {"desc", ""}, {"key", 1}},
    {{"name", "two opt"}, {"desc", ""}, {"key", 2}},
    {{"name", "convex layers"}, {"desc", ""}, {"key", 3}},
    {{"name", "convex bottom"}, {"desc", ""}, {"key", 4}},
    {{"name", "space partitioning"}, {"desc", ""}, {"key", 5}},
    {{"name", "permute and reject"}, {"desc", ""}, {"key", 6}}
  };

  return obj;
}
