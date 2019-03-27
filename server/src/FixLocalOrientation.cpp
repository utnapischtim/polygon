#include <vector>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <iomanip>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "FixLocalOrientation.h"
#include "RegularPolygon.h"
#include "Point.h"
#include "CommonSetting.h"
#include "Filter.h"
#include "random.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

static pl::PointList createRegularPolygon(pl::RegularPolygonSettings &rps);
static void enrichWithReflexPoints(pl::PointList &final_list, const pl::RegularPolygonSettings &rps, const pl::FixLocalOrientationSettings &flos);
static void stretch(pl::PointList &final_list, const pl::RegularPolygonSettings &rps, const pl::FixLocalOrientationSettings &flos);

pl::PointList pl::fixLocalOrientation(pl::CommonSettingList &common_settings, const pl::FilterList &filters) {
  pl::RegularPolygonSettings rps(common_settings);
  pl::FixLocalOrientationSettings flos(common_settings, filters);

  // the regular polygon node count is the whole node count without
  // the reflex node counts. this is a design decision
  rps.updateNodeCount(rps.node_count - flos.reflex_point_count);

  pl::PointList final_list = createRegularPolygon(rps);

  enrichWithReflexPoints(final_list, rps, flos);

  if (0 < flos.convex_stretch)
    stretch(final_list, rps, flos);

  // this is necessary to close the polygon
  final_list.push_back(final_list[0]);

  return final_list;
}

pl::PointList createRegularPolygon(pl::RegularPolygonSettings &rps) {
  if (rps.node_count < 4)
    throw std::runtime_error("the final node_counts after substracting the reflex points,\
                              should not be below 4 points");

  pl::PointList point_list = pl::regularPolygon(rps);

  // the last element is the same as the first element, this is
  // because of closing the polygon!
  point_list.pop_back();

  return point_list;
}

std::vector<cgal::Point_2> createReflexPointsOnInnerArc(const pl::RegularPolygonSettings &rps, const pl::FixLocalOrientationSettings &flos, const size_t segment_number, const cgal::Point_2 &start_point_of_reflex_circle) {
  std::vector<cgal::Point_2> reflex_points;
  int reflex_counts_for_this_run = flos.reflex_point_count_per_segment[segment_number];

  double rotation_angle_of_random_segment = rps.gamma * segment_number;

  CGAL::Aff_transformation_2<cgal> rotate_clockwise(CGAL::ROTATION, std::sin(-rotation_angle_of_random_segment), std::cos(-rotation_angle_of_random_segment));

  cgal::Vector_2 distance_to_random_center_of_reflex_circle = rotate_clockwise(flos.distance_to_center_of_reflex_circle);
  cgal::Point_2 center_of_reflex_circle = rps.center + distance_to_random_center_of_reflex_circle;

  cgal::Vector_2 vec_from_center_to_point_on_reflex_circle = start_point_of_reflex_circle - center_of_reflex_circle;

  // it has to be one more, because there are k points, but k+1
  // segments to add, therefore k + 1 angles
  double angle_between_reflex_points = flos.iota / (reflex_counts_for_this_run + 1);

  while (0 < reflex_counts_for_this_run) {
    // the angle could be calculated randomly. there is only the
    // problem to calculate the angle randomly in such a way that
    // reflex_counts_for_this_run + 1 angles could be calculated.
    // the sum off all angles (mu) has to be less than iota

    CGAL::Aff_transformation_2<cgal> rotate_clockwise_l(CGAL::ROTATION, std::sin(-angle_between_reflex_points), std::cos(-angle_between_reflex_points));
    vec_from_center_to_point_on_reflex_circle = rotate_clockwise_l(vec_from_center_to_point_on_reflex_circle);
    cgal::Point_2 next_reflex_point = center_of_reflex_circle + vec_from_center_to_point_on_reflex_circle;

    reflex_points.push_back(next_reflex_point);

    reflex_counts_for_this_run -= 1;
  }

  return reflex_points;
}

void enrichWithReflexPoints(pl::PointList &final_list, const pl::RegularPolygonSettings &rps, const pl::FixLocalOrientationSettings &flos) {
  for (int segment_number = rps.node_count - 1; 0 <= segment_number; --segment_number) {
    int reflex_counts_for_this_run = flos.reflex_point_count_per_segment[segment_number];

    if (0 < reflex_counts_for_this_run) {
      auto insert_it = final_list.begin() + segment_number;
      cgal::Point_2 start_point_of_reflex_circle = *insert_it;

      std::vector<cgal::Point_2> reflex_points = createReflexPointsOnInnerArc(rps, flos, segment_number, start_point_of_reflex_circle);

      for (const auto &reflex_point : reflex_points)
        insert_it = final_list.insert(insert_it, reflex_point);
    }
  }
}

void stretch(pl::PointList &final_list, const pl::RegularPolygonSettings &rps, const pl::FixLocalOrientationSettings &flos) {
  // iterate over the 0 reflex nodes per segment to stretch those
  // points. the call of the function right_turn on every point
  // should not be necessary with this approach

  // the convex counter holds for the target of the current segment!
  size_t
    convex_counter = 0,
    pos = 0;

  double kappa = M_PI - (rps.gamma / 2);

  CGAL::Aff_transformation_2<cgal> rotate_clockwise_m(CGAL::ROTATION, std::sin(-kappa), std::cos(-kappa));

  // it is also possible to do this in the previous loop, but it
  // would be more difficult to distinguish between the different
  // deformations.
  for (size_t segment_number = 0, size = flos.reflex_point_count_per_segment.size(); segment_number < size; ++segment_number) {
    // get number on reflex points to put in this segment
    int reflex_counts_for_this_segment = flos.reflex_point_count_per_segment[segment_number];

    if (reflex_counts_for_this_segment == 0)
      convex_counter += 1;

    if (0 < reflex_counts_for_this_segment || pos == final_list.size()) {

      if (1 < convex_counter) {

        size_t moving_pos = pos - convex_counter;

        // the pos_point_before is the last point of the final_list
        // if moving point is 0
        size_t pos_point_before = moving_pos == 0 ? final_list.size() - 1 : moving_pos - 1;

        cgal::Point_2 P_b = final_list[pos_point_before];

        // the convex point before a reflex series does also has not
        // to be moved! to ensure this the (pos - 1) is used.
        for (; moving_pos < (pos-1); moving_pos++) {

          cgal::Point_2 P_c = final_list[moving_pos];

          cgal::Vector_2 m = P_b - P_c;

          m = rotate_clockwise_m(m);

          // necessary for the next loop entry, because then the
          // Point within the final_list has yet moved.
          P_b = P_c;

          P_c = P_c + m;

          final_list[moving_pos] = P_c;

          pos_point_before = pos_point_before == (final_list.size() - 1) ? 0 : pos_point_before + 1;
        }
      }

      convex_counter = 0;
    }

    // the + 1 is the current convex node
    pos += reflex_counts_for_this_segment + 1;
  }
}

int getReflexPointCount(const pl::FilterList &filters) {
  int reflex_point_count = 0;

  if (auto t = pl::find(filters, "reflex points"))
    reflex_point_count = (*t).val;

  return reflex_point_count;
}

int getReflexChainMaxCount(const pl::FilterList &filters) {
  int reflex_chain_max_count = getReflexPointCount(filters);

  if (auto t = pl::find(filters, "reflex chain max"); 0 <= (*t).val)
    reflex_chain_max_count = (*t).val;

  return reflex_chain_max_count;
}

double getConvexStretch(const pl::FilterList &filters) {
  int convex_stretch;

  if (auto t = pl::find(filters, "convex stretch"))
    convex_stretch = (*t).val;

  return convex_stretch;
}

std::vector<int> initReflexPointsPerSegment(const int node_counts, const int reflex_counts, const int reflex_chain_max) {
  std::vector<int> reflex_points_per_segment(node_counts, 0);

  for (int i = 0; i < reflex_counts; ++i) {
    unsigned pos;
    do {
      pos = pl::randomValueOfRange(0, node_counts-1);
    } while (reflex_chain_max < reflex_points_per_segment[pos] + 1);

    reflex_points_per_segment[pos] += 1;
  }

  return reflex_points_per_segment;
}

pl::FixLocalOrientationSettings::FixLocalOrientationSettings(const CommonSettingList &common_settings, const FilterList &filters)
  : FixLocalOrientationSettings{}
{
  pl::RegularPolygonSettings rps(common_settings);

  reflex_point_count = getReflexPointCount(filters);
  reflex_chain_max_count = getReflexChainMaxCount(filters);
  reflex_point_count_per_segment = initReflexPointsPerSegment(rps.node_count - reflex_point_count, reflex_point_count, reflex_chain_max_count);
  convex_stretch = getConvexStretch(filters);
  iota = (2*M_PI) - M_PI - rps.gamma;

  // calculate distance from center of the regular polygon center to
  // point K. the point for the circle center on which resides the new
  // reflex points
  distance_to_center_of_reflex_circle = {rps.radius / std::cos(rps.gamma / 2.0), 0.0};


  // if reflex_max is set to the default value, then every number is
  // possible. reflex_counts would also be possible it, should be a
  // number greater then reflex_counts and node_counts is a number
  // that fullfills those needs.
  if (reflex_chain_max_count == 0)
    reflex_chain_max_count = rps.node_count;
}

std::ostream &pl::operator<<(std::ostream &out, const pl::FixLocalOrientationSettings &flos) {
  std::cout << "flos.reflex_point_count: " << std::setw(10) << flos.reflex_point_count << '\n'
            << "flos.reflex_chain_max_count: " << std::setw(10) << flos.reflex_chain_max_count << '\n'
            << "flos.reflex_point_count_per_segment: " << std::setw(10);

  for (auto k : flos.reflex_point_count_per_segment)
    std::cout << k << '\n';

  std::cout << "flos.convex_stretch: " << std::setw(10) << flos.convex_stretch << '\n'
            << "flos.iota: " << std::setw(10) << flos.iota << '\n'
            << "flos.distance_to_center_of_reflex_circle: " << std::setw(10) << flos.distance_to_center_of_reflex_circle << '\n';
  return out;
}
