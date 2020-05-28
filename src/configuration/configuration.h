#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>
#include <string>
#include <mpi.h>
#include <vector>
#include <array>
#include <memory>
#include <utility>

#include "definitions.h"
#include "reordering_algorithms/include_reodering_schemes.h"
#include "Stencils/Stencil_Creater.h"
#include "spdlog/spdlog.h"

namespace mpireorderinglib {
class Configuration {
private:
  std::string str_alg;
  std::string str_stencil;
  std::string str_node_scheme;
  static const std::string env_alg;
  static const std::string env_stencil;
  static const std::string env_node_scheme;
  static const std::array<std::unique_ptr<mpireorderinglib::ReorderingScheme>, 3> reorder_schemes;
public:
  Configuration();
  std::string get_str_stencil() const;
  int perform_reordering(MPI_Comm old_comm, const int ndims, const int dims[], const int periods[],
						 const int stencil[], const int n_neighbors, MPI_Comm *cart_comm) const;
};

const std::string Configuration::env_alg = "CART_REORDER_ALGORITHM";
const std::string Configuration::env_stencil = "CART_REORDER_STENCIL";
const std::string Configuration::env_node_scheme = "CART_REORDER_NODE_AGGREGATION";
const std::array<std::unique_ptr<mpireorderinglib::ReorderingScheme>, 3>
	reorder_schemes{std::make_unique<mpireorderinglib::Hyperplane_Reorderer>(),
					std::make_unique<mpireorderinglib::kd_Tree_Reorderer>(),
					std::make_unique<mpireorderinglib::Stencil_Strips_Reorderer>()};
}

#endif
