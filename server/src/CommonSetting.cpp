#include <optional>
#include <vector>
#include <map>
#include <iomanip>

#include "json.hpp"
#include "docopt.h"

#include "CommonSetting.h"

nlohmann::json pl::getCommonSettings() {
  nlohmann::json obj = {
    {{"arg", "--nodes"}, {"name", "nodes"}, {"desc", ""}, {"key", 0}, {"type", "number"}, {"val", "100"}},
    {{"arg", "--sampling-grid"}, {"name", "sampling grid"}, {"desc", ""}, {"key", 1}, {"type", "text"}, {"val", "1500x800"}},
    {{"arg", "--phases"}, {"name", "phases"}, {"desc", ""}, {"key", 2}, {"type", "number"}, {"val", "10"}},
    {{"arg", "--radius"}, {"name", "radius"}, {"desc", ""}, {"key", 3}, {"type", "number"}, {"val", "60"}},
    {{"arg", "--segment-length"}, {"name", "segment length"}, {"desc", ""}, {"key", 4}, {"type", "number"}, {"val", "20"}},
    {{"arg", "--bouncing-radius"}, {"name", "bouncing radius"}, {"desc", ""}, {"key", 5}, {"type", "number"}, {"val", "60"}}// ,
    // {{"arg", "--center"}, {"name", "center"}, {"desc", ""}, {"key", 5}, {"type", "text"}, {"val", "-1,-1"}}
  };

  return obj;
}

void pl::printCommonSettings() {
  nlohmann::json common_settings = pl::getCommonSettings();

  size_t max_length_name = 0, max_length_arg = 0;

  for (auto common_setting : common_settings) {
    if (size_t length = common_setting["name"].get<std::string>().length(); max_length_name < length)
      max_length_name = length;

    if (size_t length = common_setting["arg"].get<std::string>().length(); max_length_arg < length)
      max_length_arg = length;
  }

  size_t fill_name = max_length_name + 4, fill_arg = max_length_arg + 4;

  std::cout << "list common settings\n";
  std::cout << std::setw(fill_name) << "name"
            << std::setw(fill_arg) << "arg"
            << std::setw(5) <<  "key"
            << std::setw(8) << "type"
            << std::setw(14) << "defaultVal"
            << std::setw(5) << "desc" << "\n";

  for (auto common_setting : common_settings)
    std::cout << std::setw(fill_name) << common_setting["name"].get<std::string>()
              << std::setw(fill_arg) << common_setting["arg"].get<std::string>()
              << std::setw(5) << common_setting["key"].get<int>()
              << std::setw(8) << common_setting["type"].get<std::string>()
              << std::setw(14) << common_setting["val"].get<std::string>()
              << std::setw(5) << common_setting["desc"].get<std::string>() << "\n";
}

pl::CommonSettingList pl::createCommonSettingList(nlohmann::json common_settings) {
  pl::CommonSettingList common_setting_list;

  for (auto obj : common_settings)
    common_setting_list.push_back({obj});

  return common_setting_list;
}

pl::CommonSettingList pl::createCommonSettingList(const std::map<std::string, docopt::value> &args) {
  pl::CommonSettingList common_setting_list;
  pl::CommonSettingList list = pl::createCommonSettingList(pl::getCommonSettings());

  for (auto const& arg : args)
    if (auto it = pl::find(list, arg.first)) {
      it->val = arg.second.asString();
      common_setting_list.push_back(*it);
    }

  return common_setting_list;
}

std::optional<pl::CommonSetting> pl::find(const pl::CommonSettingList &common_settings, std::string name) {
  auto it = std::find_if(common_settings.begin(), common_settings.end(), [&](auto c) { return c.name == name || c.arg == name; });

  if (it == common_settings.end()) {
    // std::string msg = "name '" + name + "' could not be found in common_settings";
    // throw std::runtime_error(msg);
    return {};
  }

  return *it;
}
