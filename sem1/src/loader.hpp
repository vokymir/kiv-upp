#pragma once

#include "model/model.hpp"
#include "threadpool.hpp"
#include <cstddef>
#include <filesystem>
#include <string_view>
#include <vector>
namespace chmu::load {

namespace serial {

// load all stations and all measurements into that stations
// do not perform any filtration what so ever
std::vector<Station> work(const std::filesystem::path &stations_path,
                          const std::filesystem::path &measurements_path);

// load only all stations
std::vector<Station> only_stations(const std::filesystem::path &stations_path);

// load only all measurements into existing stations vector
void only_measurements(const std::filesystem::path &measurements_path,
                       std::vector<Station> &stations);
} // namespace serial

namespace parallel {

// load all stations & measurements into the vector
std::vector<Station> work(::parallel::Thread_Pool &thread_pool,
                          const std::filesystem::path &stations_path,
                          const std::filesystem::path &measurements_path);

std::vector<Station> only_stations(::parallel::Thread_Pool &thread_pool,
                                   const std::filesystem::path &stations_path);

void only_measurements(::parallel::Thread_Pool &thread_pool,
                       const std::filesystem::path &measurements_path,
                       std::vector<Station> &stations);

namespace _detail {

// split buffer cca fairly based on threads count, ensure no line is cut in half
// etc.
std::vector<std::string_view> split_buffer(const std::string &buffer,
                                           size_t n_threads);
} // namespace _detail

} // namespace parallel

} // namespace chmu::load
