#pragma once

#include "station.hpp"
#include "stations.hpp"
namespace chmu {

void identify_fluctuation__serial(Stations *stations);

void identify_fluctuation_station__serial(Station &station);

void identify_fluctuation_station_month__serial(Station &station, int month);

} // namespace chmu
