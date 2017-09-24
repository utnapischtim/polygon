#ifndef FIXLOCALORIENTATION_H_
#define FIXLOCALORIENTATION_H_

#include "CommonSetting.h"
#include "Filter.h"

namespace pl {

PointList fixLocalOrientation(const CommonSettingList &common_settings, const FilterList &filters);

}

#endif
