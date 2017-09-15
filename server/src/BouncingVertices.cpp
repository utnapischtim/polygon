#include <tuple>
#include <limits>
#include <cstdlib>
#include <iterator>
#include <vector>
#include <string>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "BouncingVertices.h"
#include "Point.h"
#include "CommonSetting.h"
#include "Filter.h"
#include "random.h"

using cgal = CGAL::Exact_predicates_inexact_constructions_kernel;

using Segments = std::vector<cgal::Segment_2>;

struct ConvexFilter {
  size_t N;
  int point_max = -1;
  int chain_max = -1;
  std::vector<bool> vec;

  ConvexFilter(size_t n, int points, int chains)
    : N(n), point_max(points), chain_max(chains), vec(N, false)
  {}

  bool isSet(unsigned index) const {
    return vec[index];
  }

  void set(unsigned index) {
    vec[index] = true;
  }

  explicit operator bool() const {
    return -1 < point_max || -1 < chain_max;
  }

  // this function could be used, to do that, what it is done in calculateReflexFilter
  // void initPoint(unsigned index) {}
};

struct ReflexFilter {
  size_t N;
  int point_max;
  int chain_max;
  std::vector<bool> vec;
  std::vector<int> indices;

  ReflexFilter(size_t n, int points, int chains)
    : N(n), point_max(points), chain_max(chains), vec(N, false), indices{}
  {}

  bool isSet(unsigned index) const {
    return vec[index];
  }

  void set(unsigned index) {
    vec[index] = true;
    indices.push_back(index);
  }

  explicit operator bool() const {
    return -1 < point_max || -1 < chain_max;
  }

  std::vector<int> getIndices() const {
    return indices;
  }

  // this function could be used, to do that, what it is done in calculateReflexFilter
  // void initPoint(unsigned index) {}
};


static Segments::iterator next(Segments &segments, const Segments::iterator &it);
static Segments::iterator prev(Segments &segments, const Segments::iterator &it);

static std::tuple<pl::SamplingGrid, unsigned, double> init(const pl::CommonSettingList &common_settings);
static Segments init(const pl::PointList &point_list);

static ConvexFilter calculateConvexFilter(const pl::FilterList &filters, size_t N);
static ReflexFilter calculateReflexFilter(const pl::FilterList &filters, size_t N);

static void calculateConvexPoints(ConvexFilter &convex_filter, const ReflexFilter &/*reflex_filter*/);
static void calculateReflexPoints(ReflexFilter &reflex_filter, Segments &segments, const pl::SamplingGrid &sampling_grid, const double radius, const ConvexFilter &convex_filter);

static cgal::Point_2 createPointInsideArea(const pl::SamplingGrid &sampling_grid, const double radius, const Segments::iterator &sit);

static bool isConvex(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, const cgal::Point_2 &shifted_point,
                     Segments &segments, const ConvexFilter &convex_filter, const Segments::iterator &sit, const unsigned k, const size_t N);
static bool isReflex(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, const cgal::Point_2 &shifted_point,
                     Segments &segments, const ReflexFilter &reflex_filter, const Segments::iterator &sit, const unsigned k, const size_t N, const size_t phase);
static bool isIntersection(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, Segments &segments, const Segments::iterator &sit);

pl::PointList pl::bouncingVertices(const pl::PointList &point_list, const pl::CommonSettingList &common_settings, const pl::FilterList &filters) {
  pl::PointList final_list;

  size_t N = point_list.size();

  Segments segments = init(point_list);
  auto [sampling_grid, phases, radius] = init(common_settings);

  ConvexFilter convex_filter = calculateConvexFilter(filters, N);
  ReflexFilter reflex_filter = calculateReflexFilter(filters, N);

  if (convex_filter)
    calculateConvexPoints(convex_filter, reflex_filter);

  if (reflex_filter)
    calculateReflexPoints(reflex_filter, segments, sampling_grid, radius, convex_filter);

  // if there is a chain, then above number list should be overriden

  for (size_t phase = 0; phase < phases; ++phase) {
    // not use next or prev because this would cause a endles loop
    for (auto [sit, k] = std::tuple{segments.begin(), 0u}; sit != segments.end(); ++sit, ++k) {
      bool
        intersection_occur = false,

        // if the convex_filter has a value and the point number in
        // the convex_point_vec is set to true than the convex_flag is
        // set to true, therefore a point has to be found which
        // resolves the requirements and set the convex_flag to false
        // to break the do while loop

        filter_unsatisfied = false;

      cgal::Point_2 shifted_point;

      Segments::iterator sitn = next(segments, sit);

      cgal::Segment_2 e_1, e_2;

      do {
        // reset to false, because point search starts new
        //intersection_occur = false;
        filter_unsatisfied = false;

        shifted_point = createPointInsideArea(sampling_grid, radius, sit);

        e_1 = {sit->source(), shifted_point};
        e_2 = {shifted_point, sitn->target()};

        intersection_occur = isIntersection(e_1, e_2, segments, sit);

        // additional filters
        if (!intersection_occur) {
          if (convex_filter && convex_filter.isSet(k))
            filter_unsatisfied = !isConvex(e_1, e_2, shifted_point, segments, convex_filter, sit, k, N);

          // filter_unsatisfied does not have to be within this
          // condition, because those two values are mutual exclusive.
          if (reflex_filter && reflex_filter.isSet(k))
            filter_unsatisfied = !isReflex(e_1, e_2, shifted_point, segments, reflex_filter, sit, k, N, phase);
        }
      } while (intersection_occur || filter_unsatisfied);

      // set the new segments;
      *sit = e_1;
      *sitn = e_2;
    }
  }

  pl::convert(segments, final_list);
  return final_list;
}

Segments::iterator next(Segments &segments, const Segments::iterator &it) {
  auto it_n = it + 1;

  if (it_n == segments.end())
    it_n = segments.begin();

  return it_n;
}

Segments::iterator prev(Segments &segments, const Segments::iterator &it) {
  Segments::iterator it_p;

  if (it == segments.begin())
    it_p = segments.end() - 1;
  else
    it_p = it - 1;

  return it_p;
}

std::tuple<pl::SamplingGrid, unsigned, double> init(const pl::CommonSettingList &common_settings) {
    pl::CommonSetting c_s_sampling_grid, c_s_phases, c_s_radius;

  if (auto t = pl::find(common_settings, "sampling grid"))
    c_s_sampling_grid = *t;
  else {
    std::string msg = std::string("essential common setting 'sampling grid' not set to generate random pointList");
    throw std::runtime_error(msg);
  }

  if (auto t = pl::find(common_settings, "phases"))
    c_s_phases = *t;
  else {
    std::string msg = std::string("essential common setting 'nodes' not set to generate random pointList");
    throw std::runtime_error(msg);
  }

  if (auto t = pl::find(common_settings, "radius"))
    c_s_radius = *t;
  else
    c_s_radius = pl::CommonSetting("radius", "", 3, "number", "60");

  // TODO:
  // make it robust!
  pl::SamplingGrid sampling_grid(c_s_sampling_grid);
  unsigned phases = std::stoi(c_s_phases.val);
  double radius = std::stod(c_s_radius.val);

  return {sampling_grid, phases, radius};
}

Segments init(const pl::PointList &point_list) {
  Segments segments;

  for (size_t i = 1, size = point_list.size(); i < size; ++i) {
    cgal::Point_2
      source(point_list[i - 1].x, point_list[i - 1].y),
      target(point_list[i].x, point_list[i].y);
    segments.push_back({source, target});
  }

  // close the polygon
  segments.push_back({segments[segments.size() - 1].target(), segments[0].source()});

  return segments;
}

ConvexFilter calculateConvexFilter(const pl::FilterList &filters, size_t N) {
  // -1 means no matter what number of convex points finally appear
  int convex_points = -1;

  if (auto convex_point_filter = pl::find(filters, "convex points"); convex_point_filter)
    convex_points = (*convex_point_filter).val;

  int convex_chains = -1;
  if (auto convex_chain_filter = pl::find(filters, "convex chains"); convex_chain_filter)
    convex_chains = (*convex_chain_filter).val;

  return {N, convex_points, convex_chains};
}

ReflexFilter calculateReflexFilter(const pl::FilterList &filters, size_t N) {
  int reflex_points = -1;
  if (auto reflex_point_filter = pl::find(filters, "reflex points"); reflex_point_filter)
    reflex_points = (*reflex_point_filter).val;

  int reflex_chains = -1;
  if (auto reflex_chain_filter = pl::find(filters, "reflex chains"); reflex_chain_filter)
    reflex_chains = (*reflex_chain_filter).val;

  return {N, reflex_points, reflex_chains};
}

// it is not necessary for the moment, that this function does use
// reflex_filter, because this function get's called after calculateReflexPoints
void calculateConvexPoints(ConvexFilter &convex_filter, const ReflexFilter &/*reflex_filter*/) {
  for (int i = 0; i < convex_filter.point_max; ++i) {
    int random_val;
    do {
      random_val = pl::randomValueOfRange(0, convex_filter.N);
    } while (convex_filter.isSet(random_val));

    convex_filter.set(random_val);
  }
}

// convex_filter is used to be sure, that the reflex points are not
// set on the same index as the convex index. and that the reflex
// points are far enough away.
void calculateReflexPoints(ReflexFilter &reflex_filter, Segments &segments, const pl::SamplingGrid &sampling_grid, const double radius, const ConvexFilter &convex_filter) {
  for (int i = 0; i < reflex_filter.point_max; ++i) {
    int random_val;
    do {
      random_val = pl::randomValueOfRange(0, reflex_filter.N);
    } while (convex_filter.isSet(random_val) || reflex_filter.isSet(random_val));

    reflex_filter.set(random_val);
  }

  auto indices = reflex_filter.getIndices();

  for (int reflex_index : indices) {
    cgal::Segment_2 e_1, e_2;
    cgal::Point_2 shifted_point;
    auto sit = segments.begin() + reflex_index;
    auto sitn = next(segments, sit); // not only add, boundary problems

    do {
      shifted_point = createPointInsideArea(sampling_grid, radius, sit);

      e_1 = {sit->source(), shifted_point};
      e_2 = {shifted_point, sitn->target()};
    } while (isIntersection(e_1, e_2, segments, sit) || !isReflex(e_1, e_2, shifted_point, segments, reflex_filter, sit, reflex_index, reflex_filter.N, 0u));

    *sit = e_1;
    *sitn = e_2;
  }
}



cgal::Point_2 createPointInsideArea(const pl::SamplingGrid &sampling_grid, const double radius, const Segments::iterator &sit) {
  cgal::Point_2 shifted_point;

  do {
    cgal::Point_2 velocity = {pl::randomValueOfRange(-radius, radius), pl::randomValueOfRange(-radius, radius)};

    // check if the shifted_point of the current segment is inside
    // of the boundary box, the shifted_point is also the source
    // of the next segment, therefore the next segment does not
    // have to be checked.
    shifted_point = {sit->target().x() + velocity.x(), sit->target().y() + velocity.y()};
  } while (sampling_grid.isOutOfArea(shifted_point));

  return shifted_point;
}

bool isConvex(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, const cgal::Point_2 &shifted_point, Segments &segments, const ConvexFilter &convex_filter, const Segments::iterator &sit, const unsigned k, const size_t N) {
  bool is_convex = false;

  auto sitn = next(segments, sit);

  bool next_angle_still_convex = true;
  if (int l = (k+1 == N ? 0 : k+1); convex_filter.isSet(l)) {
    auto sitnn = next(segments, sitn);
    next_angle_still_convex = CGAL::angle(shifted_point, sitn->target(), sitnn->target()) == CGAL::OBTUSE && CGAL::right_turn(shifted_point, sitn->target(), sitnn->target());
  }

  bool prev_angle_still_convex = true;
  if (size_t l = (k == 0 ? N : k); next_angle_still_convex && convex_filter.isSet(l-1)) {
    auto sitp = prev(segments, sit);
    prev_angle_still_convex = CGAL::angle(sitp->source(), sitp->target(), shifted_point) == CGAL::OBTUSE && CGAL::right_turn(sitp->source(), sitp->target(), shifted_point);
  }

  // if the current shifted point has to be convex, then it
  // only has to be checked, if the prev(point) is still
  // convex if it has to be!
  if (prev_angle_still_convex && next_angle_still_convex)
    is_convex = CGAL::angle(e_1.source(), shifted_point, e_2.target()) == CGAL::OBTUSE && CGAL::right_turn(e_1.source(), shifted_point, e_2.target());

  // if the prev(point) has to be convex and it is not more,
  // but the current point does not have to be convex, the
  // is_convex would be false, but this is not good,
  // because the current shifted_point has to be reshifted,
  // therefore is_convex has to be set to true manually!
  if ((!prev_angle_still_convex || !next_angle_still_convex) && !is_convex)
    is_convex = true;

  return is_convex;
}

bool isReflex(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, const cgal::Point_2 &shifted_point, Segments &segments, const ReflexFilter &reflex_filter, const Segments::iterator &sit, const unsigned k, const size_t N, const size_t phase) {
  bool is_reflex = false;

  auto sitn = next(segments, sit);

  // not look in front if the phase is equal to 0 because it
  // could not be a reflex point. because the init algorithm
  // is the regular n-gon algorithm
  bool next_angle_still_reflex = true;
  if (int l = (k+1 == N ? 0 : k+1); 0u < phase && reflex_filter.isSet(l)) {
    auto sitnn = next(segments, sitn);
    next_angle_still_reflex = CGAL::angle(shifted_point, sitn->target(), sitnn->target()) == CGAL::ACUTE && CGAL::left_turn(shifted_point, sitn->target(), sitnn->target());
  }

  bool prev_angle_still_reflex = true;
  if (size_t l = (k == 0 ? N : k); next_angle_still_reflex && reflex_filter.isSet(l-1)) {
    auto sitp = prev(segments, sit);
    prev_angle_still_reflex = CGAL::angle(sitp->source(), sitp->target(), shifted_point) == CGAL::ACUTE && CGAL::left_turn(sitp->source(), sitp->target(), shifted_point);
  }

  if (prev_angle_still_reflex && next_angle_still_reflex)
    is_reflex = CGAL::angle(e_1.source(), shifted_point, e_2.target()) == CGAL::ACUTE && CGAL::left_turn(e_1.source(), shifted_point, e_2.target());

  // the same problem
  if ((!prev_angle_still_reflex || !next_angle_still_reflex) && !is_reflex)
    is_reflex = true;

  return is_reflex;
}


// *ssit != old_e_1 && *ssit != old_e_2, the new segments
//   should not be checked about intersection with the old,
//   because per def they should intersect
// ssit->target() != e_1.source() && ssit->source() != e_2.target()
//   the binding points between the segments does not count
//   as intersections. and this construct should work in
//   both directions, clockwise and anti-clockwise
// but all elements which are not e_1 respectively e_2
//   should be checked. because if not a intersection
//   between e_1 respectively e_2 could be overlooked
// ssit->target != e_1.source() is not enough, because e_1
//   could also lie on ssit and this is also not acceptable
//   therefore if those two are connected, than the
//   intersection_occur variable has to be set with the two
//   collinearity checks
// ssit->source != e_2.target the same as with e_1
bool isIntersection(const cgal::Segment_2 &e_1, const cgal::Segment_2 &e_2, Segments &segments, const Segments::iterator &sit) {
  bool intersection_occur = false;

  cgal::Segment_2 old_e_1 = *sit, old_e_2 = *next(segments, sit);

  for (auto ssit = segments.begin(); ssit != segments.end() && !intersection_occur; ++ssit)
    if (*ssit != old_e_1 && *ssit != old_e_2) {
      if (ssit->target() == e_1.source())
        intersection_occur = intersection_occur || CGAL::orientation(e_1.target(), (*ssit).source(), e_1.source()) == CGAL::COLLINEAR || CGAL::orientation((*ssit).source(), e_1.target(), e_1.source()) == CGAL::COLLINEAR;
      else
        intersection_occur = intersection_occur || CGAL::do_intersect(*ssit, e_1);

      if (ssit->source() == e_2.target())
        intersection_occur = intersection_occur || CGAL::orientation(e_2.source(), e_2.target(), (*ssit).target()) == CGAL::COLLINEAR || CGAL::orientation(e_2.source(), (*ssit).target(), e_2.target()) == CGAL::COLLINEAR;
      else
        intersection_occur = intersection_occur || CGAL::do_intersect(*ssit, e_2);
    }

  return intersection_occur;
}
