#include <mpi.h>
#include <stdio.h>

#include "mpicartreorderlib/include/mpicartreorder.h"

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	MPI_Comm cart;
	
	int w_size, rank, ndims = 2, dims[ndims], periods[ndims];
	for (int i = 0; i < ndims; i++) {
		dims[i] = 0;
		periods[i] = 0;
	}
	MPI_Comm_size(MPI_COMM_WORLD, &w_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Dims_create(w_size, ndims, dims);
	MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, 1, &cart);

	int total = 0, max = 0;
	MPIX_Internode_cost(cart, &total, &max);

	if (rank == 0) printf("Total cost = %d. Max cost = %d.\n", total, max);

	MPI_Finalize();
}
