//
// Created by konradvonkirchbach on 5/27/20.
//

#ifndef MPIREORDERLIB_SRC_REORDERING_ALGORITHMS_ABSTRACT_REORDERING_SCHEME_H
#define MPIREORDERLIB_SRC_REORDERING_ALGORITHMS_ABSTRACT_REORDERING_SCHEME_H

#include <string>
#include <mpi.h>

#include "definitions.h"

namespace mpireorderinglib {
class ReorderingScheme {
public:
  virtual ~ReorderingScheme() = 0;
  virtual std::string get_name() = 0;
  virtual int perform_reordering(MPI_Comm old_comm, int ndims, const int dims[],
								  const int periods[], const int stencil[], const int n_neighbors,
								  MPI_Comm *cart_comm, mpireorderinglib::node_approximation_schemes scheme) = 0;
};
}

#endif //MPIREORDERLIB_SRC_REORDERING_ALGORITHMS_ABSTRACT_REORDERING_SCHEME_H
