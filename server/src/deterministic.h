#ifndef DETERMINISTIC_H_
#define DETERMINISTIC_H_

#include "CommonSetting.h"
#include "Filter.h"
#include "Point.h"

namespace pl::det {

PointList deterministic(CommonSettingList common_settings, FilterList /*local_filters*/);

}

#endif
