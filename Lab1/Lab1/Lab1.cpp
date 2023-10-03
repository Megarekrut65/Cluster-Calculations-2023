#include <iostream>
#include "mpi.h"


int main(int args, char** argv)
{
    MPI_Init(&args, &argv);
    std::cout << "Hello world!\n";
    MPI_Finalize();
    return 0;
}
