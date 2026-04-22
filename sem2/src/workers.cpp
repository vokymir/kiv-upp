
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
  svr.RegisterFormCallback(master_process);

  // spusteni serveru
  return svr.Run() ? EXIT_SUCCESS : EXIT_FAILURE;
}

void master_process(const std::vector<std::string> &urls, std::string &output) {

  // TODO

  output = "Zadali jste: <ul>";
  for (const auto &url : urls) {
    output += "<li>" + url + "</li>";
  }
  output += "</ul>";
}

void non_master(int rank, int argc, char **argv) {

  auto [N, M] = _detail::parse_args(argc, argv);

  if (rank <= N) {
    A();

  } else {
    B();
  }
}

void A() {}

void B() {}

namespace _detail {

std::tuple<int, int> parse_args(int argc, char **argv) {

  int N = -1, M = -1;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i - 1];

    if (arg == "-n") {
      N = std::stoi(argv[i]);

    } else if (arg == "-m") {
      M = std::stoi(argv[i]);
    }
  }

  if (N <= 0 || M <= 0) {
    throw std::runtime_error("Unspecified/Wrong arguments for -n and/or -m.");
  }

  return {N, M};
}

} // namespace _detail

} // namespace crawl::worker
