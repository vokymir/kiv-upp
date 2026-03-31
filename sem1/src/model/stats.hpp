#pragma once

#include <array>
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
};

} // namespace chmu
