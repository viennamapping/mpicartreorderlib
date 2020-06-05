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
#include "Log/Logger.h"

namespace mpireorderinglib {
class Configuration {
private:
  std::string str_alg;
  std::string str_stencil;
  std::string str_node_scheme;
  static bool logSet;
  static const std::string env_alg;
  static const std::string env_stencil;
  static const std::string env_node_scheme;
  static const std::string env_log_level;
  static const std::array<std::unique_ptr<mpireorderinglib::ReorderingScheme>, 3> reorder_schemes;
public:
  Configuration();
  std::string get_str_stencil() const;
  int perform_reordering(MPI_Comm old_comm, const int ndims, const int dims[], const int periods[],
						 const int stencil[], const int n_neighbors, MPI_Comm *cart_comm) const;
};
}

#endif
