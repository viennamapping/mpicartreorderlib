//
// Created by konradvonkirchbach on 5/27/20.
//

#include "definitions.h"

mpireorderinglib::node_approximation_schemes string_to_node_approximation_schemes(const std::string &str){
if (str.compare("NODES_MIN")) {
return mpireorderinglib::NODES_MIN;
} else if (str.compare("NODES_MEAN")) {
return mpireorderinglib::NODES_MEAN;
} else if (str.compare("NODES_MAX")) {
return mpireorderinglib::NODES_MAX;
} else {
spdlog::warn("No suitable node aggregation was found! Fall back on default: NODES_MEAN");
return mpireorderinglib::NODES_MEAN;
}
}
