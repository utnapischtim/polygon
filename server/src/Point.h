#ifndef POINT_H_
#define POINT_H_

#include <iostream>
#include <limits>
#include <vector>
#include <cmath>
#include <list>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "json.hpp"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

namespace pl {

template<typename T>
struct Point {
  T x;
  T y;

  Point() : x{0}, y{0} {}
  Point(T k, T l) : x{k}, y{l} {}
};

using PointList = std::vector<Point<double>>;
using PointDList = std::list<Point<double>>;

template<typename T>
struct Vector {
  T x;
  T y;

  Point<T> &operator=(const Point<T> &a) {
    x = a.x;
    y = a.y;
    return *this;
  }
};

template<typename T>
struct Triangle {
  Point<T> A;
  Point<T> B;
  Point<T> C;

  bool is_rollback = false;

  Triangle() : A{}, B{}, C{} {}

  // TODO:
  // move semantic? for the moment it should be enough to copy,
  // because to copy 6 integers should not be that bad
  void push(Point<T> point) {
    if (is_rollback) {
      C = point;
      is_rollback = false;
    } else {
      A = B;
      B = C;
      C = point;
    }
  }

  void rollback() {
    is_rollback = true;
  }
};

nlohmann::json to_json(PointList point_list);


template<typename T>
int calculateQuadrant(Vector<T> v) {
  if (0 < v.x && 0 < v.y)
    return 1;
  else if (v.x < 0 && 0 < v.y)
    return 2;
  else if (v.x < 0 && v.y < 0)
    return 3;
  else
    return 4;
}

template<typename T>
double calculateIncludedAngle(const Triangle<T> &triangle) {
  // cos phi_{a,b} = <a,b> / (||a||*||b||) => phi_{a,b} = arccos(<a,b> / (||a||*||b||))
  // a = point - prev_point
  // b = next_point - point

  auto a = triangle.B - triangle.A;
  auto b = triangle.C - triangle.B;
  auto denominator = length(a) * length(b);
  double phi;

  if (denominator == 0)
    phi = 0;
  else
    phi = std::acos((a * b) / denominator);

  return phi;
}

template<class T>
bool operator==(const Point<T> &a, const Point<T> &b) {
  auto diff_x = std::fabs(a.x - b.x);
  auto diff_y = std::fabs(a.y - b.y);
  auto epsilon = std::numeric_limits<T>::epsilon();

  return diff_x < epsilon && diff_y < epsilon;
}

template<class T>
bool operator!=(const Point<T> &a, const Point<T> &b) {
  return !(a == b);
}

template<class T>
bool operator==(const cgal::Point_2 &a, const Point<T> &b) {
  auto diff_x = std::fabs(a.x() - b.x);
  auto diff_y = std::fabs(a.y() - b.y);
  auto epsilon = std::numeric_limits<T>::epsilon();

  return diff_x < epsilon && diff_y < epsilon;
}

template<class T>
bool operator==(const Point<T> &a, const cgal::Point_2 &b) {
  return b == a;
}

template<class T>
bool operator!=(const cgal::Point_2 &a, const Point<T> &b) {
  return !(a == b);
}

template<class T>
bool operator!=(const Point<T> &a, const cgal::Point_2 &b) {
  return !(b == a);
}



template<class T>
Vector<T> operator-(const Point<T> &a, const Point<T> &b) {
  return pl::Vector<T>{a.x - b.x, a.y - b.y};
}

template<class T>
T operator*(const Vector<T> &a, const Vector<T> &b) {
  return a.x * b.x + a.y * b.y;
}

template<class T>
double length(const Vector<T> &a) {
  return std::sqrt(a.x * a.x + a.y * a.y);
}

template<class T>
bool isCollinear(const Point<T> &a, const Point<T> &b, const Point<T> &c) {
  cgal::Point_2 p(a.x, a.y), q(b.x, b.y), r(c.x, c.y);
  return CGAL::collinear(p, q, r);
}

template<class T>
std::ostream &operator<<(std::ostream &out, const Point<T> &p) {
  return out << "(" << p.x << "," << p.y << ")";
}

std::ostream &operator<<(std::ostream &out, const PointList &point_list);

template<class T>
std::vector<T> operator+(const std::vector<T> &v, Point<T> p) {
  auto k = v;
  k.push_back(p);
  return k;
}

template<class T>
std::vector<T> &operator+=(std::vector<T> &v, Point<T> p) {
  v.push_back(p);
  return v;
}

void convert(std::vector<cgal::Point_2> &point_2_list, const PointList &point_list);

}

#endif
