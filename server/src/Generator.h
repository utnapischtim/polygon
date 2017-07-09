#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <vector>
#include <string>

#include "json.hpp"

#include "Filter.h"
#include "Point.h"
#include "CommonSetting.h"

namespace pl {


struct Generator {
  std::string name;
  std::string desc;
  int key;

  Generator(std::string n, std::string d, int k)
    : name(n), desc(d), key(k)
    {}
  Generator(nlohmann::json obj)
    : Generator(obj["name"], obj["desc"], obj["key"])
    {}
};


nlohmann::json getListOfGenerators();
Generator createGenerator(nlohmann::json obj);

PointList generatePointList(Generator generator, CommonSettingList common_settings, FilterList local_filters);

PointList randomTwoPeasants(pl::CommonSettingList common_settings, FilterList local_filters);
PointList random(CommonSettingList common_settings, FilterList local_filters);
}

#endif
