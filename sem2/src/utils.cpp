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
// ============================================================================
// NOTE: important detail:
using namespace worker::_detail;

// =====
// === BASIC DATA TYPES

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

// =====
// === CUSTOM STRUCTS

// == NEEDED FOR B

void send_result_B(const Result_B &r, int dest) {
  int tag = TAG_RESULT_B;

  // = URL
  send_string(r.page.url, dest, tag);

  // = INTEGERS
  send_int(r.page.imgs, dest, tag);
  send_int(r.page.links, dest, tag);
  send_int(r.page.forms, dest, tag);

  // = FOUND PAGES
  send_vector<std::string>(r.found_pages, dest, tag, send_string);
}

Result_B recv_result_B(int &src) {
  int tag = TAG_RESULT_B;

  Result_B r;

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

// == NEEDED FOR A

void send_heading(const Heading &h, int dest, int tag) {
  send_int(h.depth, dest, tag);
  send_string(h.text, dest, tag);
}

Heading recv_heading(int &src, int tag) {
  Heading h;

  h.depth = recv_int(src, tag);
  h.text = recv_string(src, tag);

  return h;
}

void send_reference(const Reference &r, int dest, int tag) {
  send_string(r.origin, dest, tag);
  send_string(r.target, dest, tag);
}

Reference recv_reference(int &src, int tag) {
  Reference r;

  r.origin = recv_string(src, tag);
  r.target = recv_string(src, tag);

  return r;
}

void send_page_content(const Page_Content &p, int dest, int tag) {
  send_string(p.url, dest, tag);

  send_int(p.imgs, dest, tag);
  send_int(p.links, dest, tag);
  send_int(p.forms, dest, tag);

  send_vector(p.headings, dest, tag, send_heading);
}

Page_Content recv_page_content(int &src, int tag) {
  Page_Content p;

  p.url = recv_string(src, tag);

  p.imgs = recv_int(src, tag);
  p.links = recv_int(src, tag);
  p.forms = recv_int(src, tag);

  p.headings = recv_vector<Heading>(src, tag, recv_heading);

  return p;
}

void send_log_entry(const Log_Entry &l, int dest, int tag) {
  auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(
                l.time.time_since_epoch())
                .count();

  send_int(static_cast<int>(ts), dest, tag);
  send_string(l.msg, dest, tag);
}

Log_Entry recv_log_entry(int &src, int tag) {
  Log_Entry l;

  int ts = recv_int(src, tag);
  l.time = std::chrono::system_clock::time_point(std::chrono::milliseconds(ts));

  l.msg = recv_string(src, tag);
  return l;
}

void send_graph(const Website_Graph &g, int dest, int tag) {
  send_vector(g.uris, dest, tag, send_string);
  send_vector(g.refs, dest, tag, send_reference);
}

Website_Graph recv_graph(int &src, int tag) {
  Website_Graph g;

  g.uris = recv_vector<std::string>(src, tag, recv_string);
  g.refs = recv_vector<Reference>(src, tag, recv_reference);

  return g;
}

void send_result_A(const Result_A &r, int dest, int tag) {
  send_graph(r.graph, dest, tag);

  send_vector(r.contents, dest, tag, send_page_content);
  send_vector(r.log, dest, tag, send_log_entry);
}

Result_A recv_result_A(int &src, int tag) {
  Result_A r;

  r.graph = recv_graph(src, tag);

  r.contents = recv_vector<Page_Content>(src, tag, recv_page_content);
  r.log = recv_vector<Log_Entry>(src, tag, recv_log_entry);

  return r;
}

} // namespace mpi

} // namespace utils
