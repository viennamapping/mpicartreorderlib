#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <iostream>
#include <string>

#include "spdlog/spdlog.h"

namespace mpireorderinglib {
enum node_approximation_schemes {
  NODES_MIN,
  NODES_MEAN,
  NODES_MAX
};
mpireorderinglib::node_approximation_schemes string_to_node_approximation_schemes(const std::string &str);
}

#endif
