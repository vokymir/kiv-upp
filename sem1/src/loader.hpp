#pragma once

#include "stations.hpp"
#include <filesystem>
#include <memory>
namespace chmu {

// load all stations and all measurements into that stations
// do not perform any filtration what so ever
std::unique_ptr<Stations>
load_serial(const std::filesystem::path &stations_path,
            const std::filesystem::path &measurements_path);

// load only all stations
std::unique_ptr<Stations>
load_stations_serial(const std::filesystem::path &stations_path);

// load only all measurements into existing stations vector
void load_measurements_serial(const std::filesystem::path &measurements_path,
                              Stations &stations);

} // namespace chmu
