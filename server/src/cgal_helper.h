#ifndef CGAL_HELPER_H_
#define CGAL_HELPER_H_

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

using Segments = std::vector<cgal::Segment_2>;
using SegmentsIterator = Segments::iterator;

namespace pl {

Segments::iterator next(Segments &segments, const Segments::iterator &it);
Segments::iterator prev(Segments &segments, const Segments::iterator &it);

}

#endif
