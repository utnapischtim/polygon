#include <algorithm>
#include <vector>
#include <tuple>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangle_2.h>

#include "easylogging++.h"

#include "TwoOptMoves.h"
#include "Point.h"
#include "random.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

static std::vector<std::tuple<cgal::Segment_2, cgal::Segment_2>> calculateIntersections(const std::vector<cgal::Segment_2> &segments);
static std::tuple<cgal::Segment_2, cgal::Segment_2> calculateNewSegments(const std::vector<cgal::Segment_2> &segments, const cgal::Segment_2 &f_1, const cgal::Segment_2 &f_2);
pl::PointList calculateFinalList(std::vector<cgal::Segment_2> &segments);
static void goingInDepth(std::vector<cgal::Segment_2> &segments, cgal::Point_2 current, pl::PointList &final_list);

pl::PointList pl::twoOptMoves(pl::PointList point_list) {
  VLOG(3) << "twoOptMoves";
  pl::PointList final_list;
  pl::random_selector<> selector{};

  std::vector<cgal::Point_2> point_2_list;
  pl::convert(point_2_list, point_list);

  std::vector<cgal::Segment_2> segments;
  for (size_t i = 1, size = point_2_list.size(); i < size; ++i)
    segments.push_back({point_2_list[i - 1], point_2_list[i]});

  // close the polygon
  segments.push_back({point_2_list[point_2_list.size() - 1], point_2_list[0]});

  VLOG(4) << "  segments.size(): " << segments.size();
  if (VLOG_IS_ON(5)) {
    VLOG(5) << "  segments:";
    for (auto s : segments)
      VLOG(5) << "    s: " << s;
  }

  auto intersection_segments = calculateIntersections(segments);

  VLOG(4) << "  intersection_segments.size(): " << intersection_segments.size();
  if (VLOG_IS_ON(5)) {
    VLOG(5) << "  intersections_segments:";
    for (auto i_s : intersection_segments)
      VLOG(5) << "      intersection_segments: (" << std::get<0>(i_s) << " cross " << std::get<1>(i_s) << ")";
  }

  for (; 0 < intersection_segments.size();) {
    VLOG(5) << "    intersection_segments.size(): " << intersection_segments.size();

    auto [f_1, f_2] = selector(intersection_segments);

    VLOG(5) << "    choosen_intersection: (f_1: " << f_1 << " cross f_2: " << f_2 << ")";

    // remove the intersections that have f1 or f2 same as the choosen
    // current intersection
    intersection_segments.erase(std::remove_if(intersection_segments.begin(),
                                               intersection_segments.end(),
                                               [&](auto inter){ return f_1 == std::get<0>(inter) ||
                                                                       f_2 == std::get<0>(inter) ||
                                                                       f_1 == std::get<1>(inter) ||
                                                                       f_2 == std::get<1>(inter); }),
                                intersection_segments.end());

    // replace f_1 with e_1 and f_2 with e_2

    auto [e_1, e_2] = calculateNewSegments(segments, f_1, f_2);


    auto it_f_1 = std::find_if(segments.begin(), segments.end(), [&](auto seg){ return seg == f_1; });
    segments.erase(it_f_1);
    // segments.insert(it_f_1, e_1);
    auto it_f_2 = std::find_if(segments.begin(), segments.end(), [&](auto seg){ return seg == f_2; });
    segments.erase(it_f_2);
    // segments.insert(it_f_2, e_2);

    VLOG(5) << "    e_1: " << e_1 << " e_2: " << e_2;
    if (VLOG_IS_ON(6)) {
      VLOG(6) << "   segments: ";
      for (auto s : segments)
        VLOG(6) << "    s: " << s;
    }

    for (auto s : segments) {
      if (s != e_1 &&
          s.source() != e_1.target() && s.target() != e_1.source() && s.source() != e_1.source() && s.target() != e_1.target() &&
          CGAL::do_intersect(s, e_1) &&
          std::count_if(intersection_segments.begin(), intersection_segments.end(), [&](auto inter){ return e_1 == std::get<0>(inter) && s == std::get<1>(inter); }) == 0)
        intersection_segments.push_back({s, e_1});

      if (s != e_2 &&
          s.source() != e_2.target() && s.target() != e_2.source() && s.source() != e_2.source() && s.target() != e_2.target() &&
          CGAL::do_intersect(s, e_2) &&
          std::count_if(intersection_segments.begin(), intersection_segments.end(), [&](auto inter){ return e_2 == std::get<0>(inter) && s == std::get<1>(inter); }) == 0)
        intersection_segments.push_back({s, e_2});
    }

    // do it after to the segments intersection check to not confuse it
    segments.insert(it_f_1, e_1);
    segments.insert(it_f_2, e_2);

    // with this it would be O(n^3) i don't know if the above approach
    //is correct, it would be to simple to reduce it to O(n^2)
    //intersection_segments = calculateIntersections(segments);

    VLOG(5) << "    intersection_segments.size(): " << intersection_segments.size();
    if (VLOG_IS_ON(6)) {
      for (auto i_s : intersection_segments)
        VLOG(6) << "      i_s: (" << std::get<0>(i_s) << " cross " << std::get<1>(i_s) << ")";
    }

    if (VLOG_IS_ON(9)) {
      std::exit(0);
    }
  }

  if (VLOG_IS_ON(3)) {
    VLOG(3) << "segments: ";
    for (auto s : segments)
      VLOG(3) << "  s: " << s;
  }

  final_list = calculateFinalList(segments);

  return final_list;
}

std::vector<std::tuple<cgal::Segment_2, cgal::Segment_2>> calculateIntersections(const std::vector<cgal::Segment_2> &segments) {
  VLOG(3) << "calculateIntersections";
  std::vector<std::tuple<cgal::Segment_2, cgal::Segment_2>> intersection_segments;

  // s_1 != s_2

  // s_1.source() != s_2.target() && s_1.target() != s_2.source() this
  // is necessary because do_intersect would treat this as a
  // intersection

  // maybe this part should be done in a better time complexity,
  // because with this, we are on O(n^3)
  // std::count(intersection_segments.begin(), intersection_segments.end(), [&](auto tup){ return std::get<0>(tup) == s_2 || std::get<1>(tup) == s_1}) == 0
  // check if there exists intersections with segments in reverse order

  for (auto s_1 : segments)
    for (auto s_2 : segments)
      if (s_1 != s_2 &&
          s_1.source() != s_2.target() && s_1.target() != s_2.source() && s_1.source() != s_2.source() && s_1.target() != s_2.target() &&
          CGAL::do_intersect(s_1, s_2) &&
          std::count_if(intersection_segments.begin(), intersection_segments.end(), [&](auto inter){ return s_2 == std::get<0>(inter) && s_1 == std::get<1>(inter); }) == 0)
        intersection_segments.push_back({s_1, s_2});

  return intersection_segments;
}

std::tuple<cgal::Segment_2, cgal::Segment_2> calculateNewSegments(const std::vector<cgal::Segment_2> &segments, const cgal::Segment_2 &f_1, const cgal::Segment_2 &f_2) {
  // TODO:
  // there has to be a check, if the combination of the segment does
  // just exists
  cgal::Segment_2 e_1(f_1.source(), f_2.source()), e_2(f_1.target(), f_2.target());

  // it could be possible, it was possible when i was doing the
  // algorithm by hand
  if (std::any_of(segments.begin(), segments.end(), [&](auto s) { return e_1 == s || e_2 == s; })) {
    e_1 = {f_1.source(), f_2.target()};
    e_2 = {f_1.target(), f_2.source()};
  }

  if (std::any_of(segments.begin(), segments.end(), [&](auto s) { return e_1 == s || e_2 == s; }))
    throw std::runtime_error("segments could not be changed, because they exists in both variants just");

  return {e_1, e_2};
}


pl::PointList calculateFinalList(std::vector<cgal::Segment_2> &segments) {
  pl::PointList final_list;

  auto segment = segments[0];

  segments.erase(segments.begin());

  auto p = segment.source(), next = segment.target();

  final_list.push_back({p.x(), p.y()});

  goingInDepth(segments, next, final_list);

  final_list.push_back(final_list[0]);

  return final_list;
}

// maybe it is possible to rewrite it with fold expressions
void goingInDepth(std::vector<cgal::Segment_2> &segments, cgal::Point_2 current, pl::PointList &final_list) {
  VLOG(6) << "goingInDepth segments.size: " << segments.size();

  for (auto it = segments.begin(); it != segments.end(); ++it) {
    auto p = (*it).source(), q = (*it).target();

    if (p == current) {
      final_list.push_back({p.x(), p.y()});
      segments.erase(it);
      return goingInDepth(segments, q, final_list);
    }

    if (q == current) {
      final_list.push_back({q.x(), q.y()});
      segments.erase(it);
      return goingInDepth(segments, p, final_list);
    }
  }

  if (segments.size() == 0)
    return;

  if (segments.size() > 0)
    throw std::runtime_error("it should not be possible to arrive to this exception, there was no successor of the searched point");
}
