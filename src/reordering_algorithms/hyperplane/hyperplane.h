#ifndef HYPERPLANE_H
#define HYPERPLANE_H

#include <algorithm>

#include <mpi.h>
#include "definitions.h"
#include "reordering_algorithms/utils/node_comm.h"
#include "reordering_algorithms/utils/utils.h"

//creates the hyperplane communicator
int MPIX_Hyperplane_comm(MPI_Comm oldcomm,
        const int ndims,
        const int dims[],
        const int periods[],
        const int stencil[],
        const int n_neighbors,
        MPI_Comm *hyperplane_comm,
        mpireorderinglib::node_approximation_schemes p_node_approximation_scheme =
        	mpireorderinglib::node_approximation_schemes::NODES_MEAN);

/*the internal function to find the new coordinates
 * of each rank. In the stencil array is a list
 * of relative coordinate vectors, n_neighbors indicates
 * the amount of communication neighbors. Grid points is 
 * an array that is 2*ndims and holds the start and endpoints of 
 * subgrid in the original grid.
 * n_nodes and n_processes_p_node is the number of computation nodes
 * and the number of processes per computation node. It is assumed
 * that on every node there is the same amount of processes.
 * new_rank_coords is the new position of the calling rank in the subgrid.
 * my_partition id describes the id of node on which the calling process is
 * assigned to. 
 * Dimension order is the order along which the algorithm tries to find the
 * hyperplane.
 * partition id is the criteria on which the decision for left hand or 
 * right hand assignment is made.
 */
void hyperplane_internal(const int ndims,
		int dims[],
		const int original_grid_dimensions[],
		int grid_points[],
		const int n_nodes,
		const int n_processes_p_node,
		const int my_rank,
		int new_rank_coords[],
		const int my_partition,
		const double angle_values[],
		int grid_size,
		int dimension_order[],
		int partition_id = 0);


/* Calculates the angles between the relativ coordinate vectors
 * and the grid dimensions and adds them up the sum_of_angles array
 */
void hyperplane_sum_of_angles(const int stencil[],
		const int n_neighbors,
		const int ndims,
		double angle_values[]);


/* Given an array with the angles between them, sort them 
 * correctly corresponding to the grid size
 */
void hyperplane_dimension_order(const double angle_values[],
		const int dims[],
		const int ndims,
		int dimension_order[]);


//Basecase calculation. When entering the base case,
//the new coordinate will be calculated and stored in 
//new_rank_coords.
void base_case_new_coord_calculation(const int ndims, 
		const int my_rank,
		const int my_partition,
		const int p_node,
		const int * const grid_points,
		int * const new_rank_coords,
		const int * const dimension_order);


//Once the original dimension order has been established,
//we only need to update the rank of the split dimension
//this can be done in O(n) time
//TODO: can be done in O(log(n)) time -> binary insert
void update_dimension_order(const double angle_values[],
		const int dims[],
		const int ndims,
		const int index,
		int dimension_order[]);

#endif
