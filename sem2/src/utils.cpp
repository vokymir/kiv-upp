/**
 * Kostra druhe semestralni prace z predmetu KIV/UPP
 * Soubory a hlavicku upravujte dle sveho uvazeni a nutnosti
 */

#include "workers.h"
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

void send_int(int num, int dest, int tag) {
  MPI_Send(&num, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
}

int recv_int(int &src, int tag) {
  MPI_Status status;
  int num;

  MPI_Recv(&num, 1, MPI_INT, src, tag, MPI_COMM_WORLD, &status);
  if (src == MPI_ANY_SOURCE) {
    src = status.MPI_SOURCE;
  }

  return num;
}

void send_string(const std::string &s, int dest, int tag) {
  int len = static_cast<int>(s.size());

  send_int(len, dest, tag);
  MPI_Send(s.c_str(), len, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
}

std::string recv_string(int &src, int tag) {
  int len = recv_int(src, tag);

  std::string msg(len, '\0');
  MPI_Recv(&msg[0], len, MPI_CHAR, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  return msg;
}

void send_result_B(const worker::_detail::Result_B &r, int dest) {
  int tag = worker::_detail::TAG_RESULT_B;

  // = URL
  send_string(r.page.url, dest, tag);

  // = INTEGERS
  send_int(r.page.imgs, dest, tag);
  send_int(r.page.links, dest, tag);
  send_int(r.page.forms, dest, tag);

  // = FOUND PAGES
  send_vector<std::string>(r.found_pages, dest, tag, send_string);
}

worker::_detail::Result_B recv_result_B(int &src) {
  int tag = worker::_detail::TAG_RESULT_B;

  worker::_detail::Result_B r;

  // = URL
  r.page.url = recv_string(src, tag);

  // = INTEGERS
  r.page.imgs = recv_int(src, tag);
  r.page.links = recv_int(src, tag);
  r.page.forms = recv_int(src, tag);

  // = FOUND PAGES
  r.found_pages = recv_vector<std::string>(src, tag, recv_string);

  return r;
}

} // namespace mpi

} // namespace utils
