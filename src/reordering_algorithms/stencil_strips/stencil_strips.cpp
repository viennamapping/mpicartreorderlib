//
// Created by konradvonkirchbach on 2/25/20.
//

#include "stencil_strips.h"

int max_index(const int *a, int n);

void compute_dim_order(int num_dims, int max_dim, int *dim_order);


void compute_stencil_strip_heights(int num_dims, int part_size, int max_dim, const int *dims, const int *dim_order,
                           int *small_strip_heights, int *nums_big_strips, int *nums_small_strips,
                           const int stencil[], const int n_neighbors) {
    int rem_part_size = part_size;
    int nroot;

    double alphas[num_dims];
  	mpireorderinglib::get_hypercube_distortion_factors(num_dims, stencil, n_neighbors, alphas);

    for (int dim_index = 0; dim_index < num_dims; dim_index++) {
        int dim = dim_order[dim_index];
        nroot = pow(rem_part_size*alphas[dim], 1.0 / num_dims);
        nroot = dim == max_dim ? 1 : nroot;

        if(nroot == 0)
          nroot = 1;

        int num_strips;
        if (dim != max_dim) {
            num_strips = dims[dim] / nroot;
        } else {
            num_strips = dims[dim];
        }

        if (num_strips <= 0) {
            num_strips = 1;
        }

        int rem = dims[dim] - num_strips * nroot;
        small_strip_heights[dim] = nroot + rem / num_strips;
        nums_big_strips[dim] = rem % num_strips;
        nums_small_strips[dim] = num_strips - nums_big_strips[dim];
        rem_part_size /= small_strip_heights[dim];
    }
}

void compute_coord(int num_dims, const int *strip_sizes, int target_rank, int *coord, const int *dim_order,
                   const bool *flipped_dim);

void compute_stencil_strip(int num_dims, const int *dims, int *strip_sizes,
                   int max_dim, int target_rank, int *coord, int total_nodes, const int *dim_order, int part_size,
                   const int stencil[], const int n_neighbor) {
    bool flip_next = false;
    bool flipped_dim[num_dims];
    int curr_dim;
    int strip_coord[num_dims];
    int small_strip_heights[num_dims];
    int nums_big_strips[num_dims];
    int nums_small_strips[num_dims];

    compute_stencil_strip_heights(num_dims, part_size, max_dim, dims, dim_order, small_strip_heights, nums_big_strips,
                          nums_small_strips, stencil, n_neighbor);

    for (int dim_index = 0; dim_index < num_dims; dim_index++) {
        curr_dim = dim_order[dim_index];
        flipped_dim[curr_dim] = flip_next;

        int small_strip_height = small_strip_heights[curr_dim];
        int num_big_strips = nums_big_strips[curr_dim];
        int num_small_strips = nums_small_strips[curr_dim];

        // regardless of strip-height, a single row in any strip contains the same number of nodes (always an integer)
        int nodes_per_row = total_nodes / dims[curr_dim];
        // a big strip has one more row than a small strip, so we calculate how many nodes are in a big strip
        int big_strip_nodes = ((small_strip_height + 1) * num_big_strips * nodes_per_row);
        int small_strip_nodes = total_nodes - big_strip_nodes;

        if (!flip_next) {
            // assuming that we start by filling the bigger strips, we calculate whether or not we surpass the 'big strip layers'
            // if so, then the small strips also have to be filled
            bool small_strips_required = target_rank >= big_strip_nodes;

            if (small_strips_required) {
                // we know for sure that our rank surpasses all big strips
                target_rank -= big_strip_nodes;
                // calculate in which small strip we are
                int small_strip_index = target_rank / (small_strip_height * nodes_per_row);
                target_rank -= small_strip_index * small_strip_height * nodes_per_row;
                coord[curr_dim] += num_big_strips * (small_strip_height + 1) + small_strip_index * small_strip_height;
                strip_sizes[curr_dim] = small_strip_height;
                strip_coord[curr_dim] = num_big_strips + small_strip_index;
            } else {
                // we know for sure that our rank is somewhere within the big strips
                int big_strip_index = target_rank / ((small_strip_height + 1) * nodes_per_row);
                target_rank -= big_strip_index * (small_strip_height + 1) * nodes_per_row;
                coord[curr_dim] += big_strip_index * (small_strip_height + 1);
                strip_sizes[curr_dim] = small_strip_height + 1;
                strip_coord[curr_dim] = big_strip_index;
            }
        } else {
            // assuming that we start by filling the smaller strips, we calculate whether or not we surpass the
            // 'small strip layers'
            // if so, then the big strips also have to be filled
            bool big_strips_required = target_rank >= small_strip_nodes;

            if (big_strips_required) {
                // we know for sure that our rank surpasses all small strips
                target_rank -= small_strip_nodes;
                // calculate in which small strip we are
                int big_strip_index = target_rank / ((small_strip_height + 1) * nodes_per_row);
                target_rank -= big_strip_index * (small_strip_height + 1) * nodes_per_row;
                coord[curr_dim] += (num_big_strips - (big_strip_index + 1)) * (small_strip_height + 1);
                strip_sizes[curr_dim] = small_strip_height + 1;
                strip_coord[curr_dim] = (num_big_strips - (big_strip_index + 1));
            } else {
                // we know for sure that our rank is somewhere within the small strips
                int small_strip_index = target_rank / (small_strip_height * nodes_per_row);
                target_rank -= small_strip_index * small_strip_height * nodes_per_row;
                coord[curr_dim] += (num_big_strips * (small_strip_height + 1)) +
                                   (num_small_strips - (small_strip_index + 1)) * small_strip_height;
                strip_sizes[curr_dim] = small_strip_height;
                strip_coord[curr_dim] = num_big_strips + (num_small_strips - (small_strip_index + 1));
            }
        }
        if (strip_coord[curr_dim] % 2 == 1) {
            flip_next = !flip_next;
        }
        total_nodes = (total_nodes / dims[curr_dim]) * strip_sizes[curr_dim];
    }
    compute_coord(num_dims, strip_sizes, target_rank, coord, dim_order, flipped_dim);
}

void MPIX_stencil_strips_cart_create(MPI_Comm old_comm, const int *dims, const int num_dims, int part_size,
                                      const int* periods, const int stencil[], const int n_neighbors,
                                      MPI_Comm *cart_comm) {
    int rank, world_size;
    MPI_Comm_rank(old_comm, &rank);
    MPI_Comm_size(old_comm, &world_size);
    int *coord = static_cast<int *>(calloc(num_dims, sizeof(int)));
    int *strip_sizes = static_cast<int *>(calloc(num_dims, sizeof(int)));
    int *dim_order = static_cast<int *>(malloc(num_dims * sizeof(int)));

    int max_dim = max_index(dims, num_dims);

    compute_dim_order(num_dims, max_dim, dim_order);
    compute_stencil_strip(num_dims, dims, strip_sizes, max_dim, rank, coord, world_size, dim_order, part_size,
    	stencil, n_neighbors);

    int new_rank = coord[0];
    for (int i = 1; i < num_dims; i++) {
        new_rank = new_rank * dims[i] + coord[i];
    }

    MPI_Comm tmp;
    MPI_Comm_split(old_comm, 0, new_rank, &tmp);
    MPI_Cart_create(tmp, num_dims, dims, periods, 0, cart_comm);
    MPI_Comm_free(&tmp);

    free(coord);
    free(strip_sizes);
    free(dim_order);
}

int get_part_size(MPI_Comm oldcomm, mpireorderinglib::node_approximation_schemes scheme);

int MPIX_Stencil_strips_cart(MPI_Comm oldcomm, const int ndims, const int *dims, const int *periods, const int reorder,
                              const int stencil[], const int n_neighbors, MPI_Comm *cartcomm,
                              mpireorderinglib::node_approximation_schemes scheme) {
    int part_size, n_nodes;
    part_size = get_part_size(oldcomm, scheme);

    MPI_Comm tmp;
    mpireorderlib::MPIX_Node_comm (oldcomm, &tmp, &n_nodes, &part_size, scheme);
    MPIX_stencil_strips_cart_create(oldcomm, dims, ndims, part_size, periods, stencil, n_neighbors, &tmp);

    int err = MPI_Cart_create(tmp, ndims, dims, periods, 0, cartcomm);
    MPI_Comm_free(&tmp);
    return err;
}
