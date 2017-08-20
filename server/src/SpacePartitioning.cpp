#include <optional>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cstdlib>

#include "easylogging++.h"

#include "CommonSetting.h"
#include "Line.h"
#include "Point.h"
#include "random.h"
#include "SpacePartitioning.h"

static void recursiveDivide(pl::PointList &S, const pl::Point<double> &s_f, const pl::Point<double> &s_l, pl::PointDList &C);
static std::optional<pl::Point<double>> calculateRandomPoint(const pl::PointList &S, const pl::Point<double> &s_f, const pl::Point<double> &s_l);
static std::optional<pl::Line<double>> calculateRandomLine(const pl::Point<double> &s, const pl::Point<double> &s_f, const pl::Point<double> &s_l);
static bool areOnSameSide(const pl::Line<double> &l, const pl::Point<double> &s_f, const pl::Point<double> &s_l);

pl::PointList pl::spacePartitioning(pl::PointList point_list) {
  VLOG(1) << "spacePartitioning";

  pl::PointList final_list;
  pl::random_selector<> selector{};

  pl::Point<double> s_f, s_l;
  do {
    s_f = selector(point_list);
    s_l = selector(point_list);
  } while (s_f == s_l || s_f.x > s_l.x);

  VLOG(1) << "s_f: " << s_f << " s_l: " << s_l;

  pl::Line l{s_f, s_l};

  pl::PointList S_f = {s_f, s_l}, S_l = {};

  for (auto p : point_list)
    if (p < l)
      S_f.push_back(p);
    else
      S_l.push_back(p);


  pl::PointDList C_f = {}, C_l = {};

  VLOG(1) << "left recursiveDivide";
  recursiveDivide(S_f, s_f, s_l, C_f);

  VLOG(1) << "right recursiveDivide";
  recursiveDivide(S_l, s_l, s_f, C_l);

  VLOG(2) << "--------------------";
  VLOG(2) << "C_f";
  for (auto p : C_f)
    VLOG(2) << " " << p;

  VLOG(2) << "C_l";
  for (auto p : C_l)
    VLOG(2) << " " << p;


  C_f.insert(C_f.end(), C_l.begin(), C_l.end());

  for (auto p : C_f)
    final_list.push_back(p);

  final_list.push_back(C_f.front());

  return final_list;
}

void recursiveDivide(pl::PointList &S, const pl::Point<double> &s_f, const pl::Point<double> &s_l, pl::PointDList &C) {

  VLOG(2) << "-------------------------------------";
  VLOG(2) << " size: " << S.size();
  VLOG(2) << " s_f: " << s_f << " s_l: " << s_l;
  VLOG(2) << " S: " << S;

  if (S.size() == 1) {
    VLOG(1) << " size is equal to 1 in recursiveDivide";
    std::exit(1);
  }

  else if (S.size() == 2) {
    C.push_back(s_l);
  }
  else if (S.size() == 3) {
    pl::Point<double> s;
    for (auto p : S)
      if (p != s_f && p != s_l)
        s = p;

    pl::PointList S_f = {s, s_f}, S_l = {s, s_l};
    recursiveDivide(S_f, s_f, s, C);
    recursiveDivide(S_l, s, s_l, C);
  } else {
    pl::random_selector<> selector{};

    auto s = calculateRandomPoint(S, s_f, s_l);

    VLOG(2) << " s: " << s.value();

    auto l = calculateRandomLine(s.value(), s_f, s_l);

    VLOG(2) << " l: " << l.value();

    // the algorithm says to put it in both sets. the for loop does
    // this. if it is done here too, then it is there twice.
    pl::PointList S_f = {s.value()}, S_l = {};

    for (auto p : S)
      if (p < l)
        S_f.push_back(p);
      else
        S_l.push_back(p);


    VLOG(2) << " S_f: " << S_f << ", S_l: " << S_l;

    recursiveDivide(S_f, s_f, s.value(), C);
    recursiveDivide(S_l, s.value(), s_l, C);
  }
}

std::optional<pl::Point<double>> calculateRandomPoint(const pl::PointList &S, const pl::Point<double> &s_f, const pl::Point<double> &s_l) {
  pl::random_selector<> selector{};
  pl::Point<double> s;

  // implemented that way, because of the informations from
  // heuristic_for_generation_of_random_polygons.pdf page 98 second
  // paragraph.
  if (S.size() == 3) {
    auto it = std::find_if(S.begin(), S.end(), [&](auto p) { return p != s_f && p != s_l; });
    s = *it;
    if (pl::isCollinear(s_f, s, s_l))
      return {};

  } else {
    do {
      // TODO:
      // What to do, if there exists only collinear points?
      // the problem is, how is it possible to preserve the random
      // choice and exit if there doesn't exist a point that fullfill
      // all conditions.
      s = selector(S);

    } while (s_f == s || s == s_l || pl::isCollinear(s_f, s, s_l));
  }

  return s;
}

std::optional<pl::Line<double>> calculateRandomLine(const pl::Point<double> &s, const pl::Point<double> &s_f, const pl::Point<double> &s_l) {
  pl::Line<double> l;

  double delta_x = s_l.x - s_f.x;
  double delta_y = s_l.y - s_f.y;

  do {
    // implement heuristic....pdf implementation details about finding line

    double x = 0.0, y = 0.0;
    auto epsilon = std::numeric_limits<double>::epsilon();
    if (std::abs(delta_x) < epsilon) {
      if (std::abs(delta_y) > epsilon)
        y = pl::randomValueOfRange(0.0, delta_y);
    } else {
      x = pl::randomValueOfRange(0.0, delta_x);
      y = (x * delta_y) / delta_x;
    }
    VLOG(3) << "  delta_x: " << delta_x << " delta_y: " << delta_y;
    // what to do if x and y are 0.0?

    // there should be a catchable exception!!
    if (std::abs(x) < epsilon && std::abs(y) < epsilon) {
      VLOG(1) << "  x and y are 0";
      std::exit(1);
    }

    pl::Point s_ = {s_f.x + x, s_f.y + y};
    l = {s, s_};

    VLOG(3) << "  l: " << l << " s_f: " << s_f << " s_l: " << s_l;
  } while (areOnSameSide(l, s_f, s_l));

  if (l < s_f)
    l.flipDirection();

  return l;
}

bool areOnSameSide(const pl::Line<double> &l, const pl::Point<double> &s_f, const pl::Point<double> &s_l) {
  return (s_f < l && s_l < l) || (l < s_f && l < s_l);
}
