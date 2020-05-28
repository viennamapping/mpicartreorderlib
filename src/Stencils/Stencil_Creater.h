//
// Created by konradvonkirchbach on 5/27/20.
//

#ifndef MPIREORDERLIB_SRC_STENCILS_STENCIL_CREATER_H
#define MPIREORDERLIB_SRC_STENCILS_STENCIL_CREATER_H

#include <vector>
#include <string>

#include <iostream>
#include <sstream>
#include "spdlog/spdlog.h"

namespace mpireorderinglib {
class Stencil_Creater {
private:
  std::string str_stencil;
public:
  Stencil_Creater();
  explicit Stencil_Creater(std::string& str);
  void create_stencil(int ndims, std::vector< int >& stencil, int* n_neighbors);
  void set_stencil(std::basic_string<char> str);
};
}

#endif //MPIREORDERLIB_SRC_STENCILS_STENCIL_CREATER_H
