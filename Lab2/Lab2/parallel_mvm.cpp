#include "parallel_mvm.h"



void mpi_multiply(int* matrix1, int* matrix2, int* result, int size) {

	int number, rank, grid_size;
	MPI_Comm grid_comm, row_comm, col_comm;
	int grid_coords[2];

	MPI_Comm_size(MPI_COMM_WORLD, &number);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	grid_size = sqrt((double)number); 
	if (number != grid_size * grid_size) {
		if (rank == 0) std::cout << "Number of processes must be a perfect square\n";
		return;
	}
	if (rank == 0) std::cout << "Parallel matrix multiplication program\n";
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	create_grid_communicators(grid_size, &grid_comm, &row_comm, &col_comm, grid_coords, rank);

	int block_size = size/grid_size;
	int* matrix1_block;
	int* block1;
	int* block2;
	int* block_result;

	init_data(matrix1_block, block1, block2, block_result, block_size);
	data_distribution(matrix1, matrix2, matrix1_block, block2, size, block_size, grid_coords, &row_comm, &col_comm);

	MPI_Barrier(MPI_COMM_WORLD);

	for (int i = 0; i < number; i++) {
		if (rank == i) {
			std::cout << "rank " << rank << std::endl;
			print_matrix(block2, block_size);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	free_data(matrix1_block, block1, block2, block_result);
}