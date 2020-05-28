//
// Created by konradvonkirchbach on 5/28/20.
//

#ifndef MPIREORDERLIB_SRC_EVALUATION_EVALUATION_METHODS_H
#define MPIREORDERLIB_SRC_EVALUATION_EVALUATION_METHODS_H

#include <mpi.h>
#include <cassert>
#include <vector>

#include "configuration/configuration.h"
#include "Stencils/Stencil_Creater.h"

namespace mpireorderinglib {
void get_neighbors_from_stencil(MPI_Comm cart_comm,
									  const int cart_rank,
									  const int ndims,
									  const int stencil[],
									  const int n_neighbors,
									  const int dims[],
									  const int periods[],
									  std::vector<int> &neighbors);

void MPIX_Dist_graph_internode_cost(MPI_Comm dist_graph_comm,
								   int &total_offnode_neighbors,
								   int &max_offnode_neighbors);

void MPIX_Dist_graph_create_from_cart_comm(MPI_Comm cart_comm, const int ndims,
										const int stencil[],
										const int num_neighbors,
										const int dims[],
										const int periods[],
										MPI_Comm *dist_graph_comm);
}
#endif //MPIREORDERLIB_SRC_EVALUATION_EVALUATION_METHODS_H
