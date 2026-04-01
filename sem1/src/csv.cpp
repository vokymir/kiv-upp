
#include "csv.hpp"
#include "model/station.hpp"
#include "threadpool.hpp"
#include <cstddef>
#include <fstream>
#include <latch>
#include <stdexcept>
#include <string>
#include <vector>
namespace chmu::csv {

namespace serial {

void work(const std::vector<Station> &stations) {
  std::ofstream file(DEFAULT_EXPORT_CSV_PATH);
  if (!file) {
    throw std::runtime_error("Cannot open CSV file.");
  }

  file << "station_id;month;year;diff\n";

  for (const auto &st : stations) {
    for (const auto &fluct : st.fluctuations_const()) {
      file << st.id() << ";" << fluct.month << ";" << fluct.year << ";"
           << fluct.temp_diff << "\n";
    }
  }
}

} // namespace serial

namespace parallel {

void work(::parallel::Thread_Pool &thread_pool,
          const std::vector<Station> &stations) {
  std::ofstream file(DEFAULT_EXPORT_CSV_PATH);
  if (!file) {
    throw std::runtime_error("Cannot open CSV file.");
  }

  file << "station_id;month;year;diff\n";

  size_t n = thread_pool.n_threads();
  std::vector<std::string> buffers_pool(n);

  std::latch latch(n);

  size_t batch_size = (stations.size() + n - 1) / n;

  for (size_t i = 0; i < n; ++i) {
    size_t begin = i * batch_size;
    size_t end = std::min(begin + batch_size, stations.size());

    thread_pool.enqueue([&, i, begin, end]() {
      auto &buffer = buffers_pool[i];

      for (size_t j = begin; j < end; ++j) {
        const auto &st = stations[j];

        for (const auto &fluct : st.fluctuations_const()) {

          buffer.append(std::to_string(st.id()));
          buffer.push_back(';');

          buffer.append(std::to_string(fluct.month));
          buffer.push_back(';');

          buffer.append(std::to_string(fluct.year));
          buffer.push_back(';');

          buffer.append(std::to_string(fluct.temp_diff));
          buffer.push_back('\n');
        }
      }

      buffers_pool[i] = std::move(buffer);
      latch.count_down();
    });
  }

  latch.wait();

  for (const auto &buf : buffers_pool) {
    file.write(buf.data(), buf.size());
  }
}

} // namespace parallel

} // namespace chmu::csv
