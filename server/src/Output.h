#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <string>

#include "Point.h"
#include "CommonSetting.h"

namespace pl {

void output(PointList point_list, std::string format, std::string filename, SamplingGrid sampling_grid = {}, int phase = -1);
void printOutputFormats();

}

#endif
