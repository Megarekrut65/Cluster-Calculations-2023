#include "mpi.h"
#include <iostream>
#include <string>
#include <chrono>
#include <sstream>

void generate_vector(int* vector, int size) {
	for (int i = 0; i < size; i++) {
		vector[i] = rand() % 90 + 10;
	}
}

std::string vtos(int* vector, int size) {
	std::stringstream out;
	for (int i = 0; i < size; i++) {
		out << vector[i] << " ";
	}
	out << "\n";

	return out.str();
}

void print_vector(int* vector, int size) {
	std::cout << vtos(vector, size);
}
void print_matrix(int* matrix, int rows, int cols = -1) {
	if (cols == -1) cols = rows;

	std::string res = "";
	for (int i = 0; i < rows; i++) {
		res += vtos(matrix + i * cols, cols);
	}
	std::cout << res +  "\n";
}

void init_data(int* &matrix, int* &vector, int* &result, int* & process_rows, int* & process_result,
	int size, int row_number) {
	matrix = new int [size * size] {};
	vector = new int[size] {};
	result = new int[size] {};
	process_rows = new int[row_number *size] {};
	process_result = new int[row_number] {};
}

void generate_data(int* matrix, int* vector, int size) {
	generate_vector(matrix, size * size);
	generate_vector(vector, size);
}

void free_data(int* matrix, int* vector, int* result, int* process_rows, int* process_result) {
	delete[] matrix;
	delete[] vector;
	delete[] result;
	delete[] process_rows;
	delete[] process_result;
}

void calculation(int* matrix, int* vector, int* result, int size) {
	for (int i = 0; i < size; i++) {
		result[i] = 0;
		for (int j = 0; j < size; j++)
			result[i] += matrix[i * size + j] * vector[j];
	}
}

void distribution(int rank, int number, int* matrix, int* process_rows, int* vector, int size) {

	MPI_Bcast(vector, size, MPI_INT, 0, MPI_COMM_WORLD); 
	
	int* send_indexes = new int[number];
	int* send_number = new int[number];

	int rest = size;
	int row_number = (size / number);

	send_number[0] = row_number * size;
	send_indexes[0] = 0;

	for (int i = 1; i < number; i++) {
		rest -= row_number;
		row_number = rest / (number - i);
		send_number[i] = row_number * size;
		send_indexes[i] = send_indexes[i - 1] + send_number[i - 1];
	}

	MPI_Scatterv(matrix, send_number, send_indexes, MPI_INT, process_rows,
		send_number[rank], MPI_INT, 0, MPI_COMM_WORLD);

	delete[] send_number;
	delete[] send_indexes;
}

void parallel_calculation(int* process_rows, int* vector, int* process_result,
	int size, int row_number) {

	for (int i = 0; i < row_number; i++) {
		process_result[i] = 0;
		for (int j = 0; j < size; j++) {
			process_result[i] += process_rows[i * size + j] * vector[j];
		}
	}
}

void print_data(int rank, int number, int* process_rows, int row_number, int size) {
	for (int i = 0; i < number; i++) {
		if (rank == i) {
			std::cout << "Rank: " << rank << std::endl;

			print_matrix(process_rows, row_number, size);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
}
void print_result(int rank, int number, int* process_result, int row_number, int size) {
	for (int i = 0; i < number; i++) {
		if (rank == i) {
			std::cout << "Rank: " << rank << std::endl;
			print_vector(process_result, row_number);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
}
void result_replication(int rank, int number, int* process_result, int* result, int size) {

	int* recive_number = new int[number];
	int* recive_indexes = new int[number];

	int rest = size;

	recive_indexes[0] = 0;
	recive_number[0] = size / number;

	for (int i = 1; i < number; i++) {
		rest -= recive_number[i - 1];
		recive_number[i] = rest / (number - i);
		recive_indexes[i] = recive_indexes[i - 1] + recive_number[i - 1];

	}

	MPI_Allgatherv(process_result, recive_number[rank], MPI_INT, result,
		recive_number, recive_indexes, MPI_INT, MPI_COMM_WORLD);

	delete[] recive_number;
	delete[] recive_indexes;
}

bool check_correct(int* matrix, int* vector, int* result, int size) {
	int* serial_result = new int[size];
	calculation(matrix, vector, serial_result, size);

	for (int i = 0; i < size; i++) {
		if (serial_result[i] != result[i]) return false;
	}

	return true;
}
int main(int argc, char** argv) {
	//srand(time(0));

	int size = 5;

	if (argc > 1) {
		size = std::stoi(argv[1]);
	}
	bool print_all = true;
	if (argc > 2) {
		print_all = std::stoi(argv[2]);
	}

	int number, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &number);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int row_number = ceil((double)size / number);

	int* matrix, *vector, *result, *process_rows, *process_result;

	int rest = size;
	for (int i = 0; i < rank; i++) rest = rest - rest / (number - i);
	
	row_number = rest / (number - rank);

	init_data(matrix, vector, result, process_rows, process_result, size, row_number);

	if (rank == 0) {
		generate_data(matrix, vector, size);
		std::cout << "Size: " << size << std::endl;
	}
	if (print_all && rank == 0) {

		std::cout << "Matrix:\n";
		print_matrix(matrix, size);
		std::cout << "Vector:\n";
		print_vector(vector, size);
	}

	double start = MPI_Wtime();

	distribution(rank, number, matrix, process_rows, vector, size);
	MPI_Barrier(MPI_COMM_WORLD);
	parallel_calculation(process_rows, vector, process_result, size, row_number);
	result_replication(rank, number, process_result, result, size);

	double finish = MPI_Wtime();

	if (print_all && rank == 0) {
		std::cout << "\nResult:\n";
		print_vector(result, size);
	}
	
	if (rank == 0) {
		bool correct = check_correct(matrix, vector, result, size);

		std::cout << "\nMultiplication is " << (correct?"correct":"incorrect") << "!\n";

		double duration = finish - start;
		std::cout << "\nTime: " << duration << "s\n";
	}

	MPI_Finalize();

	free_data(matrix, vector, result, process_rows, process_result);
	return 0;
}