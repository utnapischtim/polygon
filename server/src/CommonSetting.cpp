#include <optional>
#include <vector>

#include "json.hpp"

#include "CommonSetting.h"

nlohmann::json pl::getCommonSettings() {
  nlohmann::json obj = {
    {{"name", "nodes"}, {"desc", ""}, {"key", 0}, {"type", "number"}, {"val", "100"}},
    {{"name", "sampling grid"}, {"desc", ""}, {"key", 1}, {"type", "text"}, {"val", "1500x800"}},
    {{"name", "phases"}, {"desc", ""}, {"key", 2}, {"type", "number"}, {"val", "10"}},
    {{"name", "radius"}, {"desc", ""}, {"key", 3}, {"type", "number"}, {"val", "60"}},
    {{"name", "segment length"}, {"desc", ""}, {"key", 4}, {"type", "number"}, {"val", "20"}}
  };

  return obj;
}

pl::CommonSettingList pl::createCommonSettingList(nlohmann::json common_settings) {
  pl::CommonSettingList common_setting_list;

  for (auto obj : common_settings)
    common_setting_list.push_back(CommonSetting(obj));

  return common_setting_list;
}

std::optional<pl::CommonSetting> pl::find(const pl::CommonSettingList &common_settings, std::string name) {
  auto it = std::find_if(common_settings.begin(), common_settings.end(), [&](auto c) { return c.name == name; });

  if (it == common_settings.end()) {
    // std::string msg = "name '" + name + "' could not be found in common_settings";
    // throw std::runtime_error(msg);
    return {};
  }

  return *it;
}
