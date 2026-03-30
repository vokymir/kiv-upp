#include "averager.hpp"
#include "model/model.hpp"
#include <cstddef>
#include <iterator>
#include <numeric>
#include <ranges>
namespace chmu {

void compute_averages__serial(Stations &stations) {
  for (auto &st : stations.stations) {
    compute_averages_station__serial(st);
  }
}

void compute_averages_station__serial(Station &station) {
  auto &measurements = station.measurements_const();
  auto &avgs = station.averages();

  // group measurements by month
  auto month_groups =
      measurements | std::views::chunk_by([](const auto &a, const auto &b) {
        return a.civil_date().month_ == b.civil_date().month_;
      });

  for (auto month_view : month_groups) {
    // get month from first element - doesn't matter
    auto date = month_view.front().civil_date();
    int month = date.month_;
    int year = date.year_;

    // get values out of month across all years
    auto values = month_view | std::views::transform([](const Measurement &m) {
                    return m.value();
                  });

    float sum = std::accumulate(values.begin(), values.end(), 0.0f);
    size_t days = std::ranges::distance(values);

    if (days != 0) {
      float avg = sum / days;

      avgs[month - 1].add(avg, year);
    }
  }
}

void compute_monthly_averages__serial(Stations &stations) {
  for (auto &st : stations.stations) {
    compute_monthly_averages_station__serial(st);
  }
}

void compute_monthly_averages_station__serial(Station &station) {
  auto &avgs = station.averages();

  auto &final_avgs = station.averages_by_month();

  for (size_t i = 0; i < avgs.size(); ++i) {
    const auto &month = avgs[i].averages;
    if (month.empty()) {
      continue;
    }

    float sum = std::accumulate(month.begin(), month.end(), 0.0f,
                                [](const auto &sum, const auto &record) {
                                  return sum + record.average;
                                });
    float avg = sum / month.size();

    final_avgs[i] = avg;
  }
}

} // namespace chmu
