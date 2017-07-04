#ifndef FILTER_H_
#define FILTER_H_

#include "json.hpp"

namespace pl {

struct Filter {
  int key;
};

using FilterList = std::vector<Filter>;

FilterList createFilterList(nlohmann::json activatedFilterKeys);

nlohmann::json getListOfFilters();

}

#endif
