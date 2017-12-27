#include <algorithm>
#include <vector>
#include <deque>
#include <cstdlib>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Triangular_expansion_visibility_2.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>

#include "easylogging++.h"

#include "SteadyGrowth.h"
#include "Point.h"
#include "random.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;
using Iter = pl::PointList::const_iterator;

Iter next(const pl::PointList &list, const Iter &it);
Iter prev(const pl::PointList &list, const Iter &it);

static std::tuple<cgal::Point_2, cgal::Point_2> locateTwoIndependentPoints(const pl::PointList &point_list);
static void removePoint(pl::PointList &point_list, cgal::Point_2 point);
static std::tuple<cgal::Point_2, cgal::Point_2, cgal::Point_2> locateRandomPoint(const pl::PointList &point_list, pl::PointList &hull);
static Iter calculateNearestPoint(const pl::PointList &hull, const cgal::Point_2 &p);
static std::tuple<Iter, Iter> locateSupportVertices(const pl::PointList &hull, const cgal::Point_2 &p);
static void recalculateSupportVertices(const pl::PointList &hull, const cgal::Point_2 &p, Iter &s_l, Iter &s_r);
static void addToConvexHull(pl::PointList &hull, cgal::Point_2 &p, Iter &s_l, Iter &s_r);
static std::vector<cgal::Segment_2> locateVisibleSegments(const pl::PointList &final_list, cgal::Point_2 p, cgal::Point_2 s_l, cgal::Point_2 s_r);
static cgal::Point_2 chooseRandomSegment(std::vector<cgal::Segment_2> segments);
static void replaceSegment(pl::PointList &final_list, const cgal::Point_2 &source, const cgal::Point_2 &p);

pl::PointList pl::steadyGrowth(pl::PointList point_list) {
  VLOG(1) << "steady growth";

  auto [s_1, s_2] = locateTwoIndependentPoints(point_list);
  removePoint(point_list, s_1);
  removePoint(point_list, s_2);

  pl::PointList hull = {s_1, s_2};
  pl::PointList final_list = {s_1, s_2};

  VLOG(3) << "s_1: " << s_1 << " s_2: " << s_2;

  // O(n)
  for (size_t i = 0, size = point_list.size(); i < size; ++i) {
    // search point no other point is within the temporarly convex
    // hull O(n)
    auto [p, s_l, s_r] = locateRandomPoint(point_list, hull);

    removePoint(point_list, p);

    // O(n)
    auto visible_segments = locateVisibleSegments(final_list, p, s_l, s_r);

    cgal::Point_2 source = chooseRandomSegment(visible_segments);

    // O(n)
    replaceSegment(final_list, source, p);
  }

  return final_list;
}

std::tuple<cgal::Point_2, cgal::Point_2> locateTwoIndependentPoints(const pl::PointList &point_list) {
  VLOG(3) << "locateTwoIndependentPoints";

  pl::random_selector<> selector{};

  cgal::Point_2 s_1 = selector(point_list);
  cgal::Point_2 s_2;

  do {
    s_2 = selector(point_list);
  } while (s_1 == s_2);

  VLOG(4) << "  s_1: " << s_1 << " s_2: " << s_2;

  return {s_1, s_2};
}

void removePoint(pl::PointList &point_list, cgal::Point_2 point) {
  VLOG(3) << "removePoint";
  point_list.erase(std::remove_if(point_list.begin(),
                                  point_list.end(),
                                  [&](auto p) { return p == point; }),
                   point_list.end());
}


std::tuple<cgal::Point_2, cgal::Point_2, cgal::Point_2> locateRandomPoint(const pl::PointList &point_list, pl::PointList &hull) {
  VLOG(3) << "locateRandomPoint";

  pl::random_selector<> selector{};

  cgal::Point_2 p = selector(point_list);

  VLOG(3) << "  p: " << p;

  // naive approach O(n)
  // with Preparata should be possible to search with O(log n)
  auto [s_l, s_r] = locateSupportVertices(hull, p);

  VLOG(3) << "  s_l: " << *s_l << " s_r: " << *s_r;

  cgal::Triangle_2 triangle(*s_l, *s_r, p);

  for (auto point : point_list)
    if (triangle.has_on_bounded_side(point)) {
      p = point;

      // recalculate the support vertices
      recalculateSupportVertices(hull, p, s_l, s_r);

      triangle = {*s_l, *s_r, p};
    }

  addToConvexHull(hull, p, s_l, s_r);

  return {p, *s_l, *s_r};
}

Iter calculateNearestPoint(const pl::PointList &hull, const cgal::Point_2 &p) {
  // O(n) get the point with the min distance
  cgal::Segment_2 seg = {*(hull.begin()), p};
  Iter nearest_point = hull.begin();
  int min = seg.squared_length();

  for (Iter it = hull.begin(); it < hull.end(); ++it) {
    seg = {*it, p};
    if (auto l = seg.squared_length(); l < min) {
      min = l;
      nearest_point = it;
    }
  }

  return nearest_point;
}


std::tuple<Iter, Iter> locateSupportVertices(const pl::PointList &hull, const cgal::Point_2 &p) {
  Iter s_l, s_r;

  Iter nearest_point = calculateNearestPoint(hull, p);

  if (hull.size() == 2) {
    s_l = hull.begin();
    s_r = hull.begin() + 1;
  }

  else if (hull.size() < 5) {
    Iter u = nearest_point;
    Iter v = next(hull, u);

    while (CGAL::left_turn(p, *u, *v)) {
      u = v;
      v = next(hull, u);
    }

    s_r = u;

    u = nearest_point;
    v = prev(hull, u);
    while (CGAL::right_turn(p, *u, *v)) {
      u = v;
      v = prev(hull, u);
    }

    s_l = u;
  }

  // this would be another version to get the support vertices. but it
  // does not work for size < 5 and the version with left_turn and
  // right_turn works also with size > 2 therefore it is not necessary
  // to use both!

  // this is not working. it is possible, that a segment pv_1 does not
  // intersect with the uv also in case that v is the searched point.
  // this is because of the intersection occur outside of uv. it would
  // be possible to use lines instead of segment, but then there is
  // the problem, that a intersection occurs also if the point v is
  // wrong. maybe it is possible to say, that if the intersection
  // occurs before the segment uv then v is the search s_r, and after
  // uv otherwise for s_l. but this is only a tought, with no evidence
  // of truth
  // ATTENTION this version is not tested
  else {
    cgal::Segment_2 pv_1, uv;

    Iter u = nearest_point;
    Iter v = next(hull, u);

    VLOG(3) << "  u: " << *u << " v: " << *v;
    do {
      Iter v_1 = next(hull, v);
      VLOG(3) << "    v_1: " << *v_1;
      s_r = v;
      uv = {*u, *v};
      pv_1 = {p, *v_1};

      // for the next run, if condition evaluates to true
      u = v;
      v = next(hull, u);
      VLOG(3) << "    u: " << *u << " v: " << *v;
    } while (!CGAL::do_intersect(pv_1, uv));
    VLOG(3) << "  s_r: " << *s_r;

    u = nearest_point;
    v = prev(hull, u);
    VLOG(3) << "  u: " << *u << " v: " << *v;
    do {
      Iter v_1 = prev(hull, v);
      VLOG(3) << "    v_1: " << *v_1;
      s_l = v;
      uv = {*u, *v};
      pv_1 = {p, *v_1};

      // for the next run, if condition evaluates to true
      u = v;
      v = prev(hull, u);
      VLOG(3) << "    u: " << *u << " v: " << *v;
    } while (!CGAL::do_intersect(pv_1, uv));
    VLOG(3) << "  s_l: " << *s_l;
  }

  return {s_l, s_r};
}

void recalculateSupportVertices(const pl::PointList &hull, const cgal::Point_2 &p, Iter &s_l, Iter &s_r) {
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

void addToConvexHull(pl::PointList &hull, cgal::Point_2 &p, Iter &s_l, Iter &s_r) {
  VLOG(3) << "    addToConvexHull";

  cgal::Point_2
    s_l_p = *s_l,
    s_r_p = *s_r;

  VLOG(4) << "  s_l_p: " << s_l_p << " s_r_p: " << s_r_p;

  for (auto q : hull)
    VLOG(4) << "      q: " << q;

  // remove points [s_l + 1, s_r)
  hull.erase(s_l + 1, s_r);

  VLOG(4) << "      after erase";
  VLOG(4) << "      s_l: " << *s_l << " s_r: " << *s_r << " p: " << p;
  for (auto q : hull)
    VLOG(4) << "      q: " << q;

  // insert before s_r
  hull.insert(s_r, p);

  VLOG(4) << "      after insert";
  VLOG(4) << "      s_l: " << *s_l << " s_r: " << *s_r << " p: " << p;
  for (auto q : hull)
    VLOG(4) << "      q: " << q;

  // TODO:
  // make that this is not necessary. but for the moment it is easier,
  // because erase and insert above corrupts the iterator a little
  // bit. with little testing the erase should not be a problem, but
  // insert is a problem.
  s_l = std::find_if(hull.begin(), hull.end(), [&](auto q) { return q == s_l_p; });
  s_r = std::find_if(hull.begin(), hull.end(), [&](auto q) { return q == s_r_p; });

  VLOG(4) << "      after woraround with std::find_if";
  VLOG(4) << "      s_l: " << *s_l << " s_r: " << *s_r << " p: " << p;
}

std::vector<cgal::Segment_2> locateVisibleSegments(const pl::PointList &final_list, cgal::Point_2 p, cgal::Point_2 s_l, cgal::Point_2 s_r) {
  VLOG(3) << "locateVisibleSegments";
  std::vector<cgal::Segment_2> segments;

  VLOG(3) << "  s_l: " << s_l << " s_r: " << s_r << " p: " << p;
  for (auto q : final_list)
    VLOG(3) << "    q: " << q;

  // TODO:
  // visible_points maybe there are also necessary points to close
  // this polygon? maybe it is enough to close the polygon with the
  // point p and lines to s_l and s_r? done with initializing of
  // visible_points with {p}

  // TODO:
  // s_l could be the n-1 point and the s_r could be the 0+3 point
  // therefore this is not save for boundary overruns
  pl::PointList visible_points = {p};
  bool within_possible_visible_points = false;
  for (auto point : final_list) {
    if (point == s_l)
      within_possible_visible_points = true;

    if (within_possible_visible_points)
      visible_points.push_back(point);

    if (point == s_r)
      within_possible_visible_points = false;
  }

  VLOG(3) << "  visible_points.size: " << visible_points.size();

  for (size_t i = 1, size = visible_points.size(); i < size; ++i)
    segments.push_back(cgal::Segment_2(visible_points[i-1], visible_points[i]));
  segments.push_back(cgal::Segment_2(visible_points[visible_points.size() - 1], visible_points[0]));
  VLOG(3) << "  segments.size(): " << segments.size();

  using Arrangement_2 = CGAL::Arrangement_2<CGAL::Arr_segment_traits_2<cgal>>;
  Arrangement_2 env;
  CGAL::insert_non_intersecting_curves(env, segments.begin(), segments.end());
  Arrangement_2::Halfedge_const_handle he = env.halfedges_begin();

  VLOG(3) << "  env";

  // TODO: think of it again!
  // https://doc.cgal.org/latest/Visibility_2/Visibility_2_2general_polygon_example_8cpp-example.html
  while (he->target()->point() != p)
    he++;

  VLOG(3) << "  he";

  using TEV = CGAL::Triangular_expansion_visibility_2<Arrangement_2>;
  Arrangement_2 regular_output;
  TEV tev(env);

  VLOG(3) << "  tev";

  Arrangement_2::Face_handle fh = tev.compute_visibility(p, he, regular_output);

  VLOG(3) << "  tev.computed";

  pl::PointList regular_points;

  Arrangement_2::Ccb_halfedge_circulator curr = fh->outer_ccb();
  do {
    regular_points.push_back(curr->source()->point());
  } while (++curr != fh->outer_ccb());

  VLOG(3) << "  regular_points.size(): " << regular_points.size();

  // iterator
  Iter vis = next(visible_points, visible_points.begin());
  Iter reg = next(regular_points, std::find_if(regular_points.begin(), regular_points.end(), [&](auto point){ return p == point; }));
  bool add_mode = (*vis == *reg);
  std::vector<cgal::Segment_2> visible_segments;
  do {
    if (add_mode) {
      vis = next(visible_points, vis);
      reg = next(regular_points, reg);

      add_mode = (*vis == *reg);

      if (add_mode)
        visible_segments.push_back({*vis, *(vis - 1)}); // direction correct?
    }
    else {
      reg = next(regular_points, reg);
      do {
        vis = next(visible_points, vis);
        add_mode = (*vis == *reg);
      } while (!add_mode);
    }
  } while (*vis == p && *reg == p);

  VLOG(3) << "  visible_segments.size(): " << visible_segments.size();

  return visible_segments;
}

cgal::Point_2 chooseRandomSegment(std::vector<cgal::Segment_2> segments) {
  VLOG(3) << "chooseRandomSegment";
  pl::random_selector<> selector{};
  cgal::Segment_2 seg = selector(segments);
  return seg.source();
}

void replaceSegment(pl::PointList &final_list, const cgal::Point_2 &source, const cgal::Point_2 &p) {
  VLOG(3) << "replaceSegment";
  auto iter_source = std::find_if(final_list.begin(), final_list.end(), [&](auto const& point){ return source == point; });
  final_list.insert(iter_source, p);
}

Iter next(const pl::PointList &list, const Iter &it) {
  auto it_n = it + 1;

  if (it_n == list.end())
    it_n = list.begin();

  return it_n;
}

Iter prev(const pl::PointList &list, const Iter &it) {
  Iter it_p;

  if (it == list.begin())
    it_p = list.end() - 1;
  else
    it_p = it - 1;

  return it_p;
}
