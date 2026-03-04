
#include "station.hpp"
#include "stations.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <vector>
namespace chmu {

void identify_fluctuation_station_month__serial(Station &station,
                                                int month_id) {
  int month_idx = month_id - 1;
  std::vector<float> &averages = station.averages_all()[month_idx];
  std::vector<int> &years = station.averages_all_years()[month_idx];
  std::vector<Fluctuation> &flucs = station.fluctuations();
  if (averages.empty()) {
    return;
  }
  assert(averages.size() == years.size() &&
         "Those are monthly averages so there must be corresponding year for "
         "them.");

  // === find threshold
  auto [min_it, max_it] = std::ranges::minmax_element(averages);
  float min_val = *min_it;
  float max_val = *max_it;

  float threshold = (max_val - min_val) * 0.75;

  // === find fluctuations
  for (size_t i = 1; i < averages.size(); ++i) {
    float prev_avg = averages[i - 1];
    float curr_avg = averages[i];

    auto diff = std::abs(prev_avg - curr_avg);
    if (diff <= threshold) {
      continue;
    }

    flucs.emplace_back(month_id, years[i], diff);
  }
}

void identify_fluctuation_station__serial(Station &station) {
  for (size_t month = 1; month <= 12; ++month) {
    identify_fluctuation_station_month__serial(station, month);
  }
}

void identify_fluctuation__serial(Stations *stations) {
  for (auto &st : stations->stations) {
    identify_fluctuation_station__serial(st);
  }
}

} // namespace chmu
