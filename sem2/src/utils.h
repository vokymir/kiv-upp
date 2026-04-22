/**
 * Kostra druhe semestralni prace z predmetu KIV/UPP
 * Soubory a hlavicku upravujte dle sveho uvazeni a nutnosti
 */

#pragma once

#include "workers.h"
#include <mpi.h>
#include <string>

namespace utils {

// return {N, M} from the args
// may THROW on invalid args
std::tuple<int, int> parse_args(int argc, char **argv);

// precte cely soubor do retezce
// path - cesta k souboru
// vraci obsah souboru nebo prazdny retezec v pripade chyby
std::string readWholeFile(const std::string &path);

// stahne HTML kod stranky z dane URL
// url - adresa stranky
// vraci obsah stranky nebo prazdny retezec v pripade chyby
std::string downloadHTML(const std::string &url);

namespace mpi {
// some functions are one-liners (send_int), but they are useful thanks to the
// same function syntax

// NOTE: for simpler use - different contexts, so clash is not likely
using namespace worker::_detail;

// =====
// === BASIC DATA TYPES

// send any integer to some MPI worker
void send_int(int num, int dest, int tag);

// receive an integer
// IF src is MPI_ANY_SOURCE, it will be filled with actual worker ID
int recv_int(int &src, int tag);

// send any string to some MPI worker
void send_string(const std::string &s, int dest, int tag);

// receive a string
// IF src is MPI_ANY_SOURCE, it will be filled with actual worker ID
std::string recv_string(int &src, int tag);

// =====
// === CONTAINERS

// send any vector
// REQUIRES a function for given data type
template <typename T, typename T_send_fn>
void send_vector(const std::vector<T> &v, int dest, int tag, T_send_fn fn) {
  int size = static_cast<int>(v.size());
  send_int(size, dest, tag);

  for (const auto &elem : v) {
    fn(elem, dest, tag);
  }
}

// receive any vector
// IF src is MPI_ANY_SOURCE, it will be filled with actual worker ID
// REQUIRES a function for given data type
template <typename T, typename T_recv_fn>
std::vector<T> recv_vector(int &src, int tag, T_recv_fn fn) {
  int size = recv_int(src, tag);

  std::vector<T> v;
  v.reserve(size);

  for (int i = 0; i < size; ++i) {
    v.push_back(fn(src, tag));
  }

  return v;
}

// =====
// === CUSTOM STRUCTS

// == NEEDED FOR B

// send Result_B to dest. tag is already set to correct tag
void send_result_B(const Result_B &r, int dest);

// receive Result_B, tag is already set to correct tag
// IF src is MPI_ANY_SOURCE, it will be filled with actual worker ID
Result_B recv_result_B(int &src);

// == NEEDED FOR A

// send heading to dest
void send_heading(const Heading &h, int dest, int tag);

// receive heading
// IF src is MPI_ANY_SOURCE, it will be filled with actual worker ID
Heading recv_heading(int &src, int tag);

// send reference to dest
void send_reference(const Reference &h, int dest, int tag);

// receive reference
// IF src is MPI_ANY_SOURCE, it will be filled with actual worker ID
Reference recv_reference(int &src, int tag);

// send page_content to dest
void send_page_content(const Page_Content &h, int dest, int tag);

// receive page_content
// IF src is MPI_ANY_SOURCE, it will be filled with actual worker ID
Page_Content recv_page_content(int &src, int tag);

// send log_entry to dest
void send_log_entry(const Log_Entry &h, int dest, int tag);

// receive log_entry
// IF src is MPI_ANY_SOURCE, it will be filled with actual worker ID
Log_Entry recv_log_entry(int &src, int tag);

// send website graph to dest
void send_graph(const Website_Graph &h, int dest, int tag);

// receive website graph
// IF src is MPI_ANY_SOURCE, it will be filled with actual worker ID
Website_Graph recv_graph(int &src, int tag);

// send result_A to dest
void send_result_A(const Result_A &h, int dest, int tag);

// receive result_A
// IF src is MPI_ANY_SOURCE, it will be filled with actual worker ID
Result_A recv_result_A(int &src, int tag);

} // namespace mpi

} // namespace utils
