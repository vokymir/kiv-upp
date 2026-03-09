#pragma once

#include "station.hpp"
#include "stations.hpp"
namespace chmu {

// compute all and monthly averages for all stations
void compute_averages__serial(Stations &stations);

// compute all and monthly averages for one station
void compute_averages_station__serial(Station &station);

// find & store all averages grouped by month across all years
// => fill averages_
void compute_averages__serial(Station &station);

// calculate the average for all months across years
// => depends on averages_
void compute_monthly_averages__serial(Station &station);

} // namespace chmu
