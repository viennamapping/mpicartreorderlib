//
// Created by konradvonkirchbach on 5/26/20.
//

#ifndef MPIREORDERLIB_SRC_REORDERING_ALGORITHMS_UTILS_UTILS_H
#define MPIREORDERLIB_SRC_REORDERING_ALGORITHMS_UTILS_UTILS_H

#include <vector>
#include <cmath>

namespace mpireorderinglib {
void get_hypercube_distortion_factors(const int ndims,
									  const int stencil[],
									  const int n_neighbors,
									  double alphas[]);

double n_root(const double base, const int exponent);

void get_rank_from_coords(const int *const coords,
						  const int ndims,
						  const int *const dims,
						  int *rank);

void get_coord_from_rank(int rank,
						 const int ndims,
						 const int *const dims,
						 int *const coord);
}

#endif //MPIREORDERLIB_SRC_REORDERING_ALGORITHMS_UTILS_UTILS_H
