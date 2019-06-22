#ifndef BOUNCINGVERTICES_H_
#define BOUNCINGVERTICES_H_

#include <docopt.h>

#include "Point.h"

namespace pl {

PointList bouncingVertices(const PointList &point_list, const std::map<std::string, docopt::value> &args);

}

#endif
