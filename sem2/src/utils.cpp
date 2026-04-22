/**
 * Kostra druhe semestralni prace z predmetu KIV/UPP
 * Soubory a hlavicku upravujte dle sveho uvazeni a nutnosti
 */

#ifdef USE_SSL
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif
#include "../dep/cpp-httplib/httplib.h"

#include "utils.h"
#include <mpi.h>

namespace utils {

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

std::string readWholeFile(const std::string &path) {
  // otevreni souboru
  std::ifstream ifs(path);
  if (!ifs) {
    return "";
  }

  // nacteni obsahu souboru
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));
  return content;
}

std::string downloadHTML(const std::string &url) {

  std::string scheme;
  std::string rest;

  // extrahovani schematu a zbytku URL
  if (url.substr(0, 7) == "http://") {
    scheme = "http";
    rest = url.substr(7);
  } else if (url.substr(0, 8) == "https://") {
    scheme = "https";
    rest = url.substr(8);
  } else {
    return ""; // nezname schema
  }

  size_t pos = rest.find("/");
  std::string domain = rest.substr(0, pos);
  std::string path = rest.substr(pos);

  // stahne obsah stranky - pouzije SSL klienta, pokud je pozadovana podpora SSL
#ifdef USE_SSL
  httplib::SSLClient cli(domain.c_str());
  cli.enable_server_certificate_verification(false);
  cli.enable_server_hostname_verification(false);
#else
  httplib::Client cli(domain.c_str());
#endif

  cli.set_follow_location(true);
  auto res = cli.Get(path.c_str());

  if (!res || res->status != 200) {
    std::cerr << "Chyba: " << res->status << std::endl;
    return "";
  }

  return res->body;
}

namespace mpi {

void send_string(const std::string &s, int dest, int tag) {
  int len = static_cast<int>(s.size());

  MPI_Send(&len, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
  MPI_Send(s.c_str(), len, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
}

std::string recv_string(int &src, int tag) {
  MPI_Status status;
  int len;

  MPI_Recv(&len, 1, MPI_INT, src, tag, MPI_COMM_WORLD, &status);

  // caller didn't know the source - now they do!
  if (src == MPI_ANY_SOURCE) {
    src = status.MPI_SOURCE;
  }

  std::string msg(len, '\0');
  MPI_Recv(&msg[0], len, MPI_CHAR, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  return msg;
}

} // namespace mpi

} // namespace utils
