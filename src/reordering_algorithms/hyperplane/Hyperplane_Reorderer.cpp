//
// Created by konradvonkirchbach on 5/27/20.
//

#include "Hyperplane_Reorderer.h"

std::string mpireorderinglib::Hyperplane_Reorderer::get_name() {
  return name;
}
int mpireorderinglib::Hyperplane_Reorderer::perform_reordering(MPI_Comm old_comm,
											  int ndims,
											  const int *dims,
											  const int *periods,
											  const int *stencil,
											  const int n_neighbors,
											  MPI_Comm *cart_comm,
											  mpireorderinglib::node_approximation_schemes scheme) {
  	return MPIX_Hyperplane_comm(old_comm, ndims, dims, periods, stencil, n_neighbors, cart_comm, scheme);
}
mpireorderinglib::Hyperplane_Reorderer::Hyperplane_Reorderer() : name("HYPERPLANE"){

}
