cmake_minimum_required(VERSION 3.0)

project(cart_test)
find_package(MPI REQUIRED)

SET(CMAKE_C_COMPILER mpicc)
SET(MY_COMPILE_FLAGS "-O3")

include_directories(../include/)
link_directories(../lib/)

add_executable(cart_test cart_test.c)
target_link_libraries(cart_test mpicartreorderlib)
