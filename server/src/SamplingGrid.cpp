#include <tuple>
#include <vector>
#include <string>
#include <cmath>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <boost/optional/optional_io.hpp>

#include "CommonSetting.h"
#include "SamplingGrid.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

pl::SamplingGrid::SamplingGrid() : width(0), height(0) {}

pl::SamplingGrid::SamplingGrid(int w, int h) : width(w), height(h) {}

pl::SamplingGrid::SamplingGrid(CommonSetting common_setting) : SamplingGrid() {
  std::string sampling_grid = common_setting.val;

  auto t = pl::split(sampling_grid, 'x');
  width = std::stoi(t[0]);
  height = std::stoi(t[1]);
}

pl::SamplingGrid &pl::SamplingGrid::operator=(const SamplingGrid &cs) {
  width = cs.width;
  height = cs.height;
  return *this;
}

bool pl::SamplingGrid::isOutOfArea(const cgal::Point_2 p) const {
  return p.x() < 0 || width < p.x() || p.y() < 0 || height < p.y();
}

bool pl::SamplingGrid::isInsideOfArea(const cgal::Point_2 p) const {
  return (0 - 1.0e-10) <= p.x() && p.x() <= (width + 1.0e-10) && (0 - 1.0e-10) <= p.y() && p.y() <= (height + 1.0e-10);
}

std::tuple<cgal::Point_2, cgal::Point_2> pl::SamplingGrid::intersection(const cgal::Line_2 &line) const {
  std::vector<cgal::Segment_2> border = {
    {{width, height}, {0, height}},
    {{width, 0}, {width, height}},
    {{0, 0}, {width, 0}},
    {{0, height}, {0, 0}}
  };

  std::vector<cgal::Point_2> intersections;

  for (auto segment : border)
    if (const auto point = CGAL::intersection(cgal::Line_2{segment}, line); point && isInsideOfArea(boost::get<cgal::Point_2>(*point)))
      intersections.push_back(boost::get<cgal::Point_2>(*point));

  if (intersections.size() < 2 || 2 < intersections.size()) {
    std::cout << "intersection with border does have more then 2 or less then 2 intersections" << "\n";
    std::exit(-1);
  }

  return {intersections[0], intersections[1]};
}

cgal::Point_2 pl::SamplingGrid::center() const {
  cgal::Point_2 p(0, 0), q(width, 0), r(0, height);
  const auto circle = cgal::Circle_2(p, q, r);
  return circle.center();
}

std::vector<cgal::Point_2> pl::SamplingGrid::points() const {
  return {{0,0},{0,height},{width,height},{width,0}};
}
