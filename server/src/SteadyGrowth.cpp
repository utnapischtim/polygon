#include <algorithm>
#include <vector>
#include <deque>
#include <cstdlib>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangle_2.h>

#include "easylogging++.h"

#include "SteadyGrowth.h"
#include "Point.h"
#include "random.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

static void removePoint(std::vector<cgal::Point_2> &point_list, cgal::Point_2 p);
static std::tuple<std::deque<cgal::Point_2>, std::vector<cgal::Segment_2>> init(std::vector<cgal::Point_2> &point_2_list);
static bool supportVertices(std::deque<cgal::Point_2> &hull, const cgal::Point_2 &s_k, cgal::Point_2 &v_l, cgal::Point_2 &v_r);
static std::vector<cgal::Segment_2> calculateVisibleSegments(const std::vector<cgal::Segment_2> &segments, cgal::Point_2 &v_l, cgal::Point_2 &v_r);
static void replaceSegment(std::vector<cgal::Segment_2> &segments, cgal::Segment_2 segment, cgal::Point_2 s_k);

pl::PointList pl::steadyGrowth(pl::PointList point_list) {
  VLOG(1) << "steady growth";

  pl::PointList final_list;
  pl::random_selector<> selector{};

  auto [hull, segments] = init(point_list);
  size_t n = point_list.size();

  size_t N = 0;

  for (size_t i = 0; i < n; ++i) {
    cgal::Point_2 s_k;

    cgal::Point_2 v_l, v_r;

    for (bool predicate = true; predicate;) {
      if (++N > 5*n)
        std::exit(-1);
      s_k = selector(point_list);

      VLOG(4) << "  s_k: " << s_k;

      cgal::Point_2 deleted_point = hull[0];

      // this does not have to be reset, because the only case where
      // this has to be, is if there are crossing points, and if that
      // happens, the function return, without changing hull
      if (supportVertices(hull, s_k, v_l, v_r)) {

        // loop through every point to find points they lie within the
        // triangle of v_l, v_r and s_k. if there exists one, this is
        // the new s_k point, the checked points before have not to be
        // checked, because of ... heuristic ... lemma 2.5 page 44
        for (auto p : point_list) {
          cgal::Triangle_2 triangle(v_l, v_r, s_k);

          if (triangle.bounded_side(p) == CGAL::ON_BOUNDED_SIDE) {
            cgal::Triangle_2
              triangle_l(v_l, s_k, deleted_point),
              triangle_r(deleted_point, s_k, v_r);

            if (triangle_l.bounded_side(p) == CGAL::ON_BOUNDED_SIDE) {
              v_r = deleted_point;
              hull.push_front(deleted_point);
            }
            else if (triangle_r.bounded_side(p) == CGAL::ON_BOUNDED_SIDE) {
              v_l = deleted_point;
              hull.push_back(deleted_point);
            }
            else {
              // nothing, because it is in the intersection of left
              // area and right area and therefore both support
              // vertices remain the same
            }

            s_k = p;
          }
        }

        hull.push_back(s_k);
        hull.push_front(s_k);

        removePoint(point_list, s_k);

        predicate = false;
      }

      if (VLOG_IS_ON(4)) {
        VLOG(4) << "  hull";
        for (auto p : hull)
          VLOG(4) << "    p: " << p;
      }
    }

    // find all segments that lie between v_l and v_r
    std::vector<cgal::Segment_2> visible_segments = calculateVisibleSegments(segments, v_l, v_r);

    if (VLOG_IS_ON(3)) {
      VLOG(3) << "vissible segments";
      for (auto s : visible_segments)
        VLOG(4) << "  s: " << s;
    }

    // select one at random
    cgal::Segment_2 visible_segment = visible_segments[pl::randomValueOfRange(0, visible_segments.size() - 1)];

    // replace segment with the new segments
    // replace edge e with (v_i, s_k) and (s_k, v_i+1)
    replaceSegment(segments, visible_segment, s_k);
  }

  for (auto segment : segments) {
    // auto p = segment.source();
    final_list.push_back(segment.source()); //{p.x(), p.y()});
  }
  final_list.push_back(final_list[0]);



  return final_list;
}


void removePoint(std::vector<cgal::Point_2> &point_list, cgal::Point_2 point) {
  VLOG(3) << "removePoint";
  point_list.erase(std::remove_if(point_list.begin(),
                                  point_list.end(),
                                  [&](auto p) { return p == point; }),
                   point_list.end());
}

std::tuple<std::deque<cgal::Point_2>, std::vector<cgal::Segment_2>> init(std::vector<cgal::Point_2> &point_list) {
  VLOG(3) << "init";
  pl::random_selector<> selector{};

  cgal::Point_2 s_1 = selector(point_list);
  removePoint(point_list, s_1);

  cgal::Point_2 s_2 = selector(point_list);
  removePoint(point_list, s_2);

  cgal::Point_2 s_3;

  for (bool predicate = false; predicate;) {
    s_3 = selector(point_list);
    cgal::Triangle_2 triangle(s_1, s_2, s_3);

    predicate = predicate || triangle.is_degenerate();

    // points are within the triangle
    for (const auto &p : point_list)
      predicate = predicate || triangle.bounded_side(p) == CGAL::ON_BOUNDED_SIDE || triangle.bounded_side(p) == CGAL::ON_BOUNDARY;
  }

  removePoint(point_list, s_3);

  // test! ;)
  // s_1 = {200, 200};
  // s_2 = {300, 400};
  // s_3 = {500, 100};

  // point_2_list = {{500,400}, {600, 0}, {590, 80}, {580, 10}//, {570, 50}, {560, 70}, {100, 200}
  // };

  std::deque<cgal::Point_2> point_hull = {s_3, s_1, s_2, s_3};
  std::vector<cgal::Segment_2> segments = {{s_1, s_2}, {s_2, s_3}, {s_3, s_1}};

  return {point_hull, segments};
}

bool supportVertices(std::deque<cgal::Point_2> &hull, const cgal::Point_2 &s_k, cgal::Point_2 &v_l, cgal::Point_2 &v_r) {
  VLOG(3) << "supportVertices";
  VLOG(4) << "  s_k: " << s_k;

  // https://maxgoldste.in/melkman/ helped

  // crossing lines
  if (CGAL::left_turn(*(hull.begin() + 1), *(hull.begin()), s_k) && CGAL::right_turn(*(hull.end() - 2), *(hull.end() - 1), s_k)) {
    VLOG(4) << "  crossing lines";
    // TODO:
    // if this is once started has to be always?
  }

  cgal::Triangle_2 triangle(*(hull.begin() + 1), *(hull.begin()), s_k);
  while (!CGAL::left_turn(*(hull.begin() + 1), *(hull.begin()), s_k) && !triangle.is_degenerate()) {
    hull.pop_front();
    triangle = {*(hull.begin() + 1), *(hull.begin()), s_k};
  }

  triangle = {*(hull.end() - 2), *(hull.end() - 1), s_k};
  while (!CGAL::right_turn(*(hull.end() - 2), *(hull.end() - 1), s_k) && !triangle.is_degenerate()) {
    hull.pop_back();
    triangle = {*(hull.end() - 2), *(hull.end() - 1), s_k};
  }

  v_r = *(hull.begin());
  v_l = *(hull.end() - 1);

  return true;
}

std::vector<cgal::Segment_2> calculateVisibleSegments(const std::vector<cgal::Segment_2> &segments, cgal::Point_2 &v_l, cgal::Point_2 &v_r) {
  VLOG(3) << "calculateVisibleSegments";
  VLOG(4) << "  v_l: " << v_l << "  v_r: " << v_r;

  std::vector<cgal::Segment_2> visible_segments;

  bool between = false;

  for (auto s : segments) {
    VLOG(4) << "  s: " << s << "  between: " << between;

    if (s.source() == v_l || between || s.target() == v_r)
      visible_segments.push_back(s);

    // TODO:
    // maybee it is possible to write those two statements within one?
    if (s.source() == v_l)
      between = true;

    if (s.target() == v_r)
      between = false;
  }

  return visible_segments;
}

void replaceSegment(std::vector<cgal::Segment_2> &segments, cgal::Segment_2 segment, cgal::Point_2 s_k) {
  VLOG(3) << "replaceSegment";
  VLOG(4) << "  segment: " << segment;

  auto it = std::find_if(segments.begin(), segments.end(), [&](auto s) { return segment == s; });
  it = segments.erase(it);
  segments.insert(it, {segment.source(), s_k});
  segments.insert(++it, {s_k, segment.target()});

  if (VLOG_IS_ON(3)) {
    for (auto s : segments)
      VLOG(4) << "  s: " << s;
  }
}
