#include <chrono>
#include <cstdio>
#include <iomanip>
#include <ctime>
#include <iostream>

#include "Statistics.h"

void pl::parseTime(Time start, Time end) {
  auto diff = end - start;

  int
    m = std::chrono::duration_cast<min>(diff).count(),
    s = std::chrono::duration_cast<sec>(diff).count() - m * 60,
    mil = std::chrono::duration_cast<ms>(diff).count() - (s * 1000 + m * 1000 * 60),
    mic = std::chrono::duration_cast<mc>(diff).count() - (s * 1000000 + mil * 1000 + m * 1000000 * 60);

  auto start_time_t = std::chrono::system_clock::to_time_t(start);
  auto end_time_t = std::chrono::system_clock::to_time_t(end);

  std::cout << "start time: " << std::put_time(std::localtime(&start_time_t), "%c %Z") << "\n";
  std::cout << "end   time: " << std::put_time(std::localtime(&end_time_t), "%c %Z") << "\n";

  std::printf("diff  time: %02d::%02d::%03d::%03d\n", m, s, mil, mic);
}
