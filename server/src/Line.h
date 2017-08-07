#ifndef LINE_H_
#define LINE_H_

#include <iostream>
#include <cmath>

#include "Point.h"

namespace pl {

template<typename T>
struct Line {
  Point<T> p1;
  Point<T> p2;

  Line() : p1{}, p2{} {}

  Line(Point<T> k1, Point<T> k2)
    : p1{k1}, p2{k2} {}

  void flipDirection() {
    auto p = p1;
    p1 = p2;
    p2 = p;
  }
};

template<typename T>
bool operator<(Point<T> p, Line<T> l) {
  auto d = calculateDistance(p, l);
  return d < 0; // could be possible that it is wrong
}

template<typename T>
bool operator<=(Point<T> p, Line<T> l) {
  auto d = calculateDistance(p, l);
  auto epsilon = std::numeric_limits<double>::epsilon();
  return d < 0 || d < epsilon;
}

template<typename T>
bool operator<(Line<T> l, Point<T>p) {
  auto d = calculateDistance(p, l);
  return d > 0; // could be possible that it is wrong
}

template<typename T>
bool operator==(Line<T> l, Point<T>p) {
  auto d = calculateDistance(p, l);
  auto epsilon = std::numeric_limits<T>::epsilon();

  return d < epsilon;
}

template<typename T>
T calculateDistance(Point<T> p, Line<T> l) {
  return (p.x - l.p1.x)*(l.p2.y - l.p1.y) - (p.y - l.p1.y)*(l.p2.x - l.p1.x);
}

template<typename T>
std::ostream &operator<<(std::ostream &out, const Line<T> &line) {
  return out << line.p1 << "--" << line.p2;
}

}

#endif
