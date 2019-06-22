#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <vector>
#include <string>

#include <json.hpp>
#include <docopt.h>

#include "Point.h"

namespace pl {


struct Generator {
  std::string name;
  std::string desc;
  int key;

  Generator()
    : name{}, desc{}, key{}
    {}

  Generator(std::string n, std::string d, int k)
    : name(n), desc(d), key(k)
    {}
  Generator(nlohmann::json obj)
    : Generator(obj["name"], obj["desc"], obj["key"])
    {}
  Generator(int k)
    : Generator("", "", k)
    {}
};


nlohmann::json getListOfGenerators();
void printGenerators();
Generator createGenerator(nlohmann::json obj);
Generator createGenerator(int key);

PointList generatePointList(pl::Generator generator, const std::map<std::string, docopt::value> &args);

}

#endif
