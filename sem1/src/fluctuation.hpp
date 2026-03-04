#pragma once

#include "station.hpp"
#include "stations.hpp"
namespace chmu {

// Find add fluctuations across all stations and months.
void identify_fluctuation__serial(Stations *stations);

// Find all fluctuations at a station in all months.
void identify_fluctuation_station__serial(Station &station);

// Find all fluctuations for given month at a station.
// month is 1-indexed, ie. 1 = January
void identify_fluctuation_station_month__serial(Station &station, int month);

} // namespace chmu
