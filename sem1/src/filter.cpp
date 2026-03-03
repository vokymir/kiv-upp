
#include "filter.hpp"
#include "station.hpp"
#include <cstddef>
namespace chmu {

// 5 years
constexpr size_t NEEDED_CONTINUOUS_MONTHS = 5 * 12;
// 100 values/year on average
constexpr size_t NEEDED_YEAR_AVERAGE_VALUES = 100;

bool should_del_station(const Station &st) {
  size_t continuous_months = 0;
  size_t first_month = 0;

  auto mes = st.measurements_const();
  for (size_t i = 0; i < mes.size(); ++i) {
  }

  return true;
}

void filter_serial(Stations &stations) {}

} // namespace chmu
