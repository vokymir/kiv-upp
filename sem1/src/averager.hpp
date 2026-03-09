#pragma once

#include "station.hpp"
#include "stations.hpp"
namespace chmu {

// ===== LOOK OUT =====
// => produces averages_ inside station(s)

// compute all averages for all stations
void compute_averages__serial(Stations &stations);

// find & store all averages grouped by month across all years
// => fill averages_
void compute_averages_station__serial(Station &station);

// ===== WARN =====
// => depends on averages_

void compute_monthly_averages__serial(Stations &stations);
void compute_monthly_averages_station__serial(Station &station);

} // namespace chmu
