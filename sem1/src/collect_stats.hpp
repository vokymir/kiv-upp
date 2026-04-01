#pragma once

#include "model/model.hpp"
#include <vector>
namespace chmu::stats {

namespace serial {

// compute avg,min,max,etc for all stations, for all years, for all months -
// complete set
void work(std::vector<Station> &stations);

// compute stats for one station, all years&months
void station(Station &station);

// fill month_series for one station, one month, one year
// the month_view must contain all measurements from one particular year and
// month
void station_month(Station &station, auto month_view);

// calculate montly/global min/max for one station with filled month_series
// NOTE: require station_month to be run before
void station_minmax(Stats &stats);

// compute all means in one station
// NOTE: require station_month to be run before
void station_means(Stats &stats);

} // namespace serial

} // namespace chmu::stats
