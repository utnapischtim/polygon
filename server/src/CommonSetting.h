#ifndef COMMONSETTINGS_H_
#define COMMONSETTINGS_H_

#include <optional>
#include <string>
#include <vector>
#include <map>
#include <tuple>

#include "json.hpp"
#include "docopt.h"

#include "Point.h"
#include "utilities.h"

namespace pl {

// TODO
// maybe it is possible to template the type of val and make it
// depending of the type of the commonSetting writen in
// getCommonSetings
struct CommonSetting {
  std::string arg;
  std::string name;
  std::string desc;
  int key;
  std::string type;
  std::string val;

  CommonSetting()
    : arg{}, name{}, desc{}, key{}, type{}, val{}
    {}

  CommonSetting(std::string n, std::string v)
    : arg{}, name(n), desc{}, key{}, type{}, val(v)
    {}

  CommonSetting(std::string n, std::string d, int k, std::string t, std::string v)
    : arg(""), name(n), desc(d), key(k), type(t), val(v)
    {}

  CommonSetting(std::string a, std::string n, std::string d, int k, std::string t, std::string v)
    : arg(a), name(n), desc(d), key(k), type(t), val(v)
    {}

  CommonSetting(nlohmann::json obj)
    : CommonSetting(obj["arg"], obj["name"], obj["desc"], obj["key"], obj["type"], obj["val"])
    {}
};


using CommonSettingList = std::vector<CommonSetting>;

nlohmann::json getCommonSettings();
void printCommonSettings();

CommonSettingList createCommonSettingList(nlohmann::json common_settings);
CommonSettingList createCommonSettingList(const std::map<std::string, docopt::value> &args);

std::optional<CommonSetting> find(const CommonSettingList &common_settings, std::string name);

void setValue(CommonSettingList &common_settings, const CommonSetting &common_setting);
void setValue(CommonSettingList &common_settings, const std::string name, const std::string val);

}

#endif
