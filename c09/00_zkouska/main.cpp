#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <ctime>

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);

	int rank, total;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	// jake je moje ID?
	MPI_Comm_size(MPI_COMM_WORLD, &total);	// kolik nas je celkem v clusteru?
	
	std::cout << "Jsem cislo " << rank << " z celkovych " << total << std::endl;
	
	size_t cislo = 0;
	
	// pokud jsme root (dohodou stanoveno na rank == 0), vymyslime si cislo a rozposleme ho vsem
	if (rank == 0) {
		srand(time(nullptr));
		cislo = rand() % 100;
		std::cout << "Jsem root a vymyslel jsem si cislo " << cislo << std::endl;
	}
	
	// broadcast -- root odesila (to je ta nula na 4. parametru), vsichni kdo nejsou root prijimaji
	// MPI ma interne reprezentovane datove typy, proto musime explicitne rict, ze posilane MPI_UNSIGNED_LONG a ne obecnou pamet
	MPI_Bcast(&cislo, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
	
	if (rank != 0) {
		std::cout << "Nejsem root a prijal jsem cislo " << cislo << std::endl;
	}
	
	MPI_Finalize();

	return 0;
}
