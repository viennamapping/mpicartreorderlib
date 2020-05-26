#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <iostream>
#include <string>

namespace mpireorderinglib {
	enum reordering_algorithms {
		NO_REORDER,
		HYPERPLANE,
		KD_TREE,
		STENCIL_STRIPS
	};
	mpireorderinglib::reordering_algorithms string_to_reordering_algorithms (const std::string& str);
	std::string reordering_algorithms_to_string (mpireorderinglib::reordering_algorithms alg);

	enum node_approximation_schemes{
		NODES_MIN,
		NODES_MEAN,
		NODES_MAX
	};
	mpireorderinglib::node_approximation_schemes string_to_node_approximation_schemes (const std::string& str);
	std::string node_approximation_to_string (mpireorderinglib::node_approximation_schemes);
}

#endif
