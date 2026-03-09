
#include "station.hpp"
#include "stations.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <ranges>
#include <vector>
namespace chmu {

void identify_fluctuation_station_month__serial(Station &station,
                                                int month_id) {
  int month_idx = month_id - 1;
  auto &averages = station.averages()[month_idx].averages; // :)
  std::vector<Fluctuation> &flucs = station.fluctuations();

  if (averages.empty()) {
    return;
  }

  // === find threshold
  auto [min_it, max_it] = std::ranges::minmax_element(
      averages, [](const auto &obj1, const auto &obj2) {
        return obj1.average < obj2.average;
      });
  float min_val = min_it->average;
  float max_val = max_it->average;

  float threshold = (max_val - min_val) * 0.75;

  // === find fluctuations
  for (const auto &[prev, curr] : averages | std::views::adjacent<2>) {
    auto diff = std::abs(prev.average - curr.average);
    if (diff <= threshold) {
      continue;
    }

    flucs.emplace_back(month_id, curr.year, diff);
  }
}

void identify_fluctuation_station__serial(Station &station) {
  for (size_t month = 1; month <= 12; ++month) {
    identify_fluctuation_station_month__serial(station, month);
  }
}

void identify_fluctuation__serial(Stations &stations) {
  for (auto &st : stations.stations) {
    identify_fluctuation_station__serial(st);
  }
}

} // namespace chmu
