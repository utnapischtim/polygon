#ifndef STATISTICS_H_
#define STATISTICS_H_

#include <chrono>

#define TIME_START Time start = std::chrono::high_resolution_clock::now();
#define TIME_END Time end = std::chrono::high_resolution_clock::now();
#define TIME_PARSE pl::parseTime(start, end);

using Time = std::chrono::time_point<std::chrono::system_clock>;
using min = std::chrono::minutes;
using sec = std::chrono::seconds;
using ms = std::chrono::milliseconds;
using mc = std::chrono::microseconds;

namespace pl {

void parseTime(Time start, Time end);

}

#endif
