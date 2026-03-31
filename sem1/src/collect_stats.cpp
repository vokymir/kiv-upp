#include "collect_stats.hpp"
#include "model/model.hpp"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <ranges>
namespace chmu {

void collect_stats__serial(std::vector<Station> &stations) {
  for (auto &st : stations) {
    collect_stats_station__serial(st);
  }
}

void collect_stats_station__serial(Station &station) {
  auto &measurements = station.measurements_const();

  // grouping by one particular month
  auto month_year_groups =
      measurements | std::views::chunk_by([](const auto &a, const auto &b) {
        return a.date().year_ == b.date().year_ &&
               a.date().month_ == b.date().month_;
      });

  // store month average for each month
  for (auto month_view : month_year_groups) {
    collect_stats_station_month__serial(station, month_view);
  }

  collect_stats_station_minmax__serial(station.stats());

  // final - calculate mean
  collect_stats_station_means__serial(station.stats());
}

void collect_stats_station_month__serial(Station &station, auto month_view) {
  // get month from first element - doesn't matter from which
  auto date = month_view.front().date();
  int month = date.month_;
  int year = date.year_;

  size_t days = std::distance(month_view.begin(), month_view.end());
  if (days == 0) {
    return;
  }

  float avg = std::accumulate(month_view.begin(), month_view.end(), 0.0f,
                              [](float acc, const Measurement &m) {
                                return acc + m.value();
                              }) /
              days;

  station.stats().monthly_series_[month - 1].add(avg, year);
}

void collect_stats_station_minmax__serial(Stats &stats) {
  for (size_t i = 0; i < 12; ++i) {
    for (const auto ma : stats.monthly_series_[i].values) {
      stats.monthly_min_[i] = std::min(stats.monthly_min_[i], ma.value);
      stats.monthly_max_[i] = std::max(stats.monthly_max_[i], ma.value);
    }

    stats.global_min_ = std::min(stats.global_min_, stats.monthly_min_[i]);
    stats.global_max_ = std::max(stats.global_max_, stats.monthly_max_[i]);
  }
}

void collect_stats_station_means__serial(Stats &stats) {
  for (size_t i = 0; i < 12; ++i) {
    const auto &series = stats.monthly_series_[i].values;

    if (series.size() == 0) {
      continue;
    }

    float sum = std::accumulate(
        series.begin(), series.end(), 0.0f,
        [](float acc, const Monthly_Average &ma) { return acc + ma.value; });

    float avg = sum / series.size();

    stats.monthly_mean_[i] = avg;
  }
}

} // namespace chmu
