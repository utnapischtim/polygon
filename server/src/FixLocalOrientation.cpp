#include <vector>
#include <cstdlib>
#include <cmath>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "FixLocalOrientation.h"
#include "RegularPolygon.h"
#include "Point.h"
#include "CommonSetting.h"
#include "Filter.h"
#include "random.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

static std::tuple<double> init(const pl::CommonSettingList &common_settings);
pl::CommonSettingList buildRegularPolygonSettings(const pl::CommonSettingList &common_settings, const pl::Filter &reflex_points);

pl::PointList pl::fixLocalOrientation(const pl::CommonSettingList &common_settings, const pl::FilterList &filters) {
  pl::PointList final_list;

  auto [radius] = init(common_settings);

  auto reflex_points = pl::find(filters, "reflex points");

  if (!reflex_points)
    throw std::runtime_error("no reflex points set");

  pl::CommonSettingList regular_polygon_settings = buildRegularPolygonSettings(common_settings, *reflex_points);

  // TODO: this should be possible to regulate with a parameter
  final_list = pl::regularPolygon(regular_polygon_settings);

  // the last element is the same as the first element, this is
  // because of closing the polygon!
  final_list.pop_back();

  pl::PointList point_list = final_list;

  int node_counts = 0;
  if (auto t = pl::find(regular_polygon_settings, "nodes"))
    node_counts = std::stoi((*t).val);

  int reflex_counts = (*reflex_points).val;

  // the segment angle
  double gamma = 2 * M_PI * 1/node_counts;

  // calculate distance from center of the regular polygon center to
  // point K. the point for the circle center on which resides the new
  // reflex points
  cgal::Vector_2 d(radius / std::cos(gamma / 2.0), 0.0);

  // to calculate the point K the center is necessary
  pl::Point center(-1, -1);
  if (auto cs_center = pl::find(regular_polygon_settings, "center"); cs_center) {
    auto vec = pl::split((*cs_center).val, ',');
    if (vec.size() == 2) {
      center.x = std::stod(vec[0]);
      center.y = std::stod(vec[1]);
    }
  } else {
    // the center has to be calculated and this is not implemented for
    // the moment
    throw std::runtime_error("center has no meaningfull value");
  }

  // to do this, the given regular polygon has to have a vertical
  // segment on between first and last point.
  cgal::Point_2 K(center.x + d.x(), center.y + d.y());

  std::map<int, bool> segments_used;
  for (int i = 0; i < node_counts; ++i)
    segments_used[i] = false;

  // while there are reflex points to init
  while (0 < reflex_counts) {
    // the random segment, where the reflex points lies in
    int random_segment;
    do {
      random_segment = pl::randomValueOfRange(0, node_counts-1);
    } while (segments_used[random_segment]);

    segments_used[random_segment] = true;

    // the rotation angle to rotate the vector from C to K = d
    double omega = gamma * random_segment;

    // this rotation has to be clockwise
    CGAL::Aff_transformation_2<cgal> rotate_clockwise(CGAL::ROTATION, std::sin(-omega), std::cos(-omega));

    // the random point L around which the reflex points are rotated,
    // e is the vector from center to L
    cgal::Vector_2 e = rotate_clockwise(d);
    cgal::Point_2 L(center.x + e.x(), center.y + e.y());

    // the point P is the source of the segment where L is between
    cgal::Point_2 P(point_list[random_segment].x, point_list[random_segment].y);

    // this is necessary, because if the new segment is after one,
    // that is just processed, then the offset with final_list.begin()
    // + random_segment is wrong
    auto insert_it = std::find_if(final_list.begin(), final_list.end(), [&](auto p) { return p == P; });

    // the vector l is the vector from L to the random point P
    cgal::Vector_2 l = P - L;

    // choose number on reflex points to put in this segment
    int reflex_counts_for_this_run = pl::randomValueOfRange(1, reflex_counts);

    reflex_counts -= reflex_counts_for_this_run;

    // the angle where new points could be added
    double iota = gamma;

    // while number of reflex points
    while (0 < reflex_counts_for_this_run) {
      // choose a random angle between 0 and iota
      double mu = pl::randomValueOfRange(0.1, iota);

      // rotate clockwise around point L
      CGAL::Aff_transformation_2<cgal> rotate_clockwise_l(CGAL::ROTATION, std::sin(-mu), std::cos(-mu));
      l = rotate_clockwise_l(l);

      // calculate next reflex point position
      cgal::Point_2 P_r = L + l;

      // add point P_r to final_list between P and P+1
      insert_it = final_list.insert(insert_it, {P_r.x(), P_r.y()});

      reflex_counts_for_this_run -= 1;
      iota -= mu;
    }
  }

  final_list.push_back(final_list[0]);

  return final_list;
}

std::tuple<double> init(const pl::CommonSettingList &common_settings) {
  pl::CommonSetting c_s_radius;

  if (auto t = pl::find(common_settings, "radius"))
    c_s_radius = *t;
  else
    c_s_radius = pl::CommonSetting("radius", "", 3, "number", "60");

  double radius = std::stod(c_s_radius.val);

  return {radius};
}

pl::CommonSettingList buildRegularPolygonSettings(const pl::CommonSettingList &common_settings, const pl::Filter &reflex_points) {
  pl::CommonSetting c_s_sampling_grid, c_s_nodes, c_s_radius, c_s_segment_length;
  pl::CommonSettingList regular_polygon_settings;

  if (auto t = pl::find(common_settings, "sampling grid"))
    regular_polygon_settings.push_back(*t);

  if (auto t = pl::find(common_settings, "nodes")) {
    int node_counts = std::stoi((*t).val) - reflex_points.val;

    if (node_counts < 4)
      throw std::runtime_error("the final node_counts after substracting the reflex points, should not be below 4 points");

    pl::CommonSetting nodes("nodes", std::to_string(node_counts));
    regular_polygon_settings.push_back(nodes);
  }

  if (auto t = pl::find(common_settings, "radius"))
    regular_polygon_settings.push_back(*t);

  if (auto t = pl::find(common_settings, "segment length"))
    regular_polygon_settings.push_back(*t);

  return regular_polygon_settings;
}
