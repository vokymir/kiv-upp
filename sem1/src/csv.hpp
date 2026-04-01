#pragma once

#include "model/model.hpp"
#include "threadpool.hpp"
#include <string>
#include <vector>

constexpr std::string DEFAULT_EXPORT_CSV_PATH = "vykyvy.csv";

namespace chmu::csv {

namespace serial {

// write all fluctuations in CSV file in assignment-specified format
void work(const std::vector<Station> &stations);

} // namespace serial

namespace parallel {

// prepare the string to write in separate threads, write in one thread
void work(::parallel::Thread_Pool &thread_pool,
          const std::vector<Station> &stations);

} // namespace parallel

} // namespace chmu::csv
