#pragma once

#include "date.hpp"
#include <charconv>
#include <string_view>
namespace chmu {

class Measurement {
  // all are const by design
private:
  // duplicate dates - civil useful for averages, date useful for filtering
  // because they are duplicates, marked const to be immutable
  const Date civil_date_;
  const size_t date_;

  const float value_;

public:
  Measurement(const std::string_view &year_sv, const std::string_view &month_sv,
              const std::string_view &day_sv, const std::string_view &value_sv)
      : civil_date_(year_sv, month_sv, day_sv),
        date_(Date::from_date(civil_date_)), value_([&value_sv]() {
          float v = 0;
          std::from_chars(value_sv.data(), value_sv.data() + value_sv.size(),
                          v);
          return v;
        }()) {}

  // getters
  size_t date() const { return date_; }
  float value() const { return value_; }
  const Date &civil_date() const { return civil_date_; }
};

} // namespace chmu
