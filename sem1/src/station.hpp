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

class Station {
private:
  size_t id_;
  std::string name_;
  float lat_;
  float lon_;

  std::vector<Measurement> measurements_;
  std::vector<Fluctuation> fluctuations_;
  std::array<std::vector<float>, 12> averages_all_;
  std::array<float, 12> averages_by_month_;

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
  std::vector<Measurement> &measurements() { return measurements_; }
  const std::vector<Measurement> &measurements_const() const {
    return measurements_;
  }

  std::vector<Fluctuation> &fluctuations() { return fluctuations_; }
  const std::vector<Fluctuation> &fluctuations_const() const {
    return fluctuations_;
  }

  std::array<std::vector<float>, 12> &averages_all() { return averages_all_; }
};

} // namespace chmu
