#include "averager.hpp"
#include "filter.hpp"
#include "fluctuation.hpp"
#include "loader.hpp"
#include "stations.hpp"
#include <chrono>
#include <exception>
#include <memory>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>

using namespace std::literals;

constexpr std::string DEFAULT_STATIONS_PATH = "stanice.csv";
constexpr std::string DEFAULT_MEASUREMENTS_PATH = "mereni.csv";
constexpr bool DEFAULT_IS_PARALLEL = false;

constexpr int ERROR_ARGUMENTS = 1;
constexpr int ERROR_EXECUTION = 2;
constexpr int ERROR_UNKNOWN = 3;

struct Timer {
  using clock = std::chrono::high_resolution_clock;
  clock::time_point start = clock::now();

  void lap(const std::string_view &comment) {
    auto now = clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    std::print("[{}] {}\n", elapsed, comment);
    start = now;
  }
};

void print_usage() {
  std::print(
      "\nUsage:\n./upp_sp1 [path to stations file] [path to measurements "
      "file] [--serial | --parallel]\n\nThe defaults are:\n./upp_sp1 "
      "{} {} --{}\n\nEither enter zero or all parameters.",
      DEFAULT_STATIONS_PATH, DEFAULT_MEASUREMENTS_PATH,
      DEFAULT_IS_PARALLEL ? "parallel" : "serial");
}

void load_args(int argc, char **argv, std::string &stations_path,
               std::string &measurements_path, bool &is_parallel) {
  // use provided parameters
  if (argc == 4) {
    stations_path = argv[1];
    measurements_path = argv[2];

    if (argv[3] == "--parallel"sv) {
      is_parallel = true;
    } else if (argv[3] == "--serial"sv) {
      is_parallel = false;
    } else {
      throw std::runtime_error("Invalid serial/parallel flag.");
    }

  } else if (argc == 1) {
    // use defaults

  } else {
    throw std::runtime_error("Invalid number of arguments.");
  }
}

void print_info(const chmu::Stations &sts) {
  std::print("Stations: {}\n", sts.stations.size());
}

void serial_version(const std::string_view &stations_path,
                    const std::string_view &measurements_path) {
  Timer timer;

  // 0. load data
  std::unique_ptr<chmu::Stations> stations =
      chmu::load__serial(stations_path, measurements_path);
  timer.lap("Data loaded.");

  // 1. pre-process data (filtration)
  chmu::filter__serial(*stations);
  timer.lap("Data filtered.");

  // 3. calculate averages
  chmu::compute_averages__serial(*stations);
  timer.lap("Averages computed.");

  // 2. identify fluctuation
  chmu::identify_fluctuation__serial(*stations);
  timer.lap("Fluctuations identified.");

  // 4. draw a map for each month

  // 5. create a CSV output file
}

void parallel_version(const std::string_view &stations_path,
                      const std::string_view &measurements_path) {}

int main(int argc, char **argv) {
  std::string stations_path = DEFAULT_STATIONS_PATH;
  std::string measurements_path = DEFAULT_MEASUREMENTS_PATH;
  bool parallel = DEFAULT_IS_PARALLEL;

  try {
    load_args(argc, argv, stations_path, measurements_path, parallel);

  } catch (const std::exception &ex) {
    std::print("ERROR: {}\n", ex.what());
    print_usage();
    return ERROR_ARGUMENTS;

  } catch (...) {
    std::print("Unknown error happened...");
    print_usage();
    return ERROR_UNKNOWN;
  }

  try {
    if (parallel) {
      parallel_version(stations_path, measurements_path);
    } else {
      serial_version(stations_path, measurements_path);
    }

  } catch (const std::exception &ex) {
    std::print("ERROR: {}\n", ex.what());
    return ERROR_EXECUTION;

  } catch (...) {
    std::print("Unknown error happened...");
    return ERROR_UNKNOWN;
  }

  return 0;
}
