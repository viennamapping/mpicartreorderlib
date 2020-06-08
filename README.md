# mpicartreorderlib - A Tool for Process Reordering of Cartesian MPI Communicators
Library with rank reordering algorithms for the **MPI_Cart_comm** reorder flag.
![alt text](https://github.com/konradvonkirchbach/mpicartreorderlib/images/motivational_example.pdf "Motivational Example")

## Introduction
This dynamic library aims at helping the user reduce the amount of communication between compute nodes (**inter-node communication**) in applications that make use of the **Cartesian communicator** in **MPI** with minimal changes of the codebase. Users can parse different parameters to via environment variables, so that the input of the programm remains unchanged.
