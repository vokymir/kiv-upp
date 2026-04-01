#include "collect_stats.hpp"
#include "csv.hpp"
#include "draw.hpp"
#include "filter.hpp"
#include "fluctuation.hpp"
#include "loader.hpp"
#include "model/model.hpp"
#include "threadpool.hpp"
#include <chrono>
#include <exception>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

using namespace std::literals;

constexpr std::string DEFAULT_STATIONS_PATH = "stanice.csv";
constexpr std::string DEFAULT_MEASUREMENTS_PATH = "mereni.csv";
constexpr bool DEFAULT_IS_PARALLEL = false;

constexpr int ERROR_ARGUMENTS = 1;
constexpr int ERROR_EXECUTION = 2;
constexpr int ERROR_UNKNOWN = 3;

// small helper struct - print time elapsed
// a timer is started on creation & restarted on every lap() call
struct Timer {
  using clock = std::chrono::high_resolution_clock;
  clock::time_point start = clock::now();

  // restart timer, print elapsed time with given comment
  void lap(const std::string_view &comment) {
    auto now = clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    std::print("[{}] {}\n", elapsed, comment);
    start = now;
  }
};

// how to use this program?
void print_usage() {
  std::print(
      "\nUsage:\n./upp_sp1 [path to stations file] [path to measurements "
      "file] [--serial | --parallel]\n\nThe defaults are:\n./upp_sp1 "
      "{} {} --{}\n\nEither enter zero or all parameters.",
      DEFAULT_STATIONS_PATH, DEFAULT_MEASUREMENTS_PATH,
      DEFAULT_IS_PARALLEL ? "parallel" : "serial");
}

// load all arguments from CLI into given memory
// may THROW
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

// the program's first version, serial in execution
// works as is specified in the assignment
void serial_version(const std::string_view &stations_path,
                    const std::string_view &measurements_path) {
  Timer timer;

  // A) load data
  std::vector<chmu::Station> stations =
      chmu::load::serial::work(stations_path, measurements_path);
  timer.lap("Data loaded.");

  // B) pre-process data (=filtration) [1]
  chmu::filter::serial::work(stations);
  timer.lap("Data filtered.");

  // C) work on big data [3]
  chmu::collect_stats__serial(stations);
  timer.lap("Monthly averages computed.");

  // D) identify fluctuation [2]
  chmu::identify_fluctuation__serial(stations);
  timer.lap("Fluctuations identified.");

  // E) draw a map for each month [4]
  chmu::draw_svg__serial(stations);
  timer.lap("Draw SVG maps.");

  // F) create a CSV output file [5]
  chmu::write_csv__serial(stations);
  timer.lap("CSV with fluctuations written.");
}

void parallel_version(const std::string_view &stations_path,
                      const std::string_view &measurements_path) {
  parallel::Thread_Pool thread_pool{};
  Timer timer;

  // A) load data *the same*
  std::vector<chmu::Station> stations =
      chmu::load::parallel::work(thread_pool, stations_path, measurements_path);
  timer.lap("Data loaded.");

  // B) pre-process data (=filtration) [1]
  chmu::filter::parallel::work(stations);
  timer.lap("Data filtered.");

  // C) work on big data [3]
  chmu::collect_stats__serial(stations);
  timer.lap("Monthly averages computed.");

  // D) identify fluctuation [2]
  chmu::identify_fluctuation__serial(stations);
  timer.lap("Fluctuations identified.");

  // E) draw a map for each month [4]
  chmu::draw_svg__serial(stations);
  timer.lap("Draw SVG maps.");

  // F) create a CSV output file [5]
  chmu::write_csv__serial(stations);
  timer.lap("CSV with fluctuations written.");
}

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
