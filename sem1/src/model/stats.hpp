#pragma once

#include <array>
#include <limits>
#include <vector>
namespace chmu {

// one monthly average for a specific year
struct Monthly_Average {
  float value;
  int year;
};

// all averages for a given month across years
struct Monthly_Series {
  std::vector<Monthly_Average> values;

  void add(float value, int year) { values.push_back({value, year}); }
};

struct Stats {
  // for each month (0–11), store values across years
  std::array<Monthly_Series, 12> monthly_series_;

  // final aggregated average for each month
  std::array<float, 12> monthly_mean_;

  // min/max for each month (useful for flucs)
  std::array<float, 12> monthly_min_;
  std::array<float, 12> monthly_max_;

  // global values (useful for drawing)
  float global_min_ = std::numeric_limits<float>::infinity();
  float global_max_ = -1 * std::numeric_limits<float>::infinity();

  Stats() {
    monthly_min_.fill(std::numeric_limits<float>::infinity());
    monthly_max_.fill(-1 * std::numeric_limits<float>::infinity());
  }
};

} // namespace chmu
