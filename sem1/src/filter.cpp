
#include "filter.hpp"
#include "model/model.hpp"
#include <chrono>
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

  auto first = mes.front().date().day_point();
  auto last = mes.back().date().day_point();

  auto total_days =
      std::chrono::duration_cast<std::chrono::days>(last - first).count();

  if (total_days <= 0) {
    return false;
  }

  float total_years = total_days / 365.0f; // cca
  float yearly_avg = mes.size() / total_years;

  return yearly_avg >= NEEDED_YEAR_AVERAGE_VALUES;
}

// return TRUE if there is at least 5 years worth of continuous measurements
bool have_enough_continuous_data(const std::vector<Measurement> &mes) {
  if (mes.empty()) {
    return false;
  }

  auto first = mes.front().date().day_point();
  auto last = mes.back().date().day_point();

  auto span_days =
      std::chrono::duration_cast<std::chrono::days>(last - first).count();

  return span_days >= NEEDED_CONTINUOUS_DAYS;
}

bool should_keep_station(const Station &st) {
  auto mes = st.measurements_const();

  return have_enough_values_on_average(mes) && have_enough_continuous_data(mes);
}

void filter__serial(std::vector<Station> &stations) {

  std::erase_if(stations, [](Station &st) { return !should_keep_station(st); });
}

} // namespace chmu
