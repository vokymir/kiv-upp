#pragma once

#include "model/model.hpp"
#include <filesystem>
#include <vector>
namespace chmu {

// load all stations and all measurements into that stations
// do not perform any filtration what so ever
std::vector<Station>
load__serial(const std::filesystem::path &stations_path,
             const std::filesystem::path &measurements_path);

// load only all stations
std::vector<Station>
load_stations__serial(const std::filesystem::path &stations_path);

// load only all measurements into existing stations vector
void load_measurements__serial(const std::filesystem::path &measurements_path,
                               std::vector<Station> &stations);

} // namespace chmu
