/**
 * Kostra druhe semestralni prace z predmetu KIV/UPP
 * Soubory a hlavicku upravujte dle sveho uvazeni a nutnosti
 */

#include <mpi.h>

#include "utils.h"
#include "workers.h"

int main(int argc, char **argv) {

  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int retval = EXIT_SUCCESS;
  auto [N, M] = utils::parse_args(argc, argv);

  if (rank == 0) {
    retval = crawl::worker::master(N);
  } else {
    crawl::worker::non_master(rank, N, M);
  }

  MPI_Finalize();
  return retval;
}
