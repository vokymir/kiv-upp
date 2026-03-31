
#include "loader.hpp"
#include "model/model.hpp"
#include <cstddef>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
namespace chmu::load {

// get SV until delim is met. return TRUE on success, FALSE on fail.
// modify the <in> string, remove found part -> store that part in <out>
bool get_until(std::string_view &in, std::string_view &out, char delim) {
  size_t pos = in.find(delim);

  if (pos == std::string_view::npos) {
    if (in.empty()) {
      return false;
    }

    // this prevents last-line cut-off
    out = in; // take the rest
    in = {};  // all read already
    return true;
  }

  out = in.substr(0, pos);
  in.remove_prefix(pos + 1);
  return true;
}

namespace serial {

std::vector<Station> work(const std::filesystem::path &stations_path,
                          const std::filesystem::path &measurements_path) {
  std::vector<Station> stations = only_stations(stations_path);

  only_measurements(measurements_path, stations);

  return stations;
}

std::vector<Station> only_stations(const std::filesystem::path &stations_path) {
  std::vector<Station> stations{};
  // it will probably be more
  stations.reserve(1024);

  std::ifstream file(stations_path, std::ios::binary | std::ios::ate);
  if (!file) {
    throw std::runtime_error("Invalid stations path.");
  }

  // get size and allocate
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::string buffer;
  buffer.resize(size);

  // Read the entire file in one go
  if (!file.read(buffer.data(), size)) {
    throw std::runtime_error("Failed to read stations file.");
  }
  file.close();

  std::string_view sv_buffer(buffer);
  std::string_view line;

  // skip header
  if (!get_until(sv_buffer, line, '\n')) {
    return stations;
  }

  while (get_until(sv_buffer, line, '\n')) {
    if (line.empty()) {
      continue;
    }

    std::string_view id_sv;
    get_until(line, id_sv, ';');
    std::string_view name_sv;
    get_until(line, name_sv, ';');
    std::string_view lat_sv;
    get_until(line, lat_sv, ';');
    std::string_view lon_sv;
    // on linux not found (OK), on windows found and therefore ignored (OK)
    get_until(line, lon_sv, '\r');

    stations.emplace_back(id_sv, name_sv, lat_sv, lon_sv);
  }

  return stations;
}

void only_measurements(const std::filesystem::path &measurements_path,
                       std::vector<Station> &stations) {
  std::ifstream file(measurements_path, std::ios::binary | std::ios::ate);
  if (!file) {
    throw std::runtime_error("Invalid measurements path.");
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::string buffer;
  buffer.resize(size);
  if (!file.read(buffer.data(), size)) {
    throw std::runtime_error("Failed to read measurements file.");
  }
  file.close();

  std::string_view sv_buffer(buffer);
  std::string_view line;

  // skip header
  if (!get_until(sv_buffer, line, '\n')) {
    return;
  }

  while (get_until(sv_buffer, line, '\n')) {
    if (line.empty()) {
      continue;
    }

    /*
      station_id;ordinal;year;month;day;value
      1;0;2013;6;1;10.6
      1;1;2013;6;2;14.1
    */
    std::string_view id_sv;
    get_until(line, id_sv, ';');
    std::string_view ordinal_sv;
    get_until(line, ordinal_sv, ';');
    std::string_view year_sv;
    get_until(line, year_sv, ';');
    std::string_view month_sv;
    get_until(line, month_sv, ';');
    std::string_view day_sv;
    get_until(line, day_sv, ';');
    std::string_view value_sv;
    // the same note as in only_stations
    get_until(line, value_sv, '\r');

    size_t id = 0;
    std::from_chars(id_sv.data(), id_sv.data() + id_sv.size(), id);

    if (id == 0 || id > stations.size()) {
      throw std::runtime_error("There is a measurement which does not belong "
                               "to any existing stations.");
    }

    auto &station = stations[id - 1];
    station.measurements().emplace_back(year_sv, month_sv, day_sv, value_sv);
  }
}

} // namespace serial

namespace parallel {

std::vector<Station> work(::parallel::Thread_Pool &thread_pool,
                          const std::filesystem::path &stations_path,
                          const std::filesystem::path &measurements_path) {

  std::vector<Station> stations = only_stations(thread_pool, stations_path);

  only_measurements(thread_pool, measurements_path, stations);

  return stations;
}

std::vector<Station> only_stations(::parallel::Thread_Pool &thread_pool,
                                   const std::filesystem::path &stations_path) {

  return {};
}

void only_measurements(::parallel::Thread_Pool &thread_pool,
                       const std::filesystem::path &measurements_path,
                       std::vector<Station> &stations) {}

} // namespace parallel

} // namespace chmu::load
