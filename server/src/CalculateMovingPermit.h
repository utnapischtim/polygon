#ifndef CALCULATEMOVINGPERMIT_H_
#define CALCULATEMOVINGPERMIT_H_

#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "BouncingVerticesSettings.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

namespace pl {

using Segments = std::vector<cgal::Segment_2>;
using SegmentsIterator = Segments::iterator;

struct CalculateMovingPermit {
  const BouncingVerticesSettings &bvs;
  Segments &segments;

  CalculateMovingPermit(const BouncingVerticesSettings &b, Segments &s) : bvs(b), segments(s) {}
  void bounce(SegmentsIterator &sit);

  cgal::Point_2 createPointInsideArea(const Segments::iterator &sit);
  bool isIntersection(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, const Segments::iterator &sit);
  bool insideAngleRange(const cgal::Segment_2 &e_1_new, const cgal::Segment_2 &e_2_new, const cgal::Segment_2 &e_1_old, const cgal::Segment_2 &e_2_old, const cgal::Segment_2 &next, const cgal::Segment_2 &prev);

  /**
   * PURE FUNCTION
   **/
  bool insideOrientationArea(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, const cgal::Segment_2 &e_1_old, const cgal::Segment_2 &e_2_old, const cgal::Segment_2 &next, const cgal::Segment_2 &prev);
  double calculateAngle(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2);
};

}

#endif
