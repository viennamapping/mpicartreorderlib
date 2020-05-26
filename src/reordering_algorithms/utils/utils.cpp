//
// Created by konradvonkirchbach on 5/26/20.
//
#include "reordering_algorithms/utils/utils.h"

double mpireorderinglib::n_root(const double base, const int exponent){
  return std::pow(base, 1.0/exponent);
}

void mpireorderinglib::get_hypercube_distortion_factors(const int ndims,
									  const int stencil[],
									  const int n_neighbors,
									  double alphas[])
{
  int index;
  std::vector<int> bounding_rectangle (2*ndims, 0);
  for(int i {0}; i < n_neighbors*ndims; i++)
  {
	index = (i%ndims)*2;
	if(bounding_rectangle[index] < stencil[i])
	  bounding_rectangle[index] = stencil[i];
	if (bounding_rectangle[index+1] > stencil[i])
	  bounding_rectangle[index+1] = stencil[i];
  }
  //volumne of bounding box
  int stencil_volumne {1}, val, stencil_ndims{0};
  for(size_t i {0}; i < bounding_rectangle.size(); i+=2) {
	val = std::abs(bounding_rectangle[i] - bounding_rectangle[i + 1]);
	if (val == 0)
	  continue;
	stencil_volumne *= val;
	stencil_ndims++;
  }

  double n_root_stencil_vol = n_root(stencil_volumne, stencil_ndims);
  for(int i {0}; i < ndims; i++)
  {
	alphas[i] = static_cast<double>(bounding_rectangle[2*i] - bounding_rectangle[2*i+1])/n_root_stencil_vol;
  }
}

void mpireorderinglib::get_coord_from_rank(int rank,
						 const int ndims,
						 const int * const dims,
						 int* const coord){
  int n_processes {1};
  //O(d)
  for( int i {0}; i < ndims; i++ )
  {
	n_processes *= dims[i];
  }
  //O(d)
  for( int i {0}; i < ndims; i++ ){
	n_processes /= dims[i];
	coord[i] = rank / n_processes;
	rank %= n_processes;
  }
}

void mpireorderinglib::get_rank_from_coords(const int * const coords,
						  const int ndims,
						  const int * const dims,
						  int * rank)
{
  *rank = 0;
  int n_processes {1};

  for ( int i {0}; i < ndims; i++ )
  {
	n_processes *= dims[i];
  }

  for( int i {0}; i < ndims; i++ )
  {
	n_processes /= dims[i];
	*rank += n_processes*coords[i];
  }
}
