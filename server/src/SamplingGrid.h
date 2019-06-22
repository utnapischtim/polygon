#ifndef SAMPLINGGRID_H_
#define SAMPLINGGRID_H_

#include <tuple>
#include <vector>
#include <string>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <boost/optional/optional_io.hpp>

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

namespace pl {

struct SamplingGrid {
  int width;
  int height;

  SamplingGrid();
  SamplingGrid(int w, int h);
  SamplingGrid(const std::string sampling_grid);
  SamplingGrid &operator=(const SamplingGrid &cs);

  bool isOutOfArea(const cgal::Point_2 p) const;
  bool isInsideOfArea(const cgal::Point_2 p) const;
  std::tuple<cgal::Point_2, cgal::Point_2> intersection(const cgal::Line_2 &random_line) const;
  cgal::Point_2 center() const;
  std::vector<cgal::Point_2> points() const;
};


}

#endif
