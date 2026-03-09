#pragma once

#include "measurement.hpp"
#include <array>
#include <charconv>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
namespace chmu {

// all fluctuation are linked to some station.
// stores month, year and temperature difference as specified in assignment
struct Fluctuation {
  int month;
  int year;
  float temp_diff;
};

// storage for one monthly average value - store the value and year
struct Monthly_Average {
  float average;
  int year;
};

// storage for all averages in one month across multiple years
struct Monthly_Averages {
  std::vector<Monthly_Average> averages;

  // add one average record
  void add(float avg, int year) { averages.push_back({avg, year}); }
};

class Station {
private:
  // information about the station itself
  size_t id_;
  std::string name_;
  float lat_;
  float lon_;

  // here are stored all measurements which are loaded from the file
  std::vector<Measurement> measurements_;

  // here are stored all monthly averages for all months across all years in
  // measurements - these are computed after the original file was closed
  std::array<Monthly_Averages, 12> averages_;

  // one average value for every month - this is a mere storage, calculated from
  // averages_
  std::array<float, 12> averages_by_month_;

  // storage for all fluctuations
  std::vector<Fluctuation> fluctuations_;

public:
  Station(const std::string_view &id, const std::string_view &name,
          const std::string_view &lat, const std::string_view &lon)
      : id_(0), name_(name), lat_(0.0f), lon_(0.0f) {

    std::from_chars(id.data(), id.data() + id.size(), id_);
    std::from_chars(lat.data(), lat.data() + lat.size(), lat_);
    std::from_chars(lon.data(), lon.data() + lon.size(), lon_);
  }

  // get/set
  size_t id() const { return id_; }
  float lat() const { return lat_; }
  float lon() const { return lon_; }
  std::vector<Measurement> &measurements() { return measurements_; }
  const std::vector<Measurement> &measurements_const() const {
    return measurements_;
  }

  std::vector<Fluctuation> &fluctuations() { return fluctuations_; }
  const std::vector<Fluctuation> &fluctuations_const() const {
    return fluctuations_;
  }

  auto &averages() { return averages_; }
  std::array<float, 12> &averages_by_month() { return averages_by_month_; }
  const std::array<float, 12> &averages_by_month_const() const {
    return averages_by_month_;
  }
};

} // namespace chmu
