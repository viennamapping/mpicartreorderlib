//
// Created by konradvonkirchbach on 5/27/20.
//

#include "Stencil_Strips_Reorderer.h"
int mpireorderinglib::Stencil_Strips_Reorderer::perform_reordering(MPI_Comm old_comm,
												  const int ndims,
												  const int dims[],
												  const int periods[],
												  const int stencil[],
												  const int n_neighbors,
												  MPI_Comm* cart_comm,
												  mpireorderinglib::node_approximation_schemes scheme) {
	return MPIX_Stencil_strips_cart(old_comm, ndims, dims, periods, 0, stencil, n_neighbors, cart_comm, scheme);
}
std::string mpireorderinglib::Stencil_Strips_Reorderer::get_name() {
	return name;
}
mpireorderinglib::Stencil_Strips_Reorderer::Stencil_Strips_Reorderer() : name ("STENCIL_STRIPS"){

}
