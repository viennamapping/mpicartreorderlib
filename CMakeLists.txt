cmake_minimum_required(VERSION 3.0)

project(mpireorderlib VERSION 0.0)

find_package(MPI REQUIRED)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} "include")
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} "src")

set(MY_COMPILE_FLAGS "")

set(REORDERING_ALGORITHMS
        ${SRC_DIR}/reordering_algorithms/hyperplane/hyperplane.cpp
        ${SRC_DIR}/reordering_algorithms/kd_tree/kd_tree.cpp
        ${SRC_DIR}/reordering_algorithms/stencil_strips/stencil_strips.cpp
        ${SRC_DIR}/reordering_algorithms/utils/node_comm.cpp)

set(SETUP_CONFIGURATION
        ${SRC_DIR}/configuration/configuration src/reordering_algorithms/utils/utils.h src/reordering_algorithms/utils/utils.cpp)

add_library(mpicartreorderlib SHARED
        ${REORDERING_ALGORITHMS}
        ${SETUP_CONFIGURATION})

set_target_properties(mpicartreorderlib PROPERTIES SOVERSION 0)
set_target_properties(mpicartreorderlib PROPERTIES PUBLIC_HEADER include/mpicartreorder.h)