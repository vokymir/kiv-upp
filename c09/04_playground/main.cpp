#include <iostream>
#include <mpi.h>

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int rank, total;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &total);

	// TADY JE PROSTOR PRO VAS PROGRAM

	MPI_Finalize();
	return 0;
}
