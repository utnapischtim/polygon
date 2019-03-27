#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "cgal_helper.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

Segments::iterator pl::next(Segments &segments, const Segments::iterator &it) {
  auto it_n = it + 1;

  if (it_n == segments.end())
    it_n = segments.begin();

  return it_n;
}

Segments::iterator pl::prev(Segments &segments, const Segments::iterator &it) {
  Segments::iterator it_p;

  if (it == segments.begin())
    it_p = segments.end() - 1;
  else
    it_p = it - 1;

  return it_p;
}
