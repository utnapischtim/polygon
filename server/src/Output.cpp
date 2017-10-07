#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include "json.hpp"
#include "gnuplot-iostream.h"

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

  if (format == "png") {
    std::vector<std::pair<double, double>> image;

    for (auto point : point_list)
      image.push_back({point.x, point.y});

    // '-' using 1:2:(sprintf(\"(%d,%d)\", $1, $2)) with labels notitle,
    // gp.send1d(image); // necessary to have values for the second '-'

    Gnuplot gp;
    gp << "set title 'plot the polygon'\n";
    gp << "set terminal pngcairo \n";
    gp << "set output '" << filename << "'\n";
    gp << "plot '-' with lines\n";
    gp.send1d(image);

  }
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
