cmake_minimum_required(VERSION 3.0)

project(mpireorderlib VERSION 0.0)

find_package(MPI REQUIRED)

SET(CMAKE_CXX_COMPILER mpicxx)

SET(MY_COMPILE_FLAGS "")

if(LOGGING)
    SET(MY_COMPILE_FLAGS "${MY_COMPILE_FLAGS} -DLOGGING")
endif()

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_SOURCE_DIR}/include_")
set(CMAKE_INSTALL_LIBDIR "${CMAKE_SOURCE_DIR}/lib")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}    "include")
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} "src")

set(MY_COMPILE_FLAGS "")

set(REORDERING_ALGORITHMS
        ${SRC_DIR}/reordering_algorithms/hyperplane/hyperplane.cpp
        ${SRC_DIR}/reordering_algorithms/hyperplane/Hyperplane_Reorderer.cpp
        ${SRC_DIR}/reordering_algorithms/kd_tree/kd_tree.cpp
        ${SRC_DIR}/reordering_algorithms/kd_tree/kd_Tree_Reorderer.cpp
        ${SRC_DIR}/reordering_algorithms/stencil_strips/stencil_strips.cpp
        ${SRC_DIR}/reordering_algorithms/stencil_strips/Stencil_Strips_Reorderer.cpp
        ${SRC_DIR}/reordering_algorithms/utils/node_comm.cpp
        ${SRC_DIR}/reordering_algorithms/utils/utils.cpp
        ${SRC_DIR}/Evaluation/evaluation_method.cpp
        ${SRC_DIR}/definitions.cpp
        ${SRC_DIR}/Log/Logger.cpp)

set(SETUP_CONFIGURATION
        ${SRC_DIR}/configuration/configuration src/reordering_algorithms/utils/utils.h src/reordering_algorithms/utils/utils.cpp src/reordering_algorithms/abstract_reordering_scheme.h src/reordering_algorithms/hyperplane/Hyperplane_Reorderer.cpp src/reordering_algorithms/hyperplane/Hyperplane_Reorderer.h src/reordering_algorithms/kd_tree/kd_Tree_Reorderer.cpp src/reordering_algorithms/kd_tree/kd_Tree_Reorderer.h src/reordering_algorithms/stencil_strips/Stencil_Strips_Reorderer.cpp src/reordering_algorithms/stencil_strips/Stencil_Strips_Reorderer.h src/reordering_algorithms/include_reodering_schemes.h src/Log/Logger.cpp src/Log/Logger.h src/Stencils/Stencil_Creater.cpp src/Stencils/Stencil_Creater.h src/definitions.cpp src/Evaluation/evaluation_methods.h src/Evaluation/evaluation_method.cpp)

add_library(mpicartreorderlib SHARED
        ${REORDERING_ALGORITHMS}
        ${SETUP_CONFIGURATION})

set_target_properties(mpicartreorderlib PROPERTIES SOVERSION 0)
set_target_properties(mpicartreorderlib PROPERTIES PUBLIC_HEADER include/mpicartreorder.h)

install(TARGETS mpicartreorderlib
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
