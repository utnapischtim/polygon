#include <algorithm>
#include <vector>
#include <tuple>
#include <map>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangle_2.h>

#include "TwoOptMoves.h"
#include "Point.h"
#include "random.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

using Intersections = std::map<std::string, std::tuple<cgal::Segment_2, cgal::Segment_2>>;

static Intersections calculateIntersections(const std::vector<cgal::Segment_2> &segments);
static void resolveIntersections(std::vector<cgal::Segment_2> &segments, Intersections &intersections);
static pl::PointList calculateFinalList(std::vector<cgal::Segment_2> &segments);

pl::PointList pl::twoOptMoves(const pl::PointList &point_list) {
  pl::PointList final_list;

  std::vector<cgal::Segment_2> segments;
  pl::convert(point_list, segments);

  // O(n^2*log(n))
  auto intersections = calculateIntersections(segments);

  // O(n^2*log(n))
  resolveIntersections(segments, intersections);

  // O(n*log(n))
  final_list = calculateFinalList(segments);

  return final_list;
}

std::string to_key(const cgal::Segment_2 &s_1, const cgal::Segment_2 &s_2) {
  return pl::to_string(s_1.source()) + '-' + pl::to_string(s_1.target()) + '-' + pl::to_string(s_2.source()) + '-' + pl::to_string(s_2.target());
}

bool isIntersection(auto s_1, auto s_2) {
  return s_1 != s_2 &&
         s_1.source() != s_2.target() && s_1.target() != s_2.source() && s_1.source() != s_2.source() && s_1.target() != s_2.target() &&
         CGAL::do_intersect(s_1, s_2);
}

bool isUnique(auto intersections, auto s_1, auto s_2) {
  return intersections.find(to_key(s_2, s_1)) == intersections.end();
}

Intersections calculateIntersections(const std::vector<cgal::Segment_2> &segments) {
  Intersections intersections;
  // O(n^2*log(n))
  for (auto s_1 : segments) // O(n)
    for (auto s_2 : segments) // O(n)
      if (isIntersection(s_1, s_2) && isUnique(intersections, s_1, s_2)) // O(1) + O(log(n))
        intersections[to_key(s_1, s_2)] = {s_1, s_2}; // O(log(n))

  return intersections;
}

std::tuple<cgal::Segment_2, cgal::Segment_2> calculateNewSegments(std::vector<cgal::Segment_2> &segments, const cgal::Segment_2 &f_1, const cgal::Segment_2 &f_2) {
  cgal::Segment_2 e_1(f_1.source(), f_2.source()), e_2(f_1.target(), f_2.target());

  segments.push_back(e_1);
  segments.push_back(e_2);

  // O(n*log(n))
  pl::PointList list = calculateFinalList(segments);

  if (list.size() != segments.size()) {
    segments.pop_back();
    segments.pop_back();

    e_1 = {f_1.source(), f_2.target()};
    e_2 = {f_1.target(), f_2.source()};

    segments.push_back(e_1);
    segments.push_back(e_2);

    list = calculateFinalList(segments);

    if (list.size() != segments.size())
      throw std::runtime_error("ui big problem");
  }

  // it might be possible to add the new segments within this function
  // BUT by the new intersection calculation should considered about
  // this change!
  segments.pop_back();
  segments.pop_back();

  return {e_1, e_2};
}

void resolveIntersections(std::vector<cgal::Segment_2> &segments, Intersections &intersections) {
  pl::random_selector<> selector{};

    // O(n^2*log(n))
  for (; 0 < intersections.size();) { // O(n)
    auto [f_1, f_2] = selector(intersections).second;

    // remove the intersections that have f1 or f2 same as the choosen
    // current intersection
    auto condition_to_erase = [&](auto intersection) {
      return f_1 == std::get<0>(intersection) || f_2 == std::get<0>(intersection) ||
             f_1 == std::get<1>(intersection) || f_2 == std::get<1>(intersection);
    };

    // O(nlog(n))
    for (auto it = intersections.begin(); it != intersections.end();)
      if (condition_to_erase(it->second))
        it = intersections.erase(it); // O(log(n))
      else
        it++;

    // O(n)
    auto it_f_1 = std::find_if(segments.begin(), segments.end(), [&](auto seg){ return seg == f_1; });
    segments.erase(it_f_1);

    // O(n)
    auto it_f_2 = std::find_if(segments.begin(), segments.end(), [&](auto seg){ return seg == f_2; });
    segments.erase(it_f_2);

    // O(n*log(n))
    // replace f_1 with e_1 and f_2 with e_2
    auto [e_1, e_2] = calculateNewSegments(segments, f_1, f_2);

    // O(n*log(n))
    for (auto s : segments) { // O(n)
      if (isIntersection(s, e_1) && isUnique(intersections, s, e_1)) // O(1) + O(log(n))
        intersections[to_key(s, e_1)] = {s, e_1}; // O(log(n))

      if (isIntersection(s, e_2) && isUnique(intersections, s, e_2)) // O(1) + O(log(n))
        intersections[to_key(s, e_2)] = {s, e_2}; // O(log(n))
    }

    // do it after to the segments intersection check to not confuse it
    segments.insert(it_f_1, e_1);
    segments.insert(it_f_2, e_2);
  }
}

pl::PointList calculateFinalList(std::vector<cgal::Segment_2> &segments) {

  std::map<std::string, cgal::Point_2> look_1, look_2;

  // O(n*log(n))
  for (const auto &s : segments) { // O(n)
    std::string
      key_source = pl::to_string(s.source()),
      key_target = pl::to_string(s.target());

    if (look_1.find(key_source) == look_1.end())  // O(log(n))
      look_1[key_source] = s.target();
    else if (look_2.find(key_source) == look_2.end()) // O(log(n))
      look_2[key_source] = s.target();
    else
      throw std::runtime_error("blob");

    if (look_1.find(key_target) == look_1.end()) // O(log(n))
      look_1[key_target] = s.source();
    else if (look_2.find(key_target) == look_2.end()) // O(log(n))
      look_2[key_target] = s.source();
    else
      throw std::runtime_error("blob");
  }

  cgal::Point_2
    source = segments[0].source(),
    target = segments[0].target();

  pl::PointList final_list = {source};

  // O(n*log(n))
  for (size_t i = 0, size = segments.size(); i < size; ++i) { // O(n)
    final_list.push_back(target);

    std::string key = pl::to_string(target);

    if (look_1[key] != source) { // O(log(n))
      source = target;
      target = look_1[key];
    } else if (look_2[key] != source) { // O(log(n))
      source = target;
      target = look_2[key];
    } else
      throw std::runtime_error("not working");
  }

  return final_list;
}
