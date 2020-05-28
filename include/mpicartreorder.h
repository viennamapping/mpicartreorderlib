#ifdef __cplusplus
#define NULL nullptr
extern "C" {
#endif
int MPIX_Cart_comm_stencil(MPI_Comm comm_old, int ndims, const int dims[],
						   const int periods[], int reorder, const int stencil[],
						   const int n_neighbors, MPI_Comm *comm_cart);

void MPIX_Internode_cost(MPI_comm cart_comm, int* total, int* max, int stencil[] = NULL, int n_neighbors = NULL);
void MPIX_Internode_cost(MPI_Comm cart_comm, int* total, int* max);
#ifdef __cplusplus
}
#endif
