//
// Created by konradvonkirchbach on 5/27/20.
//

#ifndef MPIREORDERLIB_SRC_REORDERING_ALGORITHMS_STENCIL_STRIPS_STENCIL_STRIPS_REORDERER_H
#define MPIREORDERLIB_SRC_REORDERING_ALGORITHMS_STENCIL_STRIPS_STENCIL_STRIPS_REORDERER_H

#include "reordering_algorithms/abstract_reordering_scheme.h"
#include "reordering_algorithms/stencil_strips/stencil_strips.h"

namespace mpireorderinglib {
class Stencil_Strips_Reorderer : public mpireorderinglib::ReorderingScheme {
private:
  std::string name;
public:
  Stencil_Strips_Reorderer();
  virtual std::string get_name();
  void perform_reordering(MPI_Comm old_comm, const int ndims, const int dims[], const int periods[],
								  const int stencil[], const int n_neighbors, MPI_Comm *cart_comm,
								  mpireorderinglib::node_approximation_schemes scheme = mpireorderinglib::node_approximation_schemes::NODES_MEAN);
};
}

#endif //MPIREORDERLIB_SRC_REORDERING_ALGORITHMS_STENCIL_STRIPS_STENCIL_STRIPS_REORDERER_H
