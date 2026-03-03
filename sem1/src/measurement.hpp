#pragma once

#include <charconv>
#include <string_view>
namespace chmu {

class Measurement {
private:
  int year_;
  int month_;
  int day_;
  int value_;

public:
  Measurement(const std::string_view &year_sv, const std::string_view &month_sv,
              const std::string_view &day_sv,
              const std::string_view &value_sv) {

    std::from_chars(year_sv.data(), year_sv.data() + year_sv.size(), year_);
    std::from_chars(month_sv.data(), month_sv.data() + month_sv.size(), month_);
    std::from_chars(day_sv.data(), day_sv.data() + day_sv.size(), day_);
    std::from_chars(value_sv.data(), value_sv.data() + value_sv.size(), value_);
  }
};

} // namespace chmu
