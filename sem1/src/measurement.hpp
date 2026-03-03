#pragma once

#include "convert.hpp"
#include <charconv>
#include <cstddef>
#include <string_view>
namespace chmu {

class Measurement {
private:
  size_t date_;
  float value_;

public:
  Measurement(const std::string_view &year_sv, const std::string_view &month_sv,
              const std::string_view &day_sv,
              const std::string_view &value_sv) {

    int year = -1;
    int month = -1;
    int day = -1;
    std::from_chars(year_sv.data(), year_sv.data() + year_sv.size(), year);
    std::from_chars(month_sv.data(), month_sv.data() + month_sv.size(), month);
    std::from_chars(day_sv.data(), day_sv.data() + day_sv.size(), day);

    date_ = Date::from_date(year, month, day);
    std::from_chars(value_sv.data(), value_sv.data() + value_sv.size(), value_);
  }

  // get/set
  size_t date() const { return date_; }
  int value() const { return value_; }
};

} // namespace chmu
