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

// send Result_B to dest. tag is already set to correct tag
void send_result_B(const worker::_detail::Result_B &r, int dest);

// receive Result_B, tag is already set to correct tag
// IF src is MPI_ANY_SOURCE, it will be filled with actual worker ID
worker::_detail::Result_B recv_result_B(int &src);

} // namespace mpi

} // namespace utils
