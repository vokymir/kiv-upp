#pragma once

#include "stations.hpp"
#include <string>

constexpr std::string DEFAULT_EXPORT_CSV_PATH = "vykyvy.csv";

namespace chmu {

// write all fluctuations in CSV file in assignment-specified format
void write_csv__serial(const Stations &stations);

} // namespace chmu
