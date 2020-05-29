#include "configuration/configuration.h"

mpireorderinglib::Configuration::Configuration() :
	str_alg("STENCIL_STRIPS"),
	str_node_scheme("NODES_MEAN"),
	str_stencil("UNDEFINED") {
  const char *flag = getenv(env_alg.data());
  if (flag) {
	str_alg = flag;
	spdlog::info("Read " + str_alg + " as algorithm");
  }
  flag = getenv(env_stencil.data());
  if (flag) {
	str_stencil = flag;
	spdlog::info("Read " + str_stencil + " as stencil");
  }
  flag = getenv(env_node_scheme.data());
  if (flag) {
	str_node_scheme = flag;
	spdlog::info("Read " + str_node_scheme + " as node scheme");
  }
}

int mpireorderinglib::Configuration::perform_reordering(MPI_Comm old_comm,
														const int ndims,
														const int *dims,
														const int *periods,
														const int *stencil,
														const int n_neighbors,
														MPI_Comm *cart_comm) const {
  mpireorderinglib::node_approximation_schemes scheme =
	  mpireorderinglib::string_to_node_approximation_schemes(str_node_scheme);
  for (const std::unique_ptr<mpireorderinglib::ReorderingScheme> &ptr : reorder_schemes) {
	if (ptr->get_name() == str_alg) {
	  spdlog::info("Perform reordering with " + str_alg);
	  return ptr->perform_reordering(old_comm, ndims, dims, periods, stencil,
									 n_neighbors, cart_comm, scheme);
	}
  }
  spdlog::warn("Reordering Algorithm not found!"
			   " Performing Reordering with Stencil Strips");

  return reorder_schemes.back()->perform_reordering(old_comm, ndims, dims,
													periods, stencil, n_neighbors, cart_comm, scheme);
}

#ifdef __cplusplus
extern "C"
#endif
int MPIX_Cart_Comm_stencil(MPI_Comm old_comm, int ndims, const int dims[],
						   const int periods[], int reorder, const int stencil[],
						   const int n_neighbors, MPI_Comm *comm_cart) {
  static const mpireorderinglib::Configuration config;
  return config.perform_reordering(old_comm, ndims, dims, periods, stencil, n_neighbors, comm_cart);
}

std::string mpireorderinglib::Configuration::get_str_stencil() const {
  return str_stencil;
}

#ifdef __cplusplus
extern "C"
#endif
int MPI_Cart_create(MPI_Comm old_comm, int ndims, const int dims[],
					const int periods[], int reorder, MPI_Comm *comm_cart) {
  if (reorder == 0) {
	return PMPI_Cart_create(old_comm, ndims, dims, periods, reorder, comm_cart);
  } else {
	const mpireorderinglib::Configuration config;
	mpireorderinglib::Stencil_Creater stencil_creator;
	stencil_creator.set_stencil(config.get_str_stencil());
	std::vector<int> stencil;
	int n_neighbors{0};
	stencil_creator.create_stencil(ndims, stencil, &n_neighbors);
	return config.perform_reordering(old_comm, ndims, dims, periods, stencil.data(), n_neighbors,
									 comm_cart);
  }
}

const std::string mpireorderinglib::Configuration::env_alg = "CART_REORDER_ALGORITHM";
const std::string mpireorderinglib::Configuration::env_stencil = "CART_REORDER_STENCIL";
const std::string mpireorderinglib::Configuration::env_node_scheme = "CART_REORDER_NODE_AGGREGATION";
const std::array<std::unique_ptr<mpireorderinglib::ReorderingScheme>, 3>
	mpireorderinglib::Configuration::reorder_schemes{std::make_unique<mpireorderinglib::Hyperplane_Reorderer>(),
													 std::make_unique<mpireorderinglib::kd_Tree_Reorderer>(),
													 std::make_unique<mpireorderinglib::Stencil_Strips_Reorderer>()};
