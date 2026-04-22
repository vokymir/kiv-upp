#pragma once

#include <string>
#include <vector>
namespace crawl::worker {

// return value = program return value
// run the server
int master(int N, int M);

// decide based on rank, N and M
// if A or B should run
void non_master(int rank, int N, int M);

void A();

void B();

namespace _detail {

// === TAGS used for MPI communication

constexpr int TAG_URL = 1;
constexpr int TAG_RESULT = 2;

// process list of URLs using MPI and write informative html into output
void process(int N, int M, const std::vector<std::string> &urls,
             std::string &output);

} // namespace _detail

} // namespace crawl::worker
