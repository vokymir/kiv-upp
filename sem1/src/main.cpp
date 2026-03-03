#include "loader.hpp"
#include <cstddef>
#include <print>
#include <string_view>

using namespace std::literals;

int main(int argc, char **argv) {
  bool parallel = false;

  for (int i = 1; i < argc; ++i) {
    if (argv[i] == "--parallel"sv || argv[i] == "-p"sv) {
      parallel = true;
    }
  }

  std::print("{}\n", parallel ? "Is parallel" : "Is serial");

  auto value = chmu::load_serial("./data/stanice.csv", "./data/mereni.csv");

  std::print("stations size = {}\n", value->stations.size());

  size_t sum = 0;
  for (auto &s : value->stations) {
    sum += s.measurements().size();
  }

  std::print("measurements size = {}\n", sum);

  return 0;
}
