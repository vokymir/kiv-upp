#pragma once

#include "station.hpp"
#include "stations.hpp"
namespace chmu {

// compute all and monthly averages for all stations
void compute_averages__serial(Stations &stations);

// compute all and monthly averages for one station
void compute_averages_station__serial(Station &station);

} // namespace chmu
