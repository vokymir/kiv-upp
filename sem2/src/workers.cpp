
#include "workers.h"
#include "config.h"
#include "server.h"
#include "utils.h"
#include <iostream>
#include <mpi.h>
namespace crawl::worker {

int master() {

  // inicializace serveru
  CServer svr;
  if (!svr.Init("./data", "0.0.0.0", 8001)) {
    std::cerr << "Nelze inicializovat server!" << std::endl;
    return EXIT_FAILURE;
  }

  // registrace callbacku pro zpracovani odeslanych URL
  // these shananigans are only to avoid global state/config which would hold
  // M and N
  svr.RegisterFormCallback(_detail::process);

  // spusteni serveru
  return svr.Run() ? EXIT_SUCCESS : EXIT_FAILURE;
}

void non_master(int rank) {
  int N = crawl::cfg::N;

  if (rank <= N) {
    A();

  } else {
    B();
  }
}

void A() {

  while (true) {

    int employer = MPI_ANY_SOURCE;
    std::string url = utils::mpi::recv_string(employer, _detail::TAG_URL);

    // TODO: PROCESS using B, stack & queue etc

    std::string result = "Zpracoval jsem: " + url;

    utils::mpi::send_string(result, employer, _detail::TAG_RESULT);
  }
}

void B() {}

namespace _detail {

void process(const std::vector<std::string> &urls, std::string &output) {
  int N = crawl::cfg::N;
  int M = crawl::cfg::M;

  // divide the work
  for (int i = 0; i < static_cast<int>(urls.size()); ++i) {
    int worker = 1 + (i % N);
    utils::mpi::send_string(urls[i], worker, TAG_URL);
  }

  // conquer the results
  output = "Zadali jste: <ul>";
  for (int i = 0; i < urls.size(); i++) {
    int src = MPI_ANY_SOURCE;
    std::string res = utils::mpi::recv_string(src, TAG_RESULT);
    output += res;
  }
  //   for (int i = N + 1; i <= N + M; ++i) {
  //     std::string res = utils::mpi::recv_string(i, TAG_RESULT);
  //     output += "<li>" + res + "</li>";
  //   }
  output += "</ul>";
}

} // namespace _detail

} // namespace crawl::worker
