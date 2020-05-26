#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>
#include <string>
#include <mpi.h>

#include "definitions.h"
#include "reordering_algorithms/hyperplane/hyperplane.h"
#include "reordering_algorithms/kd_tree/kd_tree.h"
#include "reordering_algorithms/stencil_strips/stencil_strips.h"

namespace mpireorderinglib {
class Configuration {
private:
  mpireorderinglib::reordering_algorithms alg;
  mpireorderinglib::node_approximation_schemes node_scheme;
  static const std::string env_flag;
public:
  Configuration();
  mpireorderinglib::reordering_algorithms get_configuration() const;
  mpireorderinglib::node_approximation_schemes get_node_scheme() const;
};

const std::string Configuration::env_flag = "CART_REORDER_ALG";
}

#endif
