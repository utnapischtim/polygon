#include <algorithm>
#include <limits>
#include <cmath>
#include <cstdlib>
#include <list>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "CommonSetting.h"
#include "Point.h"
#include "random.h"
#include "SpacePartitioning.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

static void recursiveDivide(pl::PointList &S, const cgal::Point_2 &s_f, const cgal::Point_2 &s_l, pl::PointList &C);

pl::PointList pl::spacePartitioning(pl::PointList point_list) {
  pl::PointList final_list;
  pl::random_selector<> selector{};

  // find two different points, where s_f lies on the x axis on the
  // left position of s_l
  cgal::Point_2 s_f, s_l;
  do {
    s_f = selector(point_list);
    s_l = selector(point_list);
  } while (s_f == s_l || s_f.x() > s_l.x());

  cgal::Line_2 l{s_f, s_l};

  pl::PointList S_f = {s_f, s_l}, S_l = {s_l, s_f};

  for (auto p : point_list)
    // p < l
    if (p != s_f && p != s_l && l.has_on_positive_side(p))
      S_f.push_back(p);
    else if (p != s_f && p != s_l && l.has_on_negative_side(p))
      S_l.push_back(p);
    else if (p != s_f && p != s_l && CGAL::collinear(s_l, s_f, p)) {
      // TODO:
      // collinear points
    }

  recursiveDivide(S_f, s_f, s_l, final_list);
  recursiveDivide(S_l, s_l, s_f, final_list);

  final_list.push_back(final_list[0]);

  return final_list;
}



cgal::Point_2 calculateRandomPoint(const pl::PointList &S, const cgal::Point_2 &s_f, const cgal::Point_2 &s_l) {
  pl::random_selector<> selector{};
  cgal::Point_2 s;

  do {
    // TODO:
    // What to do, if there exists only collinear points?
    // the problem is, how is it possible to preserve the random
    // choice and exit if there doesn't exist a point that fullfill
    // all conditions.
    s = selector(S);

  } while (s_f == s || s == s_l || CGAL::collinear(s_f, s, s_l));

  return s;
}

bool areOnSameSide(const cgal::Line_2 &l, const cgal::Point_2 &s_f, const cgal::Point_2 &s_l) {
  // (s_f < l && s_l < l) || (l < s_f && l < s_l);
  return (l.has_on_positive_side(s_f) && l.has_on_positive_side(s_l)) || (l.has_on_negative_side(s_f) && l.has_on_negative_side(s_l));
}

cgal::Line_2 calculateRandomLine(const cgal::Point_2 &s, const cgal::Point_2 &s_f, const cgal::Point_2 &s_l) {
  cgal::Line_2 l;

  double
    delta_x = s_l.x() - s_f.x(),
    delta_y = s_l.y() - s_f.y();

  do {
    // implement heuristic....pdf implementation details about finding line

    double x = 0.0, y = 0.0;
    auto epsilon = std::numeric_limits<double>::epsilon();
    if (std::abs(delta_x) < epsilon) {
      if (std::abs(delta_y) > epsilon)
        y = pl::randomValueOfRange(0.0, delta_y);
    }
    else {
      x = pl::randomValueOfRange(0.0, delta_x);
      y = (x * delta_y) / delta_x;
    }

    cgal::Point_2 s_ = {s_f.x() + x, s_f.y() + y};

    l = {s, s_};
  } while (areOnSameSide(l, s_f, s_l));

  // l < s_f
  if (l.has_on_negative_side(s_f))
    l = l.opposite();

  return l;
}

void recursiveDivide(pl::PointList &S, const cgal::Point_2 &s_f, const cgal::Point_2 &s_l, pl::PointList &C) {
  if (S.size() == 2) {
    C.push_back(s_l);
  }
  else if (S.size() == 3) {
    cgal::Point_2 s;
    for (auto p : S)
      if (p != s_f && p != s_l)
        s = p;

    C.push_back(s);
    C.push_back(s_l);
  }
  else {
    cgal::Point_2 s = calculateRandomPoint(S, s_f, s_l);
    cgal::Line_2 l = calculateRandomLine(s, s_f, s_l);

    pl::PointList S_f = {s}, S_l = {s};

    for (auto p : S)
      // p < l
      if (p != s && l.has_on_positive_side(p))
        S_f.push_back(p);
      else if (p != s && l.has_on_negative_side(p))
        S_l.push_back(p);
      else {
        // TODO
        // not handled points are collinear with the line l
      }

    recursiveDivide(S_f, s_f, s, C);
    recursiveDivide(S_l, s, s_l, C);
  }
}
