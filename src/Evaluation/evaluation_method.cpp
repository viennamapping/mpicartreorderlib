//
// Created by konradvonkirchbach on 5/28/20.
//

#include <algorithm>
#include "Evaluation/evaluation_methods.h"

void mpireorderinglib::get_neighbors_from_stencil(MPI_Comm cart_comm,
												  const int cart_rank,
												  const int ndims,
												  const int stencil[],
												  const int n_neighbors,
												  const int dims[],
												  const int periods[],
												  std::vector<int> &neighbors) {

  int rank_coord[ndims];
  MPI_Cart_coords(cart_comm, cart_rank, ndims, rank_coord);

  //Every process will get himself as a neighbor
  //in order to prevent crash, if it has no other
  //neighbors
  int possible_neighbors[n_neighbors];

  for (int i{0}; i < n_neighbors; i++)
	possible_neighbors[i] = -2;
  int index = -1;
  bool periodic_checker;

  for (int i{0}; i < n_neighbors * ndims; i += ndims) {
	index++;
	//extract the neighbors
	int neighbor_coord[ndims];
	periodic_checker = true;

	//get neighbor coordinate
	for (int n{0}; n < ndims; n++) {
	  neighbor_coord[n] = stencil[i + n] + rank_coord[n];
	}

	/* In non periodic case, check if the coordinates of
	 * relativ neighbor is feasbile, to prevent errors in
	 * MPI_Cart_rank
	 */
	for (int n{0}; n < ndims; n++) {
	  if (!periods[n] && (neighbor_coord[n] < 0 ||
		  neighbor_coord[n] > dims[n] - 1)) {
		periodic_checker = false;
		break;
	  }
	}

	if (!periodic_checker)
	  continue;

	MPI_Cart_rank(cart_comm, &neighbor_coord[0],
				  &possible_neighbors[index]);
  }

  /* Make sure that no MPI_PROC_NULL is passed
   * to MPI_Dist_graph_create
   */
  for (int i{0}; i < n_neighbors; i++) {
	if (possible_neighbors[i] >= 0 && possible_neighbors[i] != cart_rank) {
	  neighbors.push_back(possible_neighbors[i]);
	}
  }
}

void mpireorderinglib::MPIX_Dist_graph_internode_cost(MPI_Comm dist_graph_comm,
													  int *total_offnode_neighbors,
													  int *max_offnode_neighbors) {
  int status;
  MPI_Topo_test(dist_graph_comm, &status);
  assert(status == MPI_DIST_GRAPH);
  *total_offnode_neighbors = 0;
  *max_offnode_neighbors = 0;

  //find out which processes are on the same node
  MPI_Comm node_comm;
  MPI_Group node_group;
  MPI_Group dist_graph_group;

  int dist_graph_rank;
  MPI_Comm_rank(dist_graph_comm, &dist_graph_rank);

  MPI_Comm_split_type(dist_graph_comm, MPI_COMM_TYPE_SHARED,
					  dist_graph_rank, MPI_INFO_NULL, &node_comm);

  int node_group_size, node_group_rank;

  //extract the corresponding groups
  MPI_Comm_group(node_comm, &node_group);
  MPI_Group_size(node_group, &node_group_size);
  MPI_Group_rank(node_group, &node_group_rank);

  int *const group_ranks = new int[node_group_size];
  int *const dist_graph_comm_ranks = new int[node_group_size];

  MPI_Comm_group(dist_graph_comm, &dist_graph_group);

  //initialize node group ranks array
  for (int i{0}; i < node_group_size; i++) {
	group_ranks[i] = i;
  }

  //translate the ranks in the group to the ranks
  //in the old communicator dist_graph_comm
  MPI_Group_translate_ranks(node_group, node_group_size,
							group_ranks, dist_graph_group, dist_graph_comm_ranks);

  int n_sources, n_dest, is_weighted;
  MPI_Dist_graph_neighbors_count(dist_graph_comm, &n_sources,
								 &n_dest, &is_weighted);

  //Each process only counts the communication he starts
  //Good idea to do so?
  int sources[n_sources];
  int dest[n_dest];
  int source_weights[n_sources];
  int dest_weights[n_dest];

  MPI_Dist_graph_neighbors(dist_graph_comm, n_sources, sources,
						   source_weights, n_dest, dest, dest_weights);

  //Explicitly reset the neighbor count and start from
  //zero. Anotherwise one would count MPI_PROC_NULL.
  //Start locally und communicate globally.
  int local_offnode_neighbors = 0;
  int *index_of_neighbor;

  // Another assumption was made, that the number on each
  // process is small. Meaning we have
  // O(num_oncore_pe*neighbors) operations. No presorting
  // of neighbors is done.
  // Only the destination targets are counted
  for (int i{0}; i < n_dest; i++) {
	int neighbor = dest[i];

	if (neighbor == MPI_PROC_NULL)
	  continue;

	index_of_neighbor = std::find(dist_graph_comm_ranks,
								  dist_graph_comm_ranks + node_group_size,
								  neighbor);

	//Increase offnode neighbor count if no index
	//was found
	if (index_of_neighbor == dist_graph_comm_ranks + node_group_size) {
	  local_offnode_neighbors += 1;
	}
  }

  int node_offnode_comm_size;
  MPI_Allreduce(&local_offnode_neighbors,
				&node_offnode_comm_size,
				1, MPI_INT, MPI_SUM, node_comm);

  //Communicate max offnode neighbors
  MPI_Allreduce(&node_offnode_comm_size,
				max_offnode_neighbors,
				1, MPI_INT, MPI_MAX, dist_graph_comm);

  //Communciate local offnode neighbors to everybody
  //sum up and devide by two. Symmetry assumption
  MPI_Allreduce(&local_offnode_neighbors,
				total_offnode_neighbors,
				1, MPI_INT, MPI_SUM, dist_graph_comm);

  delete[] group_ranks;
  delete[] dist_graph_comm_ranks;
}

void mpireorderinglib::MPIX_Dist_graph_create_from_cart_comm(MPI_Comm cart_comm, const int ndims,
															 const int stencil[],
															 const int num_neighbors,
															 const int dims[],
															 const int periods[],
															 MPI_Comm *dist_graph_comm) {

  int cart_rank;
  MPI_Comm_rank(cart_comm, &cart_rank);
  std::vector<int> real_neighbors{};
  mpireorderinglib::get_neighbors_from_stencil(cart_comm, cart_rank,
											   ndims, stencil, num_neighbors, dims,
											   periods, real_neighbors);

  int real_neighbors_size = real_neighbors.size();
  int weights[real_neighbors_size];
  for (int i{0}; i < real_neighbors_size; i++)
	weights[i] = 0;

  int n_vertices_to_specify{0};
  if (real_neighbors_size) {
	n_vertices_to_specify = 1;
  }

  MPI_Dist_graph_create(cart_comm,
						n_vertices_to_specify,
						&cart_rank,
						&real_neighbors_size,
						real_neighbors.data(),
						weights,
						MPI_INFO_NULL,
						0,
						dist_graph_comm);
}

void MPIX_Internode_cost_stencil(MPI_Comm cart_comm, int *total, int *max, int stencil[], int n_neighbors) {
  int status;
  MPI_Topo_test(cart_comm, &status);
  assert(status == MPI_CART);
  MPI_Comm dist_graph;
  int ndims;
  MPI_Cartdim_get(cart_comm, &ndims);
  int dims[ndims], periods[ndims], coords[ndims];
  MPI_Cart_get(cart_comm, ndims, dims, periods, coords);
  mpireorderinglib::MPIX_Dist_graph_create_from_cart_comm(cart_comm,
														  ndims,
														  stencil,
														  n_neighbors,
														  dims,
														  periods,
														  &dist_graph);
  mpireorderinglib::MPIX_Dist_graph_internode_cost(dist_graph, total, max);
  MPI_Comm_free(&dist_graph);
}

void MPIX_Internode_cost(MPI_Comm cart_comm, int *total, int *max) {
  int status;
  MPI_Topo_test(cart_comm, &status);
  assert(status == MPI_CART);
  int ndims;
  MPI_Cartdim_get(cart_comm, &ndims);
  int dims[ndims], periods[ndims], coords[ndims];
  MPI_Cart_get(cart_comm, ndims, dims, periods, coords);
  mpireorderinglib::Configuration config;
  mpireorderinglib::Stencil_Creater stencil_creater;
  stencil_creater.set_stencil(config.get_str_stencil());
  std::vector<int> stencil;
  int n_neighbors{0};
  stencil_creater.create_stencil(ndims, stencil, &n_neighbors);
  MPI_Comm dist_graph;
  mpireorderinglib::MPIX_Dist_graph_create_from_cart_comm(cart_comm,
														  ndims,
														  stencil.data(),
														  n_neighbors,
														  dims,
														  periods,
														  &dist_graph);
  mpireorderinglib::MPIX_Dist_graph_internode_cost(dist_graph, total, max);
  MPI_Comm_free(&dist_graph);
}
