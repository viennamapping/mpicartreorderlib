#include "hyperplane.h"

void hyperplane_sum_of_angles(const int stencil[],
				  const int n_neighbors,
				  const int ndims,
				  float angle_values[]) {

  for (int i{0}; i < ndims; i++) {
	angle_values[i] = 0.0;
  }
  float relativ_vector_length;

  //O(k)
  for (int k = 0; k < n_neighbors; k++) {
	//calculate the length of the relativ coordinate vector
	//O(n)
	relativ_vector_length = 0;
	for (int i = 0; i < ndims; i++) {
	  relativ_vector_length += stencil[k * ndims + i] * stencil[k * ndims + i];
	}

	//if length of relative vector is zero, than it is skipped, since it points to
	//itself
	if (relativ_vector_length == 0)
	  continue;

	//calculate the angle between each dimension
	//O(n)
	float k_i;
	for (int i = 0; i < ndims; i++) {
	  k_i = stencil[k * ndims + i] * stencil[k * ndims + i];
	  angle_values[i] += k_i / relativ_vector_length;
	}
  }
}

void hyperplane_dimension_order(const float angle_values[],
				const int dims[],
				const int ndims,
				int dimension_order[]) {
  //O(n)
  for (int i = 0; i < ndims; i++) {
	dimension_order[i] = i;
  }

  // O ( ndims log (ndims) )
  //Sort the dimension_order vector based on the values in angle_values
  std::sort(&dimension_order[0], &dimension_order[0] + ndims,
			[&](const int &j, const int &k) {
			  if (angle_values[j] == angle_values[k])
				return dims[j] > dims[k];
			  else
				return angle_values[j] < angle_values[k];
			}
  );
}

void base_case_new_coord_calculation(const int ndims,
					 const int my_rank,
					 const int my_partition,
					 const int p_node,
					 const int *const grid_points,
					 int *const new_rank_coords,
					 const int *const dimension_order) {
  for (int d = 0; d < ndims; d++) {
	new_rank_coords[d] = grid_points[2 * d];
  }

  int my_difference = my_rank - my_partition * p_node;
  int span, d;

  //Idea, fill up the the partitions s.t. if the grid size
  //is 2*p_node, the best split between the two is done
  //according to the ranks
  for (int index = ndims - 1; index >= 0; index--) {
	d = dimension_order[index];
	span = (grid_points[2 * d + 1] - grid_points[2 * d]) + 1;
	new_rank_coords[d] += my_difference % span;
	my_difference /= span;
  }
}

void update_dimension_order(const float angle_values[],
				const int dims[],
				const int ndims,
				const int index,
				int dimension_order[]) {

  auto comp = [&](const int &j, const int &k) {
	if (angle_values[j] == angle_values[k])
	  return dims[j] > dims[k];
	else
	  return angle_values[j] < angle_values[k];
  };

  for (int i{index}; i < ndims - 1; i++) {
	if (comp(dimension_order[i + 1], dimension_order[i])) {
	  std::swap(dimension_order[i + 1], dimension_order[i]);
	} else {
	  break;
	}
  }
}

void hyperplane_internal(const int ndims,
			 int dims[],
			 const int original_grid_dimensions[],
			 int grid_points[],
			 const int n_nodes,
			 const int n_processes_p_node,
			 const int my_rank,
			 int new_rank_coords[],
			 const int my_partition,
			 const float angle_values[],
			 int grid_size,
			 int dimension_order[],
			 int partition_id) {
  hyperplane_dimension_order(angle_values, dims, ndims, dimension_order);
  //Entering base case is either having two
  //nodes or one and then fill it up
  if (grid_size <= 2 * n_processes_p_node) {
	base_case_new_coord_calculation(ndims,
					my_rank,
					partition_id,
					n_processes_p_node,
					grid_points,
					new_rank_coords,
					dimension_order);

	return;
  }

  //Find the split and recurse
  //O ( ndims)
  int best_dimension_index;
  for (int i{0}; i < ndims; i++) {
	best_dimension_index = dimension_order[i];
	if (dims[best_dimension_index] == 1) {
	  continue;
	}

	assert(n_processes_p_node != 0);
	assert(dims[best_dimension_index] != 0);
	//Try to split s.t both sides are a multiple of the number of processes on a node
	int subvolume = grid_size / dims[best_dimension_index];

	//Initial positioning of hyperplane
	int split_lhs, split_rhs;
	split_lhs = dims[best_dimension_index] / 2;
	split_rhs = dims[best_dimension_index] - split_lhs;

	//Try to find split along dimension with balanced sizes
	//O( dims[best_dimension_index] )
	while ((subvolume * split_lhs) % n_processes_p_node != 0 &&
		(subvolume * split_rhs) % n_processes_p_node != 0 && split_lhs > 1) {
	  split_lhs -= 1;
	  split_rhs += 1;
	}

	//swap the two variables if the leftside of the subgrid is closer to right boundary
	if (grid_points[2 * best_dimension_index] + dims[best_dimension_index] / 2 >=
		original_grid_dimensions[best_dimension_index] / 2) {
	  int tmp = split_rhs;
	  split_rhs = split_lhs;
	  split_lhs = tmp;
	}

	//If partition was found, recurse
	if (subvolume * split_lhs % n_processes_p_node == 0 &&
		subvolume * split_rhs % n_processes_p_node == 0) {
	  //Adjust and enter the recursion
	  int remaining_partitions = subvolume * split_lhs / n_processes_p_node;

	  //case for the left hand side
	  if (my_partition < partition_id + remaining_partitions) {
		//adjust the grid points left hand side
		grid_points[2 * best_dimension_index + 1] = grid_points[2 * best_dimension_index] + split_lhs - 1;

		//adjust the dimensions left hand side
		dims[best_dimension_index] = split_lhs;

		hyperplane_internal(ndims, dims,
					original_grid_dimensions,
					grid_points,
					n_nodes,
					n_processes_p_node,
					my_rank,
					new_rank_coords,
					my_partition,
					angle_values,
					subvolume * split_lhs,
					dimension_order,
					partition_id);
	  }
		//case for the right hand side
	  else {
		//adjust the grid points rhs
		grid_points[2 * best_dimension_index] = grid_points[2 * best_dimension_index] + split_lhs;
		//adjust the dimesion size rhs
		dims[best_dimension_index] = split_rhs;

		hyperplane_internal(ndims, dims,
				original_grid_dimensions,
				grid_points, n_nodes,
				n_processes_p_node,
				my_rank,
				new_rank_coords,
				my_partition,
				angle_values,
				subvolume * split_rhs,
				dimension_order,
				partition_id + remaining_partitions);
	  }
	  //Exit for loop!
	  break;
	}
  }
}

int MPIX_Hyperplane_comm(MPI_Comm oldcomm,
			  const int ndims,
			  const int dims[],
			  const int periods[],
			  const int stencil[],
			  const int n_neighbors,
			  MPI_Comm *hyperplane_comm,
			  mpireorderinglib::node_approximation_schemes p_node_approximation_scheme) {
  if (oldcomm == MPI_COMM_NULL)
	return 0;

  int n_nodes, n_processes_p_node, my_partition;
  MPI_Comm node_world_comm;
  mpireorderlib::MPIX_Node_comm(oldcomm,
				&node_world_comm,
				&n_nodes,
				&n_processes_p_node,
				p_node_approximation_scheme);

  int my_rank, w_size;
  MPI_Comm_rank(node_world_comm, &my_rank);
  MPI_Comm_size(node_world_comm, &w_size);

  //n_processes_p_node is sum(p_node_i)/n_nodes
  //in order find a suitable partition, reduce it until the number
  //of processes in the base comm is a multiple
  assert(n_processes_p_node != 0);
  while (w_size % n_processes_p_node) {
	n_processes_p_node--;
  }
  n_nodes = w_size / n_processes_p_node;
  my_partition = my_rank / n_processes_p_node;

  //O( d )
  int grid_points[2 * ndims];
  for (int d = 0; d < 2 * ndims; d += 2) {
	grid_points[d] = 0;
	grid_points[d + 1] = dims[d / 2] - 1;
  }

  int new_rank_coords[ndims];

  //O( d )
  int grid_dims[ndims];
  int grid_size = 1;

  //Dimension order checks out in what order the algorithm
  //should try to split the dimensions
  int dimension_order[ndims];
  for (int i = 0; i < ndims; i++) {
	grid_dims[i] = dims[i];
	grid_size *= dims[i];
  }

  //Find the best dimension permutation for finding a suitable split
  //O( kn )
  float angle_values[ndims];
  hyperplane_sum_of_angles(stencil, n_neighbors,
			   ndims, angle_values);


  hyperplane_internal(ndims, grid_dims,
			  dims,
			  grid_points,
			  n_nodes,
			  n_processes_p_node,
			  my_rank,
			  new_rank_coords,
			  my_partition,
			  angle_values,
			  grid_size,
			  dimension_order);

  int new_rank;
  mpireorderinglib::get_rank_from_coords(new_rank_coords, ndims, dims, &new_rank);

  MPI_Comm tmp;
  MPI_Comm_split(node_world_comm, 0, new_rank, &tmp);
  int err = MPI_Cart_create(tmp, ndims, dims, periods, 0, hyperplane_comm);
  MPI_Comm_free(&tmp);
  MPI_Comm_free(&node_world_comm);
  return err;
}