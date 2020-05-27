#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>
#include <string>
#include <mpi.h>
#include <vector>
#include <memory>

#include "definitions.h"
#include "reordering_algorithms/include_reodering_schemes.h"
#include "Stencils/Stencil_Creater.h"

namespace mpireorderinglib {
class Configuration {
private:
  mpireorderinglib::Stencil_Creater stencil_creater;
  std::string str_alg;
  std::string str_stencil;
  std::string str_node_scheme;
  mpireorderinglib::node_approximation_schemes node_scheme;
  static const std::string env_alg;
  static const std::string env_stencil;
  static const std::string env_node_scheme;
  static const std::vector< std::unique_ptr< ReorderingScheme > > reorder_schemes;
public:
  Configuration();
  mpireorderinglib::reordering_algorithms get_configuration() const;
  mpireorderinglib::node_approximation_schemes get_node_scheme() const;
};

const std::string Configuration::env_alg = "CART_REORDER_ALGORITHM";
const std::string Configuration::env_stencil = "CART_REORDER_STENCIL";
const std::string Configuration::env_node_scheme = "CART_REORDER_NODE_AGGREGATION";
const std::vector< std::unique_ptr< ReorderingScheme > > reorder_schemes {std::make_unique<mpireorderinglib::Hyperplane_Reorderer>(),
														std::make_unique<mpireorderinglib::kd_Tree_Reorderer>(),
														std::make_unique<mpireorderinglib::Stencil_Strips_Reorderer>()};
}

#endif
