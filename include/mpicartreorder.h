#ifdef __cplusplus
extern "C" 
#endif
int MPIX_Cart_comm_stencil (MPI_Comm comm_old, int ndims, const int dims[],
		const int periods[], int reorder, const int stencil[], 
		const int n_neighbors, MPI_Comm *comm_cart);
