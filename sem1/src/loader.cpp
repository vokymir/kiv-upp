
#include "loader.hpp"
#include "model/model.hpp"
#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
namespace chmu {

// helper function for parsing next field in CSV
// naive and simple
// return new string_view with the found field, remove prefix in sv s
std::string_view next_field_(std::string_view &s, char delim = ';') {
  size_t pos = s.find(delim);
  std::string_view field = s.substr(0, pos);

  // if last field on line, use size. if not, skip delim
  s.remove_prefix(pos == std::string_view::npos ? s.size() : pos + 1);

  return field;
}

std::vector<Station>
load__serial(const std::filesystem::path &stations_path,
             const std::filesystem::path &measurements_path) {
  std::vector<Station> stations = load_stations__serial(stations_path);

  load_measurements__serial(measurements_path, stations);

  return stations;
}

std::vector<Station>
load_stations__serial(const std::filesystem::path &stations_path) {
  std::ifstream file(stations_path);
  if (!file) {
    throw std::runtime_error("Invalid stations path.");
  }

  std::vector<Station> stations{};
  // it will probably be more
  stations.reserve(1024);

  std::string line;
  // skip header
  if (!std::getline(file, line)) {
    return stations;
  }

  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    std::string_view sv(line);

    std::string_view id_sv = next_field_(sv);
    std::string_view name_sv = next_field_(sv);
    std::string_view lat_sv = next_field_(sv);
    std::string_view lon_sv = next_field_(sv);

    stations.emplace_back(id_sv, name_sv, lat_sv, lon_sv);
  }

  return stations;
}

void load_measurements__serial(const std::filesystem::path &measurements_path,
                               std::vector<Station> &stations) {
  std::ifstream file(measurements_path);
  if (!file) {
    throw std::runtime_error("Invalid measurements path.");
  }

  std::string line;
  // skip header
  if (!std::getline(file, line)) {
    return;
  }

  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    std::string_view sv(line);
    /*
      station_id;ordinal;year;month;day;value
      1;0;2013;6;1;10.6
      1;1;2013;6;2;14.1
    */
    std::string_view id_sv = next_field_(sv);
    next_field_(sv); // skip ordinal
    std::string_view year_sv = next_field_(sv);
    std::string_view month_sv = next_field_(sv);
    std::string_view day_sv = next_field_(sv);
    std::string_view value_sv = next_field_(sv);

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

} // namespace chmu
