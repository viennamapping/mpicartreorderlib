# mpicartreorderlib - A Tool for Rank Reordering of Cartesian MPI Communicators

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/eb87bc25602c49cc9708815cb436a6c0)](https://app.codacy.com/gh/viennamapping/mpicartreorderlib?utm_source=github.com&utm_medium=referral&utm_content=viennamapping/mpicartreorderlib&utm_campaign=Badge_Grade_Dashboard)
[![Build Status](https://travis-ci.org/viennamapping/mpicartreorderlib.svg?branch=master)](https://travis-ci.org/viennamapping/mpicartreorderlib)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/viennamapping/mpicartreorderlib.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/viennamapping/mpicartreorderlib/context:cpp)
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fviennamapping%2Fmpicartreorderlib.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2Fviennamapping%2Fmpicartreorderlib?ref=badge_shield)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Library with rank reordering algorithms for the **MPI_Cart_create** routine with set reordering flag. A description of the algorithms can be found in the following technical report https://arxiv.org/abs/2005.09521. If you make use of our library, please cite the technical report.

![alt text](https://github.com/konradvonkirchbach/mpicartreorderlib/blob/master/images/motivational_example-1.png "Motivational Example")

## Introduction
This dynamic library aims at helping the user reduce the amount of communication between compute nodes (**inter-node communication**) in applications that make use of the **Cartesian communicator** in **MPI** with minimal changes of the codebase. Users can parse different parameters via environment variables, so that the input of the programm remains unchanged.

## Installation and Usage
We use CMake for installation of the library. We recommend to make a build directory and call CMake from within. On UNIX systems just execute the following instructions.
```
mkdir build
cd build
cmake ..
make
make install
```
Once the library is installed, one can link it to the application but should make sure it is linked before **MPI**, since the library intercepts calls to **MPI_Cart_create**.

```mpicc *.c mympicode -lmpicartreorderlib -lmpi```

The user can specify the algorithm to perform the reordering by setting the following environment variable

```export CART_REORDER_ALGORITHM=<KD_TREE, HYPERPLANE, STENCIL_STRIPS>```

The default reordering algorithm is `STENCIL_STRIPS`. The algorithms are explained in detail in the paper.

### Stencils 
#### **MPI_Cart_create**
Using the this approach, no changes are needed to be done in the codebase, except setting the reordering flag to 1 in the `MPI_Cart_create` routine. By default, the stencil used as input for the reordering schemes is a nearest neighbor stencil, that is, a stencil that specifies the communication neighbors to be the two closest processes in each dimension. The user has the possibilty to specify a costum stencil with a flattened, '@'-separated list by setting `CART_REORDER_STENCIL`. For example, in a two dimensional case where each process should only communicate with its closest neighbors along the first dimension, one should set the stencil variable as follows:

```
export CART_REORDER_STENCIL=1@0@-1@0
```

#### **MPIX_Cart_comm_stencil**
We also included the possibilty to parse the stencil directly in the code. For that purpose, we included a wrapper routine with the signature

```
int MPIX_Cart_comm_stencil(MPI_Comm comm_old, int ndims, const int dims[],
                          const int periods[], int reorder, const int stencil[],
                          const int n_neighbors, MPI_Comm* comm_cart)
```

where `n_neighbors` specifies the number of communication neighbors, not the length of the stencil array, that is, n_neighbors is `sizeof(stencil)/ndims` and stencil is a flatted list of relative offset vectors.

### Node Aggregation Scheme

`STENCIL_STRIPS` and `HYPERPLANE` both use the number of processes per compute node (ppn) as input for reordering algorithm. In case of different number or processes per node, the user can specifiy an aggregation scheme for ppn:

```
export CART_REORDER_NODE_AGGREGATION=<NODES_MIN,NODES_MEAN,NODES_MAX>
```

### Logging

The user can also specify the log-level for the internal logger (spdlog https://github.com/gabime/spdlog) via the environmetal variables

```
export CART_REORDER_LOG_LEVEL=<0-6>
```

By default logging is turned off.

### Evaluation of Reordering Algorithms

We provide the possibility to evaluate the reordering schemes in terms of inter-node communication ammount, both the sum and in terms of the node with maximal number of inter-node communication (the bottleneck compute node). For that purpose, we included the two routines

```
void MPIX_Internode_cost(MPI_Comm cart_comm, int* total, int* max);
void MPIX_Internode_cost_stencil(MPI_Comm, cart_comm, int* total, int* max, int stencil[], int n_neighbors);
```

The communicator should have a Cartesian topology information attached to it. `int stencil[]` and `int n_neighbors` are defined as above. The total sum of inter-node communication is written in `total` and the maximum im `max`.

## Example

We included a small toy example in the test directory. The example should help illustrate the usage and installation of the library. The example consists of a two dimensional Cartesian grid where we instantiate the communicator and simply count the number of inter-node communication edges induced by it.
In order to compile it with the GNU compiler one can either link the library with `rpath`, such that it is not needed to indicate the linker the path to library when running the programming with

```
gcc cart_test.c -I /path to library/include -Wl,-rpath=/path to library/lib/ -lmpicartreorderlib -lmpi -L<path to library/lib/> -o cart_test
```

or one can link the library without `rpath` but then needs to specify the path to the library when launching the application

```
gcc cart_test.c -I /path to library/mpicartreorderlib/include/. -lmpicartreorderlib -lmpi -L/path to library/mpicartreorderlib/lib/ -ldl -o cart_test

env LD_LIBRARY_PATH=/path to library/lib/:$LD_LIBRARY_PATH <execution command>
```
The test application can also be compiled using CMake. In the CMakeLists.txt file, provided in test directory, one can set the path to the header files of the library via `include_directories(<path to header directory>)`. The linker needs to know where the library is found, this is specified with `link_directories(<path to lib directory>)`. As demonstrated in the CMakeLists.txt of the test directory, both accept relative paths.

## Licence

The program is licenced under MIT licence.
If you publish results using our algorithms, please acknowledge our work by quoting the following paper:

```
@article{DBLP:journals/corr/abs-2005-09521,
  author    = {Sascha Hunold and
               Konrad von Kirchbach and
               Markus Lehr and
               Christian Schulz and
               Jesper Larsson Tr{\"{a}}ff},
  title     = {Efficient Process-to-Node Mapping Algorithms for Stencil Computations},
  journal   = {CoRR},
  volume    = {abs/2005.09521},
  year      = {2020},
  url       = {https://arxiv.org/abs/2005.09521},
  archivePrefix = {arXiv},
  eprint    = {2005.09521},
  timestamp = {Fri, 22 May 2020 16:21:29 +0200},
  biburl    = {https://dblp.org/rec/journals/corr/abs-2005-09521.bib},
  bibsource = {dblp computer science bibliography, https://dblp.org}
}
```


[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fviennamapping%2Fmpicartreorderlib.svg?type=large)](https://app.fossa.com/projects/git%2Bgithub.com%2Fviennamapping%2Fmpicartreorderlib?ref=badge_large)
