#pragma once

#include "model/model.hpp"
#include <vector>
namespace chmu::flucs {

namespace serial {

// find all fluctuations across all stations and months.
void work(std::vector<Station> &stations);

// Find all fluctuations at a station in all months.
void station(Station &station);

// Find all fluctuations for given month at a station.
// month is 1-indexed, ie. 1 = January
void station_month(Station &station, int month);

} // namespace serial

namespace parallel {

// do the same as serial version
// uses embarrassing parallelism
void work(std::vector<Station> &stations);

} // namespace parallel

} // namespace chmu::flucs
