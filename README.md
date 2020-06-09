# mpicartreorderlib - A Tool for Rank Reordering of Cartesian MPI Communicators
Library with rank reordering algorithms for the **MPI_Cart_create** routine with set reordering flag.

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
