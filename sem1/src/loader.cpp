
#include "loader.hpp"
#include "model/measurement.hpp"
#include "model/model.hpp"
#include <cstddef>
#include <fstream>
#include <ios>
#include <iterator>
#include <latch>
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

// read the whole file at <path> to single string <buffer>
void read_into_string(const std::filesystem::path &path, std::string &buffer) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file) {
    throw std::runtime_error("Invalid stations path.");
  }

  // get size and allocate
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  buffer.resize(size);

  // Read the entire file in one go
  if (!file.read(buffer.data(), size)) {
    throw std::runtime_error("Failed to read stations file.");
  }
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

  std::string buffer;
  read_into_string(stations_path, buffer);

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

    std::string_view sv_id, sv_name, sv_lat, sv_lon;
    get_until(line, sv_id, ';');
    get_until(line, sv_name, ';');
    get_until(line, sv_lat, ';');
    // on linux not found (OK), on windows found and therefore ignored (OK)
    get_until(line, sv_lon, '\r');

    stations.emplace_back(sv_id, sv_name, sv_lat, sv_lon);
  }

  return stations;
}

void only_measurements(const std::filesystem::path &measurements_path,
                       std::vector<Station> &stations) {
  std::string buffer;
  read_into_string(measurements_path, buffer);

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
    std::string_view sv_id, sv_ordinal, sv_year, sv_month, sv_day, sv_value;
    get_until(line, sv_id, ';');
    get_until(line, sv_ordinal, ';');
    get_until(line, sv_year, ';');
    get_until(line, sv_month, ';');
    get_until(line, sv_day, ';');
    get_until(line, sv_value, '\r');

    size_t id = 0;
    std::from_chars(sv_id.data(), sv_id.data() + sv_id.size(), id);

    if (id == 0 || id > stations.size()) {
      throw std::runtime_error("There is a measurement which does not belong "
                               "to any existing stations with id=" +
                               std::to_string(id));
    }

    auto &station = stations[id - 1];
    station.measurements().emplace_back(sv_year, sv_month, sv_day, sv_value);
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
  std::string buffer;
  read_into_string(stations_path, buffer);

  size_t n = thread_pool.n_threads();

  auto chunks = _detail::split_buffer(buffer, n);
  std::vector<std::vector<Station>> stations_pool(n);

  std::latch latch(n);

  for (size_t i = 0; i < n; ++i) {
    thread_pool.enqueue([&, i]() {
      auto &chunk = chunks[i];
      auto &out = stations_pool[i];
      std::vector<Station> local_stations;

      std::string_view sv_chunk(chunk);
      std::string_view line;

      if (i == 0) { // skip header
        get_until(sv_chunk, line, '\n');
      }

      while (get_until(sv_chunk, line, '\n')) {
        if (line.empty()) {
          continue;
        }
        std::string_view sv_id, sv_name, sv_lat, sv_lon;
        get_until(line, sv_id, ';');
        get_until(line, sv_name, ';');
        get_until(line, sv_lat, ';');
        get_until(line, sv_lon, '\r');

        local_stations.emplace_back(sv_id, sv_name, sv_lat, sv_lon);
      }

      out = std::move(local_stations);

      latch.count_down();
    });
  }

  latch.wait();

  std::vector<Station> result;
  for (auto &vec : stations_pool) {
    std::ranges::move(vec, std::back_inserter(result));
  }

  return result;
}

void only_measurements(::parallel::Thread_Pool &thread_pool,
                       const std::filesystem::path &measurements_path,
                       std::vector<Station> &stations) {
  std::string buffer;
  read_into_string(measurements_path, buffer);

  size_t n = thread_pool.n_threads();
  auto chunks = _detail::split_buffer(buffer, n);

  // each thread has its map of found measurements (map is for station ID)
  std::vector<std::vector<std::vector<Measurement>>> measurements_pool(n);

  std::latch latch(n);

  for (size_t i = 0; i < n; ++i) {
    thread_pool.enqueue([&, i]() {
      auto &chunk = chunks[i];
      auto &out = measurements_pool[i];
      std::vector<std::vector<Measurement>> local_mesurements(stations.size());

      std::string_view sv_chunk(chunk);
      std::string_view line;

      if (i == 0) { // skip header
        get_until(sv_chunk, line, '\n');
      }

      while (get_until(sv_chunk, line, '\n')) {
        if (line.empty()) {
          continue;
        }

        std::string_view sv_id, sv_ordinal, sv_year, sv_month, sv_day, sv_value;
        get_until(line, sv_id, ';');
        get_until(line, sv_ordinal, ';');
        get_until(line, sv_year, ';');
        get_until(line, sv_month, ';');
        get_until(line, sv_day, ';');
        get_until(line, sv_value, '\r');

        size_t id = 0;
        std::from_chars(sv_id.data(), sv_id.data() + sv_id.size(), id);

        if (id == 0 || id > stations.size()) {
          throw std::runtime_error(
              "There is a measurement which does not belong "
              "to any existing stations with id=" +
              std::to_string(id));
        }

        local_mesurements[id - 1].emplace_back(sv_year, sv_month, sv_day,
                                               sv_value);
      }

      out = std::move(local_mesurements);
      latch.count_down();
    });
  }

  latch.wait();

  // MERGE all work - ordered (1st thread worked on 1st chunk of file etc)
  for (auto &thread_vec : measurements_pool) {
    for (size_t station_idx = 0; station_idx < stations.size(); ++station_idx) {

      auto &src = thread_vec[station_idx];
      if (src.empty()) {
        continue;
      }

      auto &target = stations[station_idx].measurements();
      target.reserve(target.size() + src.size());

      std::ranges::move(src, std::back_inserter(target));
    }
  }
}

namespace _detail {

std::vector<std::string_view> split_buffer(const std::string &buffer,
                                           size_t n_threads) {
  // rought cut - circa
  size_t total_size = buffer.size();
  size_t chunk_size = total_size / n_threads;
  std::vector<std::string_view> chunks;

  size_t last_pos = 0;
  for (int i = 0; i < n_threads; ++i) {
    size_t search_pos = (i + 1) * chunk_size;

    // don't cut line in half
    size_t end_pos = buffer.find('\n', search_pos);

    // take the rest if a) last line, b) last worker
    if (end_pos == std::string::npos || i == n_threads - 1) {
      end_pos = total_size;
    } else {
      end_pos++; // include \n
    }

    chunks.push_back(
        std::string_view(buffer.data() + last_pos, end_pos - last_pos));
    last_pos = end_pos;
  }

  return chunks;
}

} // namespace _detail

} // namespace parallel

} // namespace chmu::load
