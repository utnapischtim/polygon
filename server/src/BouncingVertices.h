#ifndef BOUNCINGVERTICES_H_
#define BOUNCINGVERTICES_H_

#include "Point.h"
#include "CommonSetting.h"
#include "Filter.h"

namespace pl {

PointList bouncingVertices(const PointList &point_list, const CommonSettingList &common_settings, const FilterList &filter);

}

#endif
