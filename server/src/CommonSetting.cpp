#include "json.hpp"

#include "CommonSetting.h"

nlohmann::json pl::getCommonSettings() {
  nlohmann::json obj = {
    {{"name", "nodes"}, {"desc", ""}, {"key", 0}, {"type", "number"}, {"val", "100"}},
    {{"name", "sampling grid"}, {"desc", ""}, {"key", 1}, {"type", "text"}, {"val", "1500x800"}}
  };

  return obj;
}

pl::CommonSettingList pl::createCommonSettingList(nlohmann::json common_settings) {
  pl::CommonSettingList common_setting_list;

  for (auto obj : common_settings)
    common_setting_list.push_back(CommonSetting(obj));

  return common_setting_list;
}

// TODO make it robust ;)
pl::CommonSetting pl::find(pl::CommonSettingList common_settings, std::string name) {
  return *std::find_if(common_settings.begin(), common_settings.end(), [&](auto c) { return c.name == name; });
}
