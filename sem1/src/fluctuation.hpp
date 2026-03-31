#pragma once

#include "model/model.hpp"
namespace chmu {

// find all fluctuations across all stations and months.
void identify_fluctuation__serial(std::vector<Station> &stations);

// Find all fluctuations at a station in all months.
void identify_fluctuation_station__serial(Station &station);

// Find all fluctuations for given month at a station.
// month is 1-indexed, ie. 1 = January
void identify_fluctuation_station_month__serial(Station &station, int month);

} // namespace chmu
