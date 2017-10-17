#include <iostream>
#include <algorithm>
#include <string>
#include <optional>
#include <map>
#include <string>
#include <iomanip>

#include "json.hpp"
#include "docopt.h"

#include "Filter.h"
#include "Point.h"
#include "utilities.h"

nlohmann::json pl::getListOfFilters() {
  nlohmann::json obj = {
    {{"arg", "--reflex-points"}, {"name", "reflex points"}, {"desc", ""}, {"key", 0}, {"type", "number"}},
    {{"arg", "--convex-points"}, {"name", "convex points"}, {"desc", ""}, {"key", 1}, {"type", "number"}},
    {{"arg", "--reflex-chain"}, {"name", "reflex chain"}, {"desc", ""}, {"key", 2}, {"type", "number"}},
    {{"arg", "--convex-chain"}, {"name", "convex chain"}, {"desc", ""}, {"key", 3}, {"type", "number"}},
    {{"arg", "--lights-to-illuminate"}, {"name", "lights to illuminate"}, {"desc", ""}, {"key", 4}, {"type", "number"}},
    {{"arg", "--reflex-angle-range"}, {"name", "reflex angle range"}, {"desc", ""}, {"key", 5}, {"type", "text"}},
    {{"arg", "--convex-angle-range"}, {"name", "convex angle range"}, {"desc", ""}, {"key", 6}, {"type", "text"}},
    {{"arg", "--reflex-chain-max"}, {"name", "reflex chain max"}, {"desc", ""}, {"key", 7}, {"type", "number"}}
  };

  return obj;
}

void pl::printFilters() {
  nlohmann::json filters = pl::getListOfFilters();

  size_t max_length_name = 0, max_length_arg = 0;

  for (auto filter : filters) {
    if (size_t length = filter["name"].get<std::string>().length(); max_length_name < length)
      max_length_name = length;

    if (size_t length = filter["arg"].get<std::string>().length(); max_length_arg < length)
      max_length_arg = length;
  }

  size_t fill_name = max_length_name + 4, fill_arg = max_length_arg + 4;

  std::cout << "list of filters\n";
  std::cout << std::setw(fill_name) << "name"
            << std::setw(fill_arg) << "arg"
            << std::setw(5) << "key"
            << std::setw(8) << "type"
            << std::setw(5) << "desc" << "\n";


  for (auto filter : filters)
    std::cout << std::setw(fill_name) << filter["name"].get<std::string>()
              << std::setw(fill_arg) << filter["arg"].get<std::string>()
              << std::setw(5) << filter["key"].get<int>()
              << std::setw(8) << filter["type"].get<std::string>()
              << std::setw(5) << filter["desc"].get<std::string>() << "\n";
}

pl::FilterList pl::createFilterList(nlohmann::json activated_filters) {
  pl::FilterList filters;

  for (auto obj : activated_filters)
    filters.push_back({obj});

  return filters;
}

pl::FilterList pl::createFilterList(std::map<std::string, docopt::value> args) {
  pl::FilterList filters;
  pl::FilterList list = pl::createFilterList(pl::getListOfFilters());

  for (auto const& arg : args)
    if (auto it = pl::find(list, arg.first)) {
      if (it->type == "text") {
        // it should be .. as delimiter, but split is only implemented
        // with char, and this is a simple
        auto vec = pl::split(arg.second.asString(), '.');
        it->lower_bound = std::stoi(*(vec.begin()));
        it->upper_bound = std::stoi(*(vec.end() - 1));
      }

      else
        it->val = arg.second.asLong();

      filters.push_back(*it);
    }

  return filters;
}

std::optional<pl::Filter> pl::find(const pl::FilterList &filters, std::string name) {
  auto it = std::find_if(filters.begin(), filters.end(), [&](auto &f) { return f.name == name || f.arg == name; });

  if (it == filters.end())
    return {};

  return *it;
}
