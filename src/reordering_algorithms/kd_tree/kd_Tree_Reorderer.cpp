//
// Created by konradvonkirchbach on 5/27/20.
//

#include "kd_Tree_Reorderer.h"
void mpireorderinglib::kd_Tree_Reorderer::perform_reordering(MPI_Comm old_comm,
										   int ndims,
										   const int *dims,
										   const int *periods,
										   const int *stencil,
										   const int n_neighbors,
										   MPI_Comm *cart_comm,
										   mpireorderinglib::node_approximation_schemes scheme) {
	MPIX_skewed_kdtree_cart_create(old_comm, dims, ndims, stencil, n_neighbors, periods, cart_comm);
}
std::string mpireorderinglib::kd_Tree_Reorderer::get_name() {
  return name;
}
mpireorderinglib::kd_Tree_Reorderer::kd_Tree_Reorderer() : name ("KD_TREE"){

}
