#ifndef CONFIGURATION_H

#include <iostream>
#include <string>

#include "definitons.h"

namespace mpicartreorder {
class Configuration {
private:
	reordering_algorithms alg;	
	static const std::string env_flag;
public:
	Configuration (int* argc, char*** argv);
	void get_configuration(reordering_algorithms alg);
};

const std::string Configuration::env_flag = "CART_REORDER_ALG"
}
#endif
