//
// Created by markus on 27.01.20.
//

#ifndef CARTESIAN_RANK_REORDERING_KD_TREE_H
#define CARTESIAN_RANK_REORDERING_KD_TREE_H

#include <algorithm>
#include <mpi.h>

#include "reordering_algorithms/utils/node_comm.h"

/**
 * Original c-style function header from Markus.
 * @param old_comm The original (not necessarily cartesian) communicator
 * @param dims The grid's dimensions
 * @param num_dims
 * @param stencil The stencil to skew for
 * @param stencil_size The number of neighbours. e.g. a 5pt-stencil in a 2D grid would have 5 here, not 10.
 * @param periods Not really used algorithmically, only for constructing the cartesian communicator
 * @param cart_comm The resulting, optimized communicator
 */
int
MPIX_skewed_kdtree_cart_create(MPI_Comm old_comm, const int *dims, int num_dims, const int *stencil, int stencil_size,
                               const int * const periods, MPI_Comm *cart_comm);

#endif //CARTESIAN_RANK_REORDERING_KD_TREE_H
