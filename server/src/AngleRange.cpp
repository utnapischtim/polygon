#include <string>

#include "AngleRange.h"
#include "utilities.h"

pl::AngleRange::AngleRange() : lower_bound{}, upper_bound{} {}

pl::AngleRange::AngleRange(std::string str) : lower_bound{}, upper_bound{} {
  auto vec = pl::split(str, '.');
  lower_bound = std::stoi(*(vec.begin()));
  upper_bound = std::stoi(*(vec.end() - 1));
}

double pl::AngleRange::lower_bound_radian() const {
  return lower_bound * (M_PI / 180);
}

double pl::AngleRange::upper_bound_radian() const {
  return upper_bound * (M_PI / 180);
}

pl::AngleRange pl::operator-(const double lhs, const pl::AngleRange &rhs) {
  pl::AngleRange result;
  result.lower_bound = lhs - rhs.lower_bound;
  result.upper_bound = lhs - rhs.upper_bound;
  return result;
}
