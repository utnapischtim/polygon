#ifndef ANGLERANGE_H_
#define ANGLERANGE_H_

#include <string>

#include "utilities.h"

namespace pl {

struct AngleRange {
  int lower_bound;
  int upper_bound;

  AngleRange();
  AngleRange(std::string str);

  double lower_bound_radian() const;
  double upper_bound_radian() const;
};

AngleRange operator-(const double lhs, const AngleRange &rhs);

}

#endif
