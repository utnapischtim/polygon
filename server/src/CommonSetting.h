#ifndef COMMONSETTINGS_H_
#define COMMONSETTINGS_H_

#include <optional>
#include <string>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "json.hpp"

#include "Point.h"
#include "utilities.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

namespace pl {

// TODO
// maybe it is possible to template the type of val and make it
// depending of the type of the commonSetting writen in
// getCommonSetings
struct CommonSetting {
  std::string name;
  std::string desc;
  int key;
  std::string type;
  std::string val;

  CommonSetting()
    : name{}, desc{}, key{}, type{}, val{}
    {}

  CommonSetting(std::string n, std::string d, int k, std::string t, std::string v)
    : name(n), desc(d), key(k), type(t), val(v)
    {}

  CommonSetting(nlohmann::json obj)
    : CommonSetting(obj["name"], obj["desc"], obj["key"], obj["type"], obj["val"])
    {}
};


struct SamplingGrid {
  int width;
  int height;

  SamplingGrid() : width(0), height(0) {}

  SamplingGrid(CommonSetting common_setting) : SamplingGrid() {
    std::string sampling_grid = common_setting.val;

    auto t = pl::split(sampling_grid, 'x');
    width = std::stoi(t[0]);
    height = std::stoi(t[1]);
  }

  bool isOutOfArea(const cgal::Point_2 p) {
    return p.x() < 0 || width < p.x() || p.y() < 0 || height < p.y();
  }
};


using CommonSettingList = std::vector<CommonSetting>;

nlohmann::json getCommonSettings();

CommonSettingList createCommonSettingList(nlohmann::json common_settings);

std::optional<CommonSetting> find(const CommonSettingList &common_settings, std::string name);

}

#endif
