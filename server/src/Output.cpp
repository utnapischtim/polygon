#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <experimental/filesystem>

#include <Magick++.h>

#include "json.hpp"
#include "gnuplot-iostream.h"

#include "Output.h"
#include "Point.h"
#include "CommonSetting.h"

namespace fs = std::experimental::filesystem;

void pl::output(pl::PointList point_list, std::string format, std::string filename, pl::SamplingGrid sampling_grid, int phase) {
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
    gp << "set terminal pngcairo size 1400,1000\n";

    if (0 < sampling_grid.width && 0 < sampling_grid.height) {
      gp << "set xrange [0:" << std::to_string(sampling_grid.width) << "]\n";
      gp << "set yrange [0:" << std::to_string(sampling_grid.height) << "]\n";
    }

    gp << "set output '" << filename << "'\n";
    gp << "plot '-'" + (0 < phase ? " title '" + std::to_string(phase) + "'" : "") + " with lines\n";

    gp.send1d(image);
  }

  if (format == "animation") {
    std::vector<Magick::Image> frames;
    std::vector<fs::path> pngs;

    for (auto png : fs::directory_iterator("out/animation"))
      pngs.push_back(png.path());

    std::sort(pngs.begin(), pngs.end(), [](auto a, auto b) {
        return std::stoi(a.filename()) < std::stoi(b.filename());
      });

    for (auto png : pngs) {
      Magick::Image img;
      img.read(png.string());
      img.animationDelay(100);
      frames.push_back(img);
      fs::remove(png.string());
    }

    Magick::writeImages(frames.begin(), frames.end(), filename);
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
