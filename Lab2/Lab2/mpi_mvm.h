#pragma once

#include "mpi.h"
#include "utilities.h"

//creates matrices
void init_data(int*& matrix1_block, int*& block1, int*& block2, int*& block_result,int block_size);

//delete matrices
void free_data(int* matrix1_block, int* block1, int* block2, int* block_result);

void create_grid_communicators(int grid_size, MPI_Comm* grid_comm, MPI_Comm* row_comm,
	MPI_Comm* col_comm, int grid_coords[2], int rank);


void checkerboard_matrix_scatter(int* matrix, int* matrix_block, int size,
	int block_size, int grid_coords[2], MPI_Comm* row_comm, MPI_Comm* col_comm);

void data_distribution(int* matrix1, int* matrix2,
	int* matrix1_block, int* block2, int size, int block_size,
	int grid_coords[2], MPI_Comm* row_comm, MPI_Comm* col_comm);