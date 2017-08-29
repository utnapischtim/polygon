#include <limits>
#include <cstdlib>
#include <iterator>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "BouncingVertices.h"
#include "Point.h"
#include "CommonSetting.h"
#include "random.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

static std::vector<cgal::Segment_2>::iterator next(std::vector<cgal::Segment_2> &segments, const std::vector<cgal::Segment_2>::iterator &it);

pl::PointList pl::bouncingVertices(pl::PointList point_list, pl::CommonSettingList common_settings) {
  pl::PointList final_list;

  std::vector<cgal::Segment_2> segments;
  for (size_t i = 1, size = point_list.size(); i < size; ++i) {
    cgal::Point_2
      source(point_list[i - 1].x, point_list[i - 1].y),
      target(point_list[i].x, point_list[i].y);
    segments.push_back({source, target});
  }

  // close the polygon
  segments.push_back({segments[segments.size() - 1].target(), segments[0].source()});

  // TODO:
  // implement it little more passiv
  auto c_s_sampling_grid = pl::find(common_settings, "sampling grid");
  pl::SamplingGrid sampling_grid(c_s_sampling_grid);
  unsigned width = sampling_grid.width;
  unsigned height = sampling_grid.height;

  // TODO:
  // implement it little more passiv
  auto c_s_phases = pl::find(common_settings, "phases");
  unsigned phases = std::stoi(c_s_phases.val);

  // TODO:
  // implement it little more passiv
  auto c_s_radius = pl::find(common_settings, "radius");
  double radius = std::stod(c_s_radius.val);

  for (size_t i = 0; i < phases; ++i) {
    // not use next or prev because this would cause a endles loop
    for (auto sit = segments.begin(); sit != segments.end(); ++sit) {
      bool
        outside_boundary_box = false,
        intersection_occur = false;

      cgal::Point_2 target;

      std::vector<cgal::Segment_2>::iterator sitn = next(segments, sit);

      do {
        intersection_occur = false;

        // random set of new target of segment and after the do while
        // also set the new source of the next segment
        cgal::Point_2 velocity = {pl::randomValueOfRange(-radius, radius), pl::randomValueOfRange(-radius, radius)};

        // check if the new target of the segment is inside of the
        // boundary box, the new source of the next segment does not
        // have to be checked, because it is clear that if the target is
        // inside also the next source which is equal to target is inside.
        target = {sit->target().x() + velocity.x(), sit->target().y() + velocity.y()};
        outside_boundary_box = target.x() < 0 || width < target.x() || target.y() < 0 || height < target.y();

        // check only if the new point is inside, the following check
        // is more complex and hence more cpu consuming
        if (!outside_boundary_box) {

          // check this segment, and check also next segment, because next
          // segment has also a change, the source, for this segment the
          // target changes
          cgal::Segment_2
            e_1 = {sit->source(), target},
            e_2 = {target, sitn->target()},
            old_e_1 = *sit,
            old_e_2 = *sitn;

          // *ssit != old_e_1 && *ssit != old_e_2, the new segments
          //   should not be checked about intersection with the old,
          //   because per def they should intersect
          // ssit->target() != e_1.source() && ssit->source() != e_2.target()
          //   the binding points between the segments does not count
          //   as intersections. and this construct should work in
          //   both directions, clockwise and anti-clockwise

          for (auto ssit = segments.begin(); ssit != segments.end() && !intersection_occur; ++ssit)
            if (*ssit != old_e_1 && *ssit != old_e_2) {
              if (ssit->target() != e_1.source())
                intersection_occur = intersection_occur || CGAL::do_intersect(*ssit, e_1);

              if (ssit->source() != e_2.target())
                intersection_occur = intersection_occur || CGAL::do_intersect(*ssit, e_2);
            }
        }
      } while (outside_boundary_box || intersection_occur);

      // set the new segments;
      *sit = cgal::Segment_2(sit->source(), target);
      *sitn = cgal::Segment_2(target, sitn->target());
    }
  }

  for (auto segment : segments) {
    auto p = segment.source();
    final_list.push_back({p.x(), p.y()});
  }
  final_list.push_back(final_list[0]);

  return final_list;
}

std::vector<cgal::Segment_2>::iterator next(std::vector<cgal::Segment_2> &segments, const std::vector<cgal::Segment_2>::iterator &it) {
  auto it_n = it + 1;

  if (it_n == segments.end())
    it_n = segments.begin();

  return it_n;
}
