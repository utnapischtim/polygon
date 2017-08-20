#ifndef RANDOM_H_
#define RANDOM_H_

#include <random>
#include <iterator>

#include "CommonSetting.h"
#include "Point.h"
#include "Filter.h"

namespace pl {

// https://gist.github.com/cbsmith/5538174
template<typename RandomGenerator = std::default_random_engine>
struct random_selector {
  random_selector(RandomGenerator g = RandomGenerator(std::random_device()()))
    : gen(g) {}

  template<typename Iter>
  Iter select(Iter start, Iter end) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(gen));
    return start;
  }

  template<typename Iter>
  Iter operator()(Iter start, Iter end) {
    return select(start, end);
  }

  template<typename Container>
  auto operator()(const Container &c) -> decltype(*begin(c)) & {
    return *select(begin(c), end(c));
  }

private:
  RandomGenerator gen;
};

double randomValueOfRange(double start, double end);
int randomValueOfRange(int start, int end);
unsigned randomValueOfRange(unsigned start, unsigned end);
PointList random(CommonSettingList common_settings, FilterList local_filters);

}

#endif
