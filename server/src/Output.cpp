#include <iostream>
#include <fstream>
#include <string>

#include "json.hpp"

#include "Output.h"
#include "Point.h"

void pl::output(pl::PointList point_list, std::string format, std::string filename) {
  std::ofstream file;

  if (filename != "STDOUT")
    file.open(filename, std::ios::out);

  std::ostream &out = filename == "STDOUT" ? std::cout : file;

  if (format == "gnuplot")
    for (auto point : point_list)
      out << point.x << " " << point.y << "\n";
}

void pl::printOutputFormats() {
  nlohmann::json formats = {
    {{"name", "gnuplot"}}
  };

  std::cout << "list of output formats" << "\n";
  std::cout << "name" << "\n";

  for (auto format : formats)
    std::cout << format["name"].get<std::string>() << "\n";
}
