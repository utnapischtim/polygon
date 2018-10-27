#ifndef POINT_H_
#define POINT_H_

#include <iostream>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "json.hpp"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

namespace pl {

using PointList = std::vector<cgal::Point_2>;
nlohmann::json to_json(PointList point_list);



void convert(const std::vector<cgal::Segment_2> &segments, PointList &point_list);
void convert(const PointList &point_list, std::vector<cgal::Segment_2> &segments);

std::string to_string(const cgal::Point_2 &p);

}

std::ostream &operator<<(std::ostream &out, const pl::PointList &point_list);

#endif
