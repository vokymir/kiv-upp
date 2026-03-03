
#include "station.hpp"
#include <algorithm>
#include <vector>
namespace chmu {

void identify_fluctuation_station_month__serial(Station &station, int month) {
  std::vector<float> &averages = station.averages_all()[month];
  if (averages.empty()) {
    return;
  }

  auto [min_it, max_it] = std::ranges::minmax_element(averages);
  float min_val = *min_it;
  float max_val = *max_it;

  float treshold = (max_val - min_val) * 0.75;
}

} // namespace chmu
