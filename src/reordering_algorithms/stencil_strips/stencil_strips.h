//
// Created by konradvonkirchbach on 2/25/20.
//

#ifndef CARTESIAN_RANK_REORDERING_STENCIL_STRIPS_H
#define CARTESIAN_RANK_REORDERING_STENCIL_STRIPS_H

#include <iostream>
#include <mpi.h>
#include <cmath>

#include "definitions.h"
#include "reordering_algorithms/utils/node_comm.h"
#include "reordering_algorithms/utils/utils.h"

int MPIX_Stencil_strips_cart(MPI_Comm oldcomm,
        const int ndims,
        const int dims[],
        const int periods[],
        const int reorder,
        const int stencil[],
        const int n_neighbors,
        MPI_Comm* cartcomm,
        mpireorderinglib::node_approximation_schemes scheme =
        	mpireorderinglib::node_approximation_schemes::NODES_MEAN);

#endif //CARTESIAN_RANK_REORDERING_STENCIL_STRIPS_H
