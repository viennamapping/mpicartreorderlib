//
// Created by konradvonkirchbach on 5/27/20.
//

#include "Stencil_Creater.h"

namespace mpireorderinglib {
void five_point_stencil(std::vector< int >& stencil, const int ndims,
						int * n_neighbors){
  spdlog::info("Came into five_point with " + std::to_string(ndims));
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

namespace mpireorderinglib {
/**
 * Expects the string to hold integer values, sperated by @
 * Example for a 2D nearest neighbor stencil: 1@0@-1@0@0@1@0@-1
 * @param str the string passed by the environment variable CART_REORDER_STENCIL
 * @param v vector in which to write the values
 */
void extract_stencil_from_string(const std::string& str, std::vector<int>& v, int* n_neighbors, int ndims) {
  std::istringstream tokenStream (str);
  std::string token;
  while (getline(tokenStream, token, '@')) {
    try {
      v.push_back(std::atoi(token.data()));
    } catch (std::exception& e) {
      spdlog::error("Tried to convert " + token + " to int");
      std::cerr << e.what() << std::endl;
    }
  }

  if (v.size() % ndims != 0) {
    spdlog::warn("Passed stencil is not a multiple of the number of dimensions! Padding with zeros!");
    while (v.size() % ndims) v.push_back(0);
    std::string str_v = "";
    for (int i : v) str_v += std::to_string(i) + " ";
    spdlog::info("New stencil = " + str_v);
  }
  *n_neighbors = v.size();
}
}

void mpireorderinglib::Stencil_Creater::create_stencil(const int ndims, std::vector<int> &stencil, int* n_neighbors) {
	if ( str_stencil.compare("UNDEFINED\0") == 0 ) {
	  spdlog::info("No stencil pattern defined. Proceeding with ndims dimensional nearest neighbor stencil.");
	  mpireorderinglib::five_point_stencil(stencil, ndims, n_neighbors);
	} else {
		spdlog::info("Try to build stencil from " + str_stencil);
		mpireorderinglib::extract_stencil_from_string(str_stencil, stencil, n_neighbors, ndims);
	}
}
void mpireorderinglib::Stencil_Creater::set_stencil(std::basic_string<char> str) {
	str_stencil = str;
}
mpireorderinglib::Stencil_Creater::Stencil_Creater() : str_stencil("UNDEFINED") {

}
mpireorderinglib::Stencil_Creater::Stencil_Creater(std::string &str) {
	str_stencil = str;
}
