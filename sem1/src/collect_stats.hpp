#pragma once

#include "model/model.hpp"
#include <vector>
namespace chmu {

// compute avg,min,max,etc for all stations, for all years, for all months -
// complete set
void collect_stats__serial(std::vector<Station> &stations);

// compute stats for one station, all years&months
void collect_stats_station__serial(Station &station);

// compute for one station, one month, one year
// the month_view must contain all measurements from one particular year and
// month
void collect_stats_station_month__serial(Station &station, auto month_view);

// compute all means in one station
void collect_stats_station_means__serial(Station &station);

} // namespace chmu
