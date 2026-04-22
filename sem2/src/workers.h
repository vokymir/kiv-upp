#pragma once

#include <string>
#include <vector>
namespace crawl::worker {

// return value = program return value
// run the server
int master(int N);

// decide based on rank, N and M
// if A or B should run
void non_master(int rank, int N, int M);

void A();

void B();

namespace _detail {

// process list of URLs using MPI and write informative html into output
void process(int N, const std::vector<std::string> &urls, std::string &output);

} // namespace _detail

} // namespace crawl::worker
