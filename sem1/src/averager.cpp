#include "averager.hpp"
#include "measurement.hpp"
#include <cstddef>
#include <iterator>
#include <numeric>
#include <ranges>
namespace chmu {

void compute_averages_station__serial(Station &station) {
  auto &measurements = station.measurements_const();
  auto &avgs = station.averages_all();

  auto month_groups =
      measurements | std::views::chunk_by([](const auto &a, const auto &b) {
        return a.civil_date().month_ == b.civil_date().month_;
      });

  for (auto month_view : month_groups) {
    // get month from first element - doesn't matter
    int month = month_view.front().civil_date().month_;

    auto values = month_view | std::views::transform([](const Measurement &m) {
                    return m.value();
                  });

    float sum = std::accumulate(values.begin(), values.end(), 0.0f);
    size_t days = std::ranges::distance(values);

    if (days != 0) {
      avgs[month - 1].push_back(sum / days);
    }
  }

  auto &final_avgs = station.averages_by_month();

  for (size_t i = 0; i < avgs.size(); ++i) {
    const auto &month = avgs[i];
    if (month.empty()) {
      continue;
    }

    float sum = std::accumulate(month.begin(), month.end(), 0.0f);
    float avg = sum / month.size();

    final_avgs[i] = avg;
  }
}

} // namespace chmu
