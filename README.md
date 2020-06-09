# mpicartreorderlib - A Tool for Process Reordering of Cartesian MPI Communicators
Library with rank reordering algorithms for the **MPI_Cart_comm** routine with set reordering flag.

![alt text](https://github.com/konradvonkirchbach/mpicartreorderlib/blob/master/images/motivational_example-1.png "Motivational Example")

## Introduction
This dynamic library aims at helping the user reduce the amount of communication between compute nodes (**inter-node communication**) in applications that make use of the **Cartesian communicator** in **MPI** with minimal changes of the codebase. Users can parse different parameters via environment variables, so that the input of the programm remains unchanged.

## Installation and Usage 
We use CMake for installation of the library. We recommend to make a built directory and call CMake from within. On UNIX systems just execute the following instructions.
```
mkdir build
cd build
cmake ..
make
make install
```
Once the library is installed, one 
