
#include "workers.h"
#include "server.h"
#include <iostream>
#include <mpi.h>
namespace crawl::worker {

int master(int N) {

  // inicializace serveru
  CServer svr;
  if (!svr.Init("./data", "0.0.0.0", 8001)) {
    std::cerr << "Nelze inicializovat server!" << std::endl;
    return EXIT_FAILURE;
  }

  // registrace callbacku pro zpracovani odeslanych URL
  // these shananigans are only to avoid global state/config which would hold
  // M and N
  svr.RegisterFormCallback(
      [N](const std::vector<std::string> &urls, std::string &output) {
        _detail::process(N, urls, output);
      });

  // spusteni serveru
  return svr.Run() ? EXIT_SUCCESS : EXIT_FAILURE;
}

void non_master(int rank, int N, int M) {

  if (rank <= N) {
    A();

  } else {
    B();
  }
}

void A() {}

void B() {}

namespace _detail {

void process(int N, const std::vector<std::string> &urls, std::string &output) {

      // TODO

      output = "Zadali jste: <ul>";
  for (const auto &url : urls) {
    output += "<li>" + url + "</li>";
  }
  output += "</ul>";
}

} // namespace _detail

} // namespace crawl::worker
