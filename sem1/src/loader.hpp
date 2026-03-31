#pragma once

#include "model/model.hpp"
#include <filesystem>
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

} // namespace chmu::load
