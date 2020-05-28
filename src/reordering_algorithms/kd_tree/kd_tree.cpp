//
// Created by markus on 27.01.20.
//

#include <cstring>
#include <climits>
#include "kd_tree.h"

/**
 * Recursive splitting of the grid. This is the algorithms main procedure.
 * @param dims The grid's dimensions
 * @param num_dims
 * @param size The remaining grid's number of nodes
 * @param target_index This process' targeted rank for the current subtree
 * @param coordinate The resulting 'num_dims' dimensional coordinate
 * @param factors Each dimension's weight for deciding the splitting dimension
 */
void skewed_find_node_recursive(int *dims, int num_dims, int size, int target_index, int *coordinate, int *factors);

/* This function chooses the biggest a's index, based on a's value * a's factor.
 * Should, however, a dimension be at size 1 already, we cannot pick it again, so the value is negated instead.
 * This has to happen, in case a factor 0 occurs - otherwise this dimension would never be picked,
 * even though its actual size is greater than 1.*/
int skewed_max_index(const int *a, const int *factors, int n);

/* The dimensions' factors are needed to skew the "max index" function in the right direction.
 * If there are fewer communications across one dimension, the less harm will be caused by cutting along it.
 * Therefore less frequently crossed dimensions have a higher factor than highly frequented ones.
 */
void calculate_factors(int *factors, int num_dims, const int *stencil, int stencil_size);

int
MPIX_skewed_kdtree_cart_create(MPI_Comm old_comm, const int *dims, int num_dims, const int *stencil, int stencil_size,
                               const int * const periods, MPI_Comm *cart_comm) {
    MPI_Comm sorted_comm;
    mpireorderinglib::MPIX_Node_comm(old_comm, &sorted_comm);
    int *factors = static_cast<int *>(malloc(num_dims * sizeof(double)));
    int world_size;
    MPI_Comm_size(sorted_comm, &world_size);
    int world_rank;
    MPI_Comm_rank(sorted_comm, &world_rank);

    int *coordinate = static_cast<int *>(calloc(num_dims, sizeof(int)));
    int *dims_heper = static_cast<int *>(malloc(num_dims * sizeof(int)));
    memcpy(dims_heper, dims, num_dims * sizeof(int));

    // calculating these factors is only used in the skewed kd-tree variant
    calculate_factors(factors, num_dims, stencil, stencil_size);
    // actual coordinate calculation
    skewed_find_node_recursive(dims_heper, num_dims, world_size, world_rank, coordinate, factors);

    int new_rank = coordinate[0];
    for (int i = 1; i < num_dims; i++) {
        new_rank = new_rank * dims[i] + coordinate[i];
    }

    MPI_Comm tmp;
    MPI_Comm_split(sorted_comm, 0, new_rank, &tmp);
    int err = MPI_Cart_create(tmp, num_dims, dims, periods, 0, cart_comm);
    MPI_Comm_free(&tmp);
    MPI_Comm_free(&sorted_comm);
    free(factors);
    free(dims_heper);
    free(coordinate);
    return err;
}


void skewed_find_node_recursive(int *dims, int num_dims, int size, int target_index, int *coordinate, int *factors) {
    int split_dim = skewed_max_index(dims, factors, num_dims);

    int split_dim_size = dims[split_dim];
    if (split_dim_size <= 1) {
        // the largest dimension is only 1 big - we cannot iterate further
        return;
    }

    int left_dim_size = split_dim_size / 2;
    int left_size = (size / split_dim_size) * left_dim_size;

    if (target_index < left_size) {
        // this process' coordinate is on the left side
        dims[split_dim] = left_dim_size;
        skewed_find_node_recursive(dims, num_dims, left_size, target_index, coordinate, factors);

    } else {
        // this process' coordinate is on the right side
        dims[split_dim] -= left_dim_size;
        coordinate[split_dim] += left_dim_size;
        target_index -= left_size;
        skewed_find_node_recursive(dims, num_dims, size - left_size, target_index, coordinate, factors);
    }
}

int skewed_max_index(const int *a, const int *factors, int n) {
    double m = -1;
    int j = -1;
    double skewed_entry;

    for (int i = 0; i < n; i++) {
        skewed_entry = a[i];
        if (skewed_entry > 1) {
            if (factors[i] == 0) {
                skewed_entry = INT_MAX;
            } else {
                skewed_entry /= factors[i];
            }
        } else {
            skewed_entry *= -1;
        }
        if (skewed_entry > m) {
            m = skewed_entry;
            j = i;
        }
    }
    return j;
}

void calculate_factors(int *factors, int num_dims, const int *stencil, int stencil_size) {
    for (int d = 0; d < num_dims; d++) {
        factors[d] = 0;
        for (int i = 0; i < stencil_size; ++i) {

            if (stencil[(i * num_dims) + d] != 0) {
                factors[d]++;
            }
        }
    }
}
