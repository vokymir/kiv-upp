#include "averager.hpp"
#include <numeric>
namespace chmu {

void compute_averages_station__serial(Station &station) {
  auto &measurements = station.measurements_const();
  auto &avgs = station.averages_all();

  float monthly_sum = 0;
  int month_days = 0;
  int curr_month = 1;

  for (const auto &m : measurements) {
    int month = m.civil_date().month_;
    if (month != curr_month) {
      if (month_days != 0) {
        avgs[curr_month - 1].push_back(monthly_sum / month_days);
      }
      monthly_sum = 0;
      month_days = 0;
      curr_month = month;
    }

    monthly_sum += m.value();
    month_days++;
  }
  // last month not handled by for-loop
  if (month_days != 0) {
    avgs[curr_month - 1].push_back(monthly_sum / month_days);
  }

  // not handled monthly averages
}

} // namespace chmu
