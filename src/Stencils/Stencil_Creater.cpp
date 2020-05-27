//
// Created by konradvonkirchbach on 5/27/20.
//

#include "Stencil_Creater.h"

namespace mpireorderinglib {
void five_point_stencil(std::vector< int >& stencil, const int ndims,
						int * n_neighbors){
  int stencil_size = 2*ndims*ndims + ndims;
  *n_neighbors = 2*ndims + 1;
  int dim_to_set = 0;

  stencil.resize(stencil_size);
  for(int& i : stencil) i = 0;

  for( int i {0}; i < stencil_size - ndims; i += (*n_neighbors - 1)){
	stencil[i + dim_to_set] = 1;
	stencil[i + dim_to_set + ndims] = -1;
	dim_to_set++;
  }
}
}

void mpireorderinglib::Stencil_Creater::create_stencil(const int ndims, std::vector<int> &stencil, int* n_neighbors) {
	if ( str_stencil.compare("FIVE_POINT") ) {
	  mpireorderinglib::five_point_stencil(stencil, ndims, n_neighbors);
	} else {
		spdlog::warn("No stencil selected! Default nearest neighbor is selected!");
		mpireorderinglib::five_point_stencil(stencil, ndims, n_neighbors);
	}
}
void mpireorderinglib::Stencil_Creater::set_stencil(std::string &str) {
	str_stencil = str;
}
mpireorderinglib::Stencil_Creater::Stencil_Creater() : str_stencil("FIVE_POINT") {

}
mpireorderinglib::Stencil_Creater::Stencil_Creater(std::string &str) {
	str_stencil = str;
}
