
#include "filter.hpp"
#include "measurement.hpp"
#include "station.hpp"
#include <cstddef>
#include <vector>
namespace chmu {

// 5 years
// WARN: no leap years included
constexpr size_t NEEDED_CONTINUOUS_DAYS = 5 * 365;
// 100 values/year on average
constexpr size_t NEEDED_YEAR_AVERAGE_VALUES = 100;

// return TRUE if station has enough records to have at least 100 per year on
// average
// leap years are not considered
bool have_enough_values_on_average(const std::vector<Measurement> &mes) {
  // check if have too few records to begin with
  if (mes.size() < NEEDED_YEAR_AVERAGE_VALUES) {
    return false;
  }

  size_t very_first_day = mes.front().date();
  size_t very_last_day = mes.back().date();

  size_t total_days = very_last_day - very_first_day;
  if (total_days == 0) { // to avoid division by zero
    return false;
  }

  // leap years not considered
  float total_years = total_days / 365.0f;
  float yearly_avg = mes.size() / total_years;

  return yearly_avg >= NEEDED_YEAR_AVERAGE_VALUES;
}

// return TRUE if there is at least 5 years worth of continuous measurements
bool have_enough_continuous_data(const std::vector<Measurement> &mes) {
  size_t first_day = 0;
  size_t last_day = 0;

  for (size_t i = 0; i < mes.size(); ++i) {
    size_t curr_day = mes[i].date();

    // on first day it always fire-off by design, this helps to avoid checking
    // mes.size() before accessing its first element
    if (curr_day - 1 != last_day) {
      first_day = curr_day;
    }
    last_day = curr_day;

    // if already hit the target, no need to continue.
    // if last_day - first_day == 0, that actually means there is one day,
    // that's why > instead of >= is used
    if (last_day - first_day > NEEDED_CONTINUOUS_DAYS) {
      return true;
    }
  }

  // no long enough timespan was found
  return false;
}

bool should_keep_station(const Station &st) {
  auto mes = st.measurements_const();

  return have_enough_values_on_average(mes) && have_enough_continuous_data(mes);
}

void filter_serial(Stations &stations) {}

} // namespace chmu
