#include "loader.hpp"
#include <cstddef>
#include <print>
#include <string>
#include <string_view>

using namespace std::literals;

int main(int argc, char **argv) {
  bool parallel = false;
  std::string dataset_path = "./data/";

  for (int i = 1; i < argc; ++i) {
    if (argv[i] == "--parallel"sv || argv[i] == "-p"sv) {
      parallel = true;
    }
    if (argv[i] == "--s"sv) {
      dataset_path += "small/";
    }
    if (argv[i] == "-m"sv) {
      dataset_path += "medium/";
    }
    if (argv[i] == "-l"sv) {
      dataset_path += "large/";
    }
  }
  if (dataset_path.size() == 7) {
    dataset_path += "small/";
  }

  std::print("{}\n", parallel ? "Is parallel" : "Is serial");

  auto value = chmu::load_serial(dataset_path + "stanice.csv",
                                 dataset_path + "mereni.csv");

  std::print("stations size = {}\n", value->stations.size());

  size_t sum = 0;
  for (auto &s : value->stations) {
    sum += s.measurements().size();
  }

  std::print("measurements size = {}\n", sum);

  return 0;
}
