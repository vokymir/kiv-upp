#include "collect_stats.hpp"
#include "model/model.hpp"
#include <cstddef>
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

  // calculate everything from that month
  for (auto month_view : month_year_groups) {
    collect_stats_station_month__serial(station, month_view);
  }

  // final - calculate mean
  collect_stats_station_means__serial(station);
}

void collect_stats_station_month__serial(Station &station, auto month_view) {
  // get month from first element - doesn't matter from which
  auto date = month_view.front().date();
  int month = date.month_;
  int year = date.year_;

  // ===
  // find all stats
  float sum = 0;
  float min_val = std::numeric_limits<float>::infinity();
  float max_val = -1 * std::numeric_limits<float>::infinity();
  size_t days = 0;

  for (const Measurement &m : month_view) {
    float value = m.value();
    sum += value;
    min_val = min_val < value ? min_val : value;
    max_val = max_val > value ? max_val : value;
    ++days;
  }

  // if EMPTY STATS
  if (days == 0) {
    return;
  }

  // ===
  // save all stats
  Stats &s = station.stats();
  float avg = sum / days;

  s.monthly_series_[month - 1].add(avg, year);

  s.monthly_min_[month - 1] = std::min(s.monthly_min_[month - 1], min_val);
  s.monthly_max_[month - 1] = std::max(s.monthly_max_[month - 1], max_val);

  s.global_min_ = std::min(s.global_min_, min_val);
  s.global_max_ = std::max(s.global_max_, max_val);
}

void collect_stats_station_means__serial(Station &station) {
  auto &stats = station.stats();

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
