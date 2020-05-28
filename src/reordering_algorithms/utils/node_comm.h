#ifndef NODE_COMM_H
#define NODE_COMM_H

#include<iostream>
#include <mpi.h>
#include <cassert>

#include "definitions.h"

namespace mpireorderinglib {
	int MPIX_Node_comm (MPI_Comm old_comm, MPI_Comm* new_comm,
		int* n_nodes = nullptr, int* ppn = nullptr,
		mpireorderinglib::node_approximation_schemes scheme = mpireorderinglib::node_approximation_schemes::NODES_MEAN);
}

#endif