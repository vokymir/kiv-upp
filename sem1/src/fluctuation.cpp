
#include "model/model.hpp"
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <ranges>
#include <vector>
namespace chmu::flucs {

namespace serial {

void station_month(Station &station, int month_id) {
  int month_idx = month_id - 1;
  const auto &series = station.stats().monthly_series_[month_idx].values;
  std::vector<Fluctuation> &flucs = station.fluctuations();

  if (series.empty()) {
    return;
  }

  // === find threshold
  float min_val = station.stats().monthly_min_[month_idx];
  float max_val = station.stats().monthly_max_[month_idx];

  float threshold = (max_val - min_val) * 0.75;

  // === find fluctuations
  // WARN: important assumption: averages are sorted by year
  for (const auto &[prev, curr] : series | std::views::adjacent<2>) {
    auto diff = std::abs(prev.value - curr.value);
    if (diff <= threshold) {
      continue;
    }

    flucs.emplace_back(month_id, curr.year, diff);
  }
}

void station(Station &station) {
  for (size_t month = 1; month <= 12; ++month) {
    station_month(station, month);
  }
}

void work(std::vector<Station> &stations) {
  for (auto &st : stations) {
    station(st);
  }
}

} // namespace serial

} // namespace chmu::flucs
