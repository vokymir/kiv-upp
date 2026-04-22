
#include "workers.h"
#include "server.h"
#include <iostream>
namespace crawl::worker {

int master() {

  // inicializace serveru
  CServer svr;
  if (!svr.Init("./data", "0.0.0.0", 8001)) {
    std::cerr << "Nelze inicializovat server!" << std::endl;
    return EXIT_FAILURE;
  }

  // registrace callbacku pro zpracovani odeslanych URL
  svr.RegisterFormCallback(_detail::process);

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

void process(const std::vector<std::string> &urls, std::string &output) {

  // TODO

  output = "Zadali jste: <ul>";
  for (const auto &url : urls) {
    output += "<li>" + url + "</li>";
  }
  output += "</ul>";
}

} // namespace _detail

} // namespace crawl::worker
