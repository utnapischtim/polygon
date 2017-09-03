#include <tuple>
#include <limits>
#include <cstdlib>
#include <iterator>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "BouncingVertices.h"
#include "Point.h"
#include "CommonSetting.h"
#include "Filter.h"
#include "random.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

static std::vector<cgal::Segment_2>::iterator next(std::vector<cgal::Segment_2> &segments, const std::vector<cgal::Segment_2>::iterator &it);
static std::vector<cgal::Segment_2>::iterator prev(std::vector<cgal::Segment_2> &segments, const std::vector<cgal::Segment_2>::iterator &it);
static std::tuple<pl::SamplingGrid, unsigned, double> init(const pl::CommonSettingList &common_settings);
static std::vector<cgal::Segment_2> init(const pl::PointList &point_list);

pl::PointList pl::bouncingVertices(const pl::PointList &point_list, const pl::CommonSettingList &common_settings, const pl::FilterList &filters) {
  pl::PointList final_list;

  size_t N = point_list.size();

  std::vector<cgal::Segment_2> segments = init(point_list);
  auto [sampling_grid, phases, radius] = init(common_settings);

  std::vector<bool> convex_point_vec(N, false);
  std::vector<bool> reflex_point_vec(N, false);
  std::optional<pl::Filter> convex_filter, reflex_filter;

  // if convex: choose random which point should stay
  if (convex_filter = pl::find(filters, "convex points"))
    for (int i = 0; i < (*convex_filter).val; ++i)
      convex_point_vec[pl::randomValueOfRange(0, N)] = true;

  // if reflex: choose random which point should transform to reflex,
  // choose not the same, as above!
  if (reflex_filter = pl::find(filters, "reflex points"))
    for (int i = 0; i < (*reflex_filter).val; ++i) {
      int random_val = pl::randomValueOfRange(0, N);
      while (convex_point_vec[random_val])
        random_val = pl::randomValueOfRange(0, N);

      reflex_point_vec[random_val] = true;
    }

  // if there is a chain, then above number list should be overriden

  for (size_t phase = 0; phase < phases; ++phase) {
    // not use next or prev because this would cause a endles loop
    for (auto [sit, k] = std::tuple{segments.begin(), 0}; sit != segments.end(); ++sit, ++k) {
      bool
        outside_boundary_box = false,
        intersection_occur = false,

        // if the convex_filter has a value and the point number in
        // the convex_point_vec is set to true than the convex_flag is
        // set to true, therefore a point has to be found which
        // resolves the requirements and set the convex_flag to false
        // to break the do while loop
        convex_flag = false,
        reflex_flag = false;

      cgal::Point_2 shifted_point;

      std::vector<cgal::Segment_2>::iterator sitn = next(segments, sit);

      cgal::Segment_2 e_1, e_2, old_e_1 = *sit, old_e_2 = *sitn;

      do {
        intersection_occur = false;
        convex_flag = convex_filter.has_value() && convex_point_vec[k];
        reflex_flag = reflex_filter.has_value() && reflex_point_vec[k];

        // random set of new target of segment and after the do while
        // also set the new source of the next segment
        cgal::Point_2 velocity = {pl::randomValueOfRange(-radius, radius), pl::randomValueOfRange(-radius, radius)};

        // check if the new target of the segment is inside of the
        // boundary box, the new source of the next segment does not
        // have to be checked, because it is clear that if the target is
        // inside also the next source which is equal to target is inside.
        shifted_point = {sit->target().x() + velocity.x(), sit->target().y() + velocity.y()};
        outside_boundary_box = sampling_grid.isOutOfArea(shifted_point);

        // check only if the new point is inside, the following check
        // is more complex and hence more cpu consuming
        if (!outside_boundary_box) {

          e_1 = {sit->source(), shifted_point};
          e_2 = {shifted_point, sitn->target()};

          // *ssit != old_e_1 && *ssit != old_e_2, the new segments
          //   should not be checked about intersection with the old,
          //   because per def they should intersect
          // ssit->target() != e_1.source() && ssit->source() != e_2.target()
          //   the binding points between the segments does not count
          //   as intersections. and this construct should work in
          //   both directions, clockwise and anti-clockwise
          // but all elements which are not e_1 respectively e_2
          //   should be checked. because if not a intersection
          //   between e_1 respectively e_2 could be overlooked

          for (auto ssit = segments.begin(); ssit != segments.end() && !intersection_occur; ++ssit)
            if (*ssit != old_e_1 && *ssit != old_e_2) {
              if (ssit->target() != e_1.source())
                intersection_occur = intersection_occur || CGAL::do_intersect(*ssit, e_1);

              if (ssit->source() != e_2.target())
                intersection_occur = intersection_occur || CGAL::do_intersect(*ssit, e_2);
            }
        }


        // additional filters
        if (!outside_boundary_box && !intersection_occur) {
          if (convex_filter) {
            bool next_angle_still_convex = true;
            if (int l = (k+1 == static_cast<int>(N) ? 0 : k+1); convex_point_vec[l]) {
              auto sitnn = next(segments, sitn);
              next_angle_still_convex = CGAL::angle(shifted_point, sitn->target(), sitnn->target()) == CGAL::OBTUSE && CGAL::right_turn(shifted_point, sitn->target(), sitnn->target());
            }

            bool prev_angle_still_convex = true;
            if (size_t l = (k == 0 ? N : k); next_angle_still_convex && convex_point_vec[l-1]) {
              auto sitp = prev(segments, sit);
              prev_angle_still_convex = CGAL::angle(sitp->source(), sitp->target(), shifted_point) == CGAL::OBTUSE && CGAL::right_turn(sitp->source(), sitp->target(), shifted_point);
            }

            // if the current shifted point has to be convex, then it
            // only has to be checked, if the prev(point) is still
            // convex if it has to be!
            if (prev_angle_still_convex && next_angle_still_convex && convex_flag)
              convex_flag = CGAL::angle(e_1.source(), shifted_point, e_2.target()) != CGAL::OBTUSE || !CGAL::right_turn(e_1.source(), shifted_point, e_2.target());

            // if the prev(point) has to be convex and it is not more,
            // but the current point does not have to be convex, the
            // convex_flag would be false, but this is not good,
            // because the current shifted_point has to be reshifted,
            // therefore convex_flag has to be set to tue manually!
            if ((!prev_angle_still_convex || !next_angle_still_convex) && !convex_flag)
              convex_flag = true;
          }

          if (reflex_filter) {
            // not look in front if the phase is equal to 0 because it
            // could not be a reflex point. because the init algorithm
            // is the regular n-gon algorithm
            bool next_angle_still_reflex = true;
            if (int l = (k+1 == static_cast<int>(N) ? 0 : k+1); 0 < phase && reflex_point_vec[l]) {
              auto sitnn = next(segments, sitn);
              next_angle_still_reflex = CGAL::angle(shifted_point, sitn->target(), sitnn->target()) == CGAL::ACUTE && CGAL::left_turn(shifted_point, sitn->target(), sitnn->target());
            }

            bool prev_angle_still_reflex = true;
            if (size_t l = (k == 0 ? N : k); next_angle_still_reflex && reflex_point_vec[l-1]) {
              auto sitp = prev(segments, sit);
              prev_angle_still_reflex = CGAL::angle(sitp->source(), sitp->target(), shifted_point) == CGAL::ACUTE && CGAL::left_turn(sitp->source(), sitp->target(), shifted_point);
            }

            if (prev_angle_still_reflex && next_angle_still_reflex && reflex_flag)
              reflex_flag = CGAL::angle(e_1.source(), shifted_point, e_2.target()) != CGAL::ACUTE  || !CGAL::left_turn(e_1.source(), shifted_point, e_2.target());

            if ((!prev_angle_still_reflex || !next_angle_still_reflex) && !reflex_flag)
              reflex_flag = true;
          }
        }
      } while (outside_boundary_box || intersection_occur || convex_flag || reflex_flag);

      // set the new segments;
      *sit = e_1;
      *sitn = e_2;
    }
  }

  pl::convert(segments, final_list);
  return final_list;
}

std::vector<cgal::Segment_2>::iterator next(std::vector<cgal::Segment_2> &segments, const std::vector<cgal::Segment_2>::iterator &it) {
  auto it_n = it + 1;

  if (it_n == segments.end())
    it_n = segments.begin();

  return it_n;
}

std::vector<cgal::Segment_2>::iterator prev(std::vector<cgal::Segment_2> &segments, const std::vector<cgal::Segment_2>::iterator &it) {
  std::vector<cgal::Segment_2>::iterator it_p;

  if (it == segments.begin())
    it_p = segments.end() - 1;
  else
    it_p = it - 1;

  return it_p;
}

std::tuple<pl::SamplingGrid, unsigned, double> init(const pl::CommonSettingList &common_settings) {
    pl::CommonSetting c_s_sampling_grid, c_s_phases, c_s_radius;

  if (auto t = pl::find(common_settings, "sampling grid"))
    c_s_sampling_grid = *t;
  else {
    std::string msg = std::string("essential common setting 'sampling grid' not set to generate random pointList");
    throw std::runtime_error(msg);
  }

  if (auto t = pl::find(common_settings, "nodes"))
    c_s_phases = *t;
  else {
    std::string msg = std::string("essential common setting 'nodes' not set to generate random pointList");
    throw std::runtime_error(msg);
  }

  if (auto t = pl::find(common_settings, "radius"))
    c_s_radius = *t;
  else
    c_s_radius = pl::CommonSetting("radius", "", 3, "number", "60");

  // TODO:
  // make it robust!
  pl::SamplingGrid sampling_grid(c_s_sampling_grid);
  unsigned phases = std::stoi(c_s_phases.val);
  double radius = std::stod(c_s_radius.val);

  return {sampling_grid, phases, radius};
}

std::vector<cgal::Segment_2> init(const pl::PointList &point_list) {
  std::vector<cgal::Segment_2> segments;

  for (size_t i = 1, size = point_list.size(); i < size; ++i) {
    cgal::Point_2
      source(point_list[i - 1].x, point_list[i - 1].y),
      target(point_list[i].x, point_list[i].y);
    segments.push_back({source, target});
  }

  // close the polygon
  segments.push_back({segments[segments.size() - 1].target(), segments[0].source()});

  return segments;
}
