#pragma once

#include <string>
#include <vector>
namespace crawl::worker {

// return value = program return value
int master();

void master_process(const std::vector<std::string> &urls, std::string &output);

void non_master(int rank, int argc, char **argv);

void A();

void B();

namespace _detail {

std::tuple<int, int> parse_args(int argc, char **argv);

}

} // namespace crawl::worker
