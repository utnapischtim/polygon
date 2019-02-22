#ifndef CALCULATEBOUNCINGRANGE_H_
#define CALCULATEBOUNCINGRANGE_H_

#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "BouncingVerticesSettings.h"
#include "Filter.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

namespace pl {

using Segments = std::vector<cgal::Segment_2>;
using SegmentsIterator = Segments::iterator;


struct CalculateBouncingRange {
  const BouncingVerticesSettings &bvs;
  Segments &segments;
  cgal::Point_2 bouncing_point;
  cgal::Line_2 random_line;
  cgal::Point_2 before_point;
  cgal::Point_2 after_point;
  cgal::Segment_2 prev_segment;
  cgal::Segment_2 next_segment;

  CalculateBouncingRange(const BouncingVerticesSettings &b, Segments &s) : bvs(b), segments(s), bouncing_point{}, random_line{}, before_point{}, after_point{}, prev_segment{}, next_segment{} {}

  void bounce(SegmentsIterator &sit);
  cgal::Point_2 calculateShiftedPoint(const cgal::Segment_2 &allowed_segment);
  cgal::Segment_2 calculateBouncingInterval(SegmentsIterator &sit);
  double getRandomAngleToBounce([[maybe_unused]] Segments::iterator &sit);
  cgal::Line_2 calculateRandomLine(const double angle_in_radian);

  cgal::Segment_2 calculateIntersectionFreeRange();
  cgal::Segment_2 calculateOrientationStability();
  cgal::Segment_2 calculateAllowedMovingSegmentByAngleRange(const cgal::Segment_2 &seg, const pl::Filter range);
  cgal::Segment_2 calculatePreservedAngleRangeAroundBouncingPoint();
  cgal::Segment_2 calculatePreservedAngleRangeInBouncingPoint();
  cgal::Segment_2 blend(const cgal::Segment_2 &allowed_seg_prev, const cgal::Segment_2 &allowed_seg_next);
  cgal::Segment_2 calculateSmallestBouncingInterval(const Segments &allowed_segments);

  cgal::Point_2 calculateIntersectionWithRandomLine(const cgal::Segment_2 &seg, const double angle_in_radian);
  cgal::Point_2 calculatePointWithAngle(const cgal::Point_2 &A, const cgal::Point_2 &B, const cgal::Line_2 &bisecting_line, const double angle);


  /**
   * PURE FUNCTION
   **/
  cgal::Line_2 rotate(const cgal::Point_2 &A, const cgal::Point_2 &B, const double angle_in_radian);
  cgal::Line_2 calculateBisectingLine(const cgal::Point_2 &A, const cgal::Point_2 &B);
};

}

#endif
