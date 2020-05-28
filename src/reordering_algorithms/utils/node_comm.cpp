#include "reordering_algorithms/utils/node_comm.h"

int mpireorderinglib::MPIX_Node_comm (MPI_Comm oldcomm, MPI_Comm* new_comm,
								   int* n_nodes, int* ppn, mpireorderinglib::node_approximation_schemes scheme) {
  MPI_Comm nodecomm, leadercomm;
  MPI_Comm_split_type(oldcomm, MPI_COMM_TYPE_SHARED, 0,
					  MPI_INFO_NULL, &nodecomm);

  int node_rank, leader_rank;
  MPI_Comm_rank(nodecomm, &node_rank);

  int color = MPI_UNDEFINED;
  if (node_rank == 0) {
	color = 0;
  }
  MPI_Comm_split(oldcomm, color, 0, &leadercomm);

  //get number of nodes
  if (node_rank == 0) {
	MPI_Comm_size(leadercomm, n_nodes);
	MPI_Comm_rank(leadercomm, &leader_rank);
  }

  MPI_Bcast(n_nodes, 1, MPI_INT, 0, nodecomm);
  MPI_Bcast(&leader_rank, 1, MPI_INT, 0, nodecomm);

  int node_size, node_sizes[*n_nodes];
  MPI_Comm_size(nodecomm, &node_size);

  //gather the node sizes on leader comm
  if (node_rank == 0) {
	MPI_Allgather(&node_size, 1, MPI_INT,
				  node_sizes, 1, MPI_INT, leadercomm);
  }

  //Broadcast the node sizes along the node
  MPI_Bcast(node_sizes, *n_nodes, MPI_INT,
			0, nodecomm);

  switch (scheme) {
  case mpireorderinglib::NODES_MIN:
	*ppn = node_sizes[0];
	for (int i{1}; i < *n_nodes; i++) {
	  if (node_sizes[i] < *ppn)
		*ppn = node_sizes[i];
	}
	break;
  case mpireorderinglib::NODES_MEAN:
	*ppn = node_sizes[0];
	for (int i{1}; i < *n_nodes; i++) {
	  *ppn += node_sizes[i];
	}
	assert((*n_nodes) != 0);
	(*ppn) /= (*n_nodes);
	break;
  case mpireorderinglib::NODES_MAX:
	*ppn = node_sizes[0];
	for (int i{1}; i < *n_nodes; i++) {
	  if (node_sizes[i] > *ppn)
		*ppn = node_sizes[i];
	}
	break;
  }

  int processes_before_me{0};
  for (int node_id{0}; node_id < leader_rank; node_id++) {
	processes_before_me += node_sizes[node_id];
  }
  int err = MPI_Comm_split(oldcomm, 0, processes_before_me + node_rank,
				 new_comm);
  if (node_rank == 0) {
	MPI_Comm_free(&leadercomm);
  }
  MPI_Comm_free(&nodecomm);
  return err;
}