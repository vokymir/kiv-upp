/**
 * Kostra druhe semestralni prace z predmetu KIV/UPP
 * Soubory a hlavicku upravujte dle sveho uvazeni a nutnosti
 */

#include <mpi.h>

#include "config.h"
#include "utils.h"
#include "workers.h"

int main(int argc, char **argv) {
  int retval = EXIT_SUCCESS;

  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  auto [N, M] = utils::parse_args(argc, argv);
  cfg::N = N;
  cfg::M = M;

  if (cfg::is_master(rank)) {
    retval = worker::master();
  } else {
    worker::non_master(rank);
  }

  MPI_Finalize();
  return retval;
}
