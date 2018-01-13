#include <algorithm>
#include <vector>
#include <deque>
#include <cstdlib>
#include <iterator>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Triangular_expansion_visibility_2.h>
#include <CGAL/Arr_non_caching_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>

#include "easylogging++.h"

#include "SteadyGrowth.h"
#include "Point.h"
#include "random.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;
using Iter = pl::PointList::const_iterator;

static Iter next(const pl::PointList &list, const Iter &it);
static Iter prev(const pl::PointList &list, const Iter &it);

// to start the creation of the polygon two independend points where
// necessary! the points where choosen randomly
static std::tuple<cgal::Point_2, cgal::Point_2> locateTwoIndependentPoints(const pl::PointList &point_list);

// remove the point from the point list
static void removePoint(pl::PointList &point_list, cgal::Point_2 point);

// choose a random point which is outside of the hull. the return
// value is the point and its support vertices. the support vertices
// are returned also from this function because they are necessary to
// calculate the point. in particular they are necessary to check if
// no point is inside of the new convex hull. and to check this, the
// support vertices are searched and the check is performed that all
// remaining points of the point list are outside of the triangle of
// point and support vertices!
static std::tuple<cgal::Point_2, Iter, Iter> locateRandomPoint(const pl::PointList &point_list, const pl::PointList &hull);

// add the point to the convex hull
static void addToConvexHull(pl::PointList &hull, cgal::Point_2 &p, Iter &s_l, Iter &s_r);

// return a random segment completly visible from the point p. to do
// that some steps are necessary.
static cgal::Segment_2 locateVisibleSegment(const pl::PointList &final_list, cgal::Point_2 p, cgal::Point_2 s_l, cgal::Point_2 s_r);

// add the point p after the source point of the choosen segment seg.
static void replaceSegment(pl::PointList &final_list, const cgal::Segment_2 &seg, const cgal::Point_2 &p);


pl::PointList pl::steadyGrowth(pl::PointList point_list) {
  VLOG(3) << "steadyGrowth";

  auto [s_1, s_2] = locateTwoIndependentPoints(point_list);
  removePoint(point_list, s_1);
  removePoint(point_list, s_2);

  pl::PointList hull = {s_1, s_2};
  pl::PointList final_list = {s_1, s_2};

  // O(n)
  for (size_t i = 0, size = point_list.size(); i < size; ++i) {
    // search point no other point is within the temporarly convex
    // hull O(n)
    for (auto h : hull)
      VLOG(4) << "   h: " << h;
    auto [p, s_l, s_r] = locateRandomPoint(point_list, hull);

    // the iterator is necessary for the function addToConvexHull, but
    // locateVisibleSegment needs the point. it would be possible to
    // use iterator arithmetic in addToConvexHull to not cache it
    // here, but that would be creepy
    cgal::Point_2
      s_l_p = *s_l,
      s_r_p = *s_r;

    addToConvexHull(hull, p, s_l, s_r);
    removePoint(point_list, p);

    // O(n)
    cgal::Segment_2 visible_segment = locateVisibleSegment(final_list, p, s_l_p, s_r_p);

    // O(n)
    replaceSegment(final_list, visible_segment, p);

    for (auto f : final_list)
      VLOG(4) << "   f: " << f;

    VLOG(3) << "  size: " << final_list.size() << " p: " << p;
    VLOG(3) << "---------------------------------";
  }

  final_list.push_back(final_list[0]);

  return final_list;
}

std::tuple<cgal::Point_2, cgal::Point_2> locateTwoIndependentPoints(const pl::PointList &point_list) {
  VLOG(3) << "  locateTwoIndependendPoints";
  pl::random_selector<> selector{};

  cgal::Point_2 s_1 = selector(point_list);
  cgal::Point_2 s_2;

  do {
    s_2 = selector(point_list);
  } while (s_1 == s_2);

  // this flip is necessary to run clockwise.
  if (s_1.x() > s_2.x())
    std::swap(s_1, s_2);

  return {s_1, s_2};
}

void removePoint(pl::PointList &point_list, cgal::Point_2 point) {
  VLOG(3) << "  removePoint";
  point_list.erase(std::remove_if(point_list.begin(),
                                  point_list.end(),
                                  [&](auto p) { return p == point; }),
                   point_list.end());
}

Iter calculateStartPoint(const pl::PointList &hull, const cgal::Point_2 &p) {
  VLOG(3) << "    calculateStartPoint";
  // to have better possibility to test this function. use the first
  // point of the hull.
  cgal::Point_2 x = hull[0];
  cgal::Segment_2 px = {p, x};
  Iter start_point = hull.begin();

  for (size_t i = 0, size = hull.size(); i < size; ++i) {
    // to get also the segment from the last to the first hull point
    size_t
      source = i == 0 ? size - 1 : i-1,
      target = i;

    cgal::Segment_2 uv = {hull[source], hull[target]};

    // this should only occure once.
    if (CGAL::do_intersect(uv, px) && uv.source() != px.target() && uv.target() != px.target())
      start_point = hull.begin() + i;

  }

  return start_point;
}


std::tuple<Iter, Iter> locateSupportVertices(const pl::PointList &hull, const cgal::Point_2 &p) {
  VLOG(3) << "    locateSupportVertices";
  Iter s_l, s_r;

  Iter start_point = calculateStartPoint(hull, p);

  if (hull.size() == 2) {
    if (CGAL::left_turn(hull[0], hull[1], p)) {
      s_l = hull.begin();
      s_r = hull.begin() + 1;
    } else {
      s_l = hull.begin() + 1;
      s_r = hull.begin();
    }
  }

  else {
    Iter u = start_point;
    Iter v = next(hull, u);

    while (CGAL::left_turn(p, *u, *v)) {
      u = v;
      v = next(hull, u);
    }

    s_r = u;

    u = start_point;
    v = prev(hull, u);
    while (CGAL::right_turn(p, *u, *v)) {
      u = v;
      v = prev(hull, u);
    }

    s_l = u;
  }

  return {s_l, s_r};
}

void recalculateSupportVertices(const pl::PointList &hull, const cgal::Point_2 &p, Iter &s_l, Iter &s_r) {
  VLOG(3) << "    recalculateSupportVertices";
  Iter v = next(hull, s_l);
  while (CGAL::right_turn(p, *s_l, *v)) {
    s_l = v;
    v = next(hull, s_l);
  }

  v = prev(hull, s_r);
  while (CGAL::left_turn(p, *s_r, *v)) {
    s_r = v;
    v = prev(hull, s_r);
  }
}


std::tuple<cgal::Point_2, Iter, Iter> locateRandomPoint(const pl::PointList &point_list, const pl::PointList &hull) {
  VLOG(3) << "  locateRandomPoint";
  pl::random_selector<> selector{};

  cgal::Point_2 p = selector(point_list);

  // naive approach O(n)
  // with Preparata should be possible to search with O(log n)
  auto [s_l, s_r] = locateSupportVertices(hull, p);

  cgal::Triangle_2 triangle(*s_l, *s_r, p);

  for (auto point : point_list)
    if (triangle.has_on_bounded_side(point)) {
      p = point;

      // recalculate the support vertices
      recalculateSupportVertices(hull, p, s_l, s_r);

      triangle = {*s_l, *s_r, p};
    }

  return {p, s_l, s_r};
}

void addToConvexHull(pl::PointList &hull, cgal::Point_2 &p, Iter &s_l, Iter &s_r) {
  VLOG(3) << "  addToConvexHull";

  if (s_l < s_r) {
    size_t dist = std::distance(s_l, s_r);

    // remove points [s_l + 1, s_r)
    hull.erase(s_l + 1, s_r);

    s_r -= dist - 1;

    // insert before s_r
    hull.insert(s_r, p);
  }

  // s_r < s_l and s_l is not the last point
  else {
    // if s_l would be the last element, it would do nothing
    hull.erase(s_l + 1, hull.end());

    // if s_r is the first, it would do nothing either
    hull.erase(hull.begin(), s_r);

    // it is cheaper to add it at the end, it should not matter if it
    // is in front or back!
    hull.push_back(p);
  }
}

// return the points visible from p in order from s_l to s_r
// O(2n) => O(n)
pl::PointList calculatePossibleVisiblePoints(const pl::PointList &final_list, const cgal::Point_2 &s_l, const cgal::Point_2 &s_r) {
  VLOG(3) << "    calculatePossibleVisiblePoints";
  pl::PointList visible_points;
  bool within_possible_visible_points = false;
  bool exit = false;

  for (auto it = final_list.begin(); !exit; it = next(final_list, it)) {
    cgal::Point_2 point = *it;

    if (point == s_l)
      within_possible_visible_points = true;

    if (within_possible_visible_points)
      visible_points.push_back(point);

    if (within_possible_visible_points && point == s_r)
      exit = true;
  }

  return visible_points;
}

// return the segments from visible points
std::vector<cgal::Segment_2> calculatePossibleVisibleSegments(const pl::PointList &visible_points) {
  VLOG(3) << "    calculatePossibleVisibleSegments";
  std::vector<cgal::Segment_2> segments;

  for (size_t i = 1, size = visible_points.size(); i < size; ++i)
    segments.push_back(cgal::Segment_2(visible_points[i-1], visible_points[i]));

  return segments;
}

// return the segments containing the segments from and to p from s_l
// and s_r to complete the polygon!
std::vector<cgal::Segment_2> createRegularSegments(const std::vector<cgal::Segment_2> &segments, const cgal::Point_2 &p) {
  VLOG(3) << "    createRegularSegments";
  std::vector<cgal::Segment_2> regular_segments = segments;

  cgal::Point_2 s_l = segments[0].source();
  cgal::Point_2 s_r = segments[segments.size()-1].target();

  regular_segments.push_back(cgal::Segment_2(s_r, p));
  regular_segments.push_back(cgal::Segment_2(p, s_l));

  return regular_segments;
}

pl::PointList calculateRegularPoints(const std::vector<cgal::Segment_2> &segments, cgal::Point_2 query_point) {
  VLOG(3) << "    calculateRegularPoints";

  VLOG(4) << "      p: " << query_point;
  for (auto seg : segments)
    VLOG(4) << "      seg: " << seg;

  using Arrangement_2 = CGAL::Arrangement_2<CGAL::Arr_non_caching_segment_traits_2<cgal>>;
  Arrangement_2 env;
  CGAL::insert_non_intersecting_curves(env, segments.begin(), segments.end());
  Arrangement_2::Halfedge_const_handle he = env.halfedges_begin();

  cgal::Point_2 point_before_query_point = segments[segments.size() - 2].source();

  // https://doc.cgal.org/latest/Visibility_2/Visibility_2_2general_polygon_example_8cpp-example.html
  while (he->source()->point() != point_before_query_point || he->target()->point() != query_point)
    he++;

  using TEV = CGAL::Triangular_expansion_visibility_2<Arrangement_2>;
  Arrangement_2 regular_output;
  TEV tev(env);

  Arrangement_2::Face_handle fh = tev.compute_visibility(query_point, he, regular_output);

  pl::PointList regular_points;

  Arrangement_2::Ccb_halfedge_circulator curr = fh->outer_ccb();

  // with this the first construct, the first segment is not used, but
  // this is intentionally!
  while (++curr != fh->outer_ccb())
    regular_points.push_back(curr->target()->point());

  for (auto k : regular_points)
    VLOG(4) << "  k: " << k;

  return regular_points;
}



// return the real segments visible from point p. WITHOUT any partial
// or complete hidden segment.
std::vector<cgal::Segment_2> calculateRealVisibleSegments(const pl::PointList &visible_points, const pl::PointList &regular_points) {
  VLOG(3) << "    calculateRealVisibleSegments";
  // iterator
  Iter vis = visible_points.begin();
  Iter reg = regular_points.begin();

  std::vector<cgal::Segment_2> visible_segments;

  do {
    if (*(vis + 1) == *(reg + 1)) {
      visible_segments.push_back({*vis, *(vis+1)});

      vis += 1;
      reg += 1;
    }
    else {
      // there exists 3 cases.
      // 1) is that the next reg (+1) could be a visible point. or
      //    that the segment is on the line from p to the point +1
      // 2) is that the after next reg (+2) could be the visible point
      // 3) is that the after after next reg (+3) could be equal to a
      //    visible point
      // there should no other possibilities!

      bool
        is_first_case = false,
        is_second_case = false,
        is_third_case = false;

      do {
        vis += 1;

        is_first_case = *vis == *(reg + 1);
        is_second_case = *vis == *(reg + 2);
        is_third_case = *vis == *(reg + 3);
      } while (!(is_first_case || is_second_case || is_third_case));

      if (is_first_case)
        reg += 1;

      if (is_second_case)
        reg += 2;

      if (is_third_case)
        reg += 3;
    }
    // the last point from both should always be the s_r, and
    // therefore both are at the end of the vector!
  } while ((vis+1) < visible_points.end() && (reg+1) < regular_points.end());

  return visible_segments;
}

cgal::Segment_2 chooseRandomSegment(std::vector<cgal::Segment_2> segments) {
  VLOG(3) << "    chooseRandomSegment";
  pl::random_selector<> selector{};
  cgal::Segment_2 seg = selector(segments);
  return seg;
}


cgal::Segment_2 locateVisibleSegment(const pl::PointList &final_list, cgal::Point_2 p, cgal::Point_2 s_l, cgal::Point_2 s_r) {
  VLOG(3) << "  locateVisibleSegment";
  VLOG(4) << "    s_l: " << s_l << " s_r: " << s_r;
  for (auto q : final_list)
    VLOG(4) << "    q: " << q;
  cgal::Segment_2 located_visible_segment;

  pl::PointList visible_points = calculatePossibleVisiblePoints(final_list, s_l, s_r);

  // with one or two segments it is not necessary to do the calculation.
  // and some problematic cases don't have to be looked
  if (visible_points.size() < 4)
    located_visible_segment = cgal::Segment_2(visible_points[0], visible_points[1]);
  else {
    std::vector<cgal::Segment_2> segments = calculatePossibleVisibleSegments(visible_points);
    std::vector<cgal::Segment_2> regular_segments = createRegularSegments(segments, p);
    pl::PointList regular_points = calculateRegularPoints(regular_segments, p);
    std::vector<cgal::Segment_2> visible_segments = calculateRealVisibleSegments(visible_points, regular_points);
    located_visible_segment = chooseRandomSegment(visible_segments);
  }
  return located_visible_segment;
}


void replaceSegment(pl::PointList &final_list, const cgal::Segment_2 &seg, const cgal::Point_2 &p) {
  VLOG(3) << "  replaceSegment";
  cgal::Point_2 source = seg.target();
  auto iter_source = std::find_if(final_list.begin(), final_list.end(), [&](auto const& point){ return source == point; });
  final_list.insert(iter_source, p);
}

Iter next(const pl::PointList &list, const Iter &it) {
  VLOG(3) << "      next";
  auto it_n = it + 1;

  if (it_n == list.end())
    it_n = list.begin();

  return it_n;
}

Iter prev(const pl::PointList &list, const Iter &it) {
  VLOG(3) << "      prev";
  Iter it_p;

  if (it == list.begin())
    it_p = list.end() - 1;
  else
    it_p = it - 1;

  return it_p;
}
