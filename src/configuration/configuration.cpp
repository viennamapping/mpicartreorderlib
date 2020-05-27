#include "configuration/configuration.h"

mpireorderinglib::Configuration::Configuration() :
str_alg("STENCIL_STRIPS"),
str_node_scheme("NODES_MEAN"),
str_stencil("FIVE_POINT") {
  const char *env_var = getenv(env_flag);
  if (env_var) {
	char *pch = strtok(env_var, ":");
	switch (pch) {
	case "HYPERPLANE": alg = HYPERPLANE;
	  break;
	case "KD_TREE": alg = KD_TREE;
	  break;
	case "STENCIL_STRIPS": alg = STENCIL_STRIPS;
	  break;
	case "NO_REORDER": alg = NO_REORDER;
	  break;
	default:
	  std::cout << "Warning: Invalid Algorithm! Set "
				   "CART_REORDER_ALG=<HYPERPLANE;KD_TREE;STENCIL_STRIPS;NO_REORDER>:<NODES_MEAN;NODES_MIN;NODES_MAX>. "
				   "Default Algorithm: STENCIL_STRIPS. Default Node scheme = NODES_MEAN."
				<< std::endl;
	}

	pch = strtok(NULL, ":");
	switch (pch) {
	case "NODES_MIN": node_scheme = NODES_MIN;
	  break;
	case "NODES_MEAN": node_scheme = NODES_MEAN;
	  break;
	case "NODES_MAX";
	  node_scheme = NODES_MAX;
	  break;
	default:
	  std::cout << "Warning: Invalid Node Scheme! Set "
				   "CART_REORDER_ALG=<HYPERPLANE;KD_TREE;STENCIL_STRIPS;NO_REORDER>:<NODES_MEAN;NODES_MIN;NODES_MAX>. "
				   "Default Algorithm: STENCIL_STRIPS. Default Node scheme = NODES_MEAN."
				<< std::endl;
	}
  }
}

mpireorderinglib::reordering_algorithms mpireorderinglib::Configuration::get_configuration() {
  return alg;
}

mpireorderinglib::node_approximation_schemes mpireorderinglib::Configuration::get_node_scheme() {
  return node_scheme;
}

int MPIX_Cart_Comm_stencil(MPI_Comm old_comm, int ndims, const int dims[],
						   const int periods[], int reorder, const int stencil[],
						   const int n_neighbors, MPI_Comm *comm_cart) {
  static const mpireorderinglib::Configuration config;
  mpireorderinglib::reordering_algorithms alg = config.get_configuration();
  mpireorderinglib::node_approximation_schemes scheme = config.get_node_scheme();

  int err;
  switch (alg) {
  case mpireorderinglib::HYPERPLANE:
    err = MPIX_Hyperplane_comm(old_comm, ndims, dims, periods, reorder, const int stencil[],
    					n_neighbors, comm_cart, scheme);
    break;
  case mpireorderinglib::KD_TREE:
    err = MPIX_skewed_kdtree_cart_create(old_comm, dims, ndims, stencil, n_neighbors, periods, comm_cart);
    break;
  case mpireorderinglib::STENCIL_STRIPS:
    err = MPIX_Stencil_strips_cart(old_comm, ndims, dims, periods, 0, stencil, n_neighbors, comm_cart, scheme);
    break;
  case mpireorderinglib::NO_REORDER:
    err = MPI_Cart_create(old_comm, ndims, dims, periods, 0, comm_cart);
    break;
  }
  return err;
}
