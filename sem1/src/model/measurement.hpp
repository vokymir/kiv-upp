#pragma once

#include "date.hpp"
#include <charconv>
#include <string_view>
namespace chmu {

class Measurement {
  // all are const by design
private:
  const Date civil_date_;
  const float value_;

public:
  Measurement(const std::string_view &year_sv, const std::string_view &month_sv,
              const std::string_view &day_sv, const std::string_view &value_sv)
      : civil_date_(year_sv, month_sv, day_sv), value_([&value_sv]() {
          float v = 0;
          std::from_chars(value_sv.data(), value_sv.data() + value_sv.size(),
                          v);
          return v;
        }()) {}

  // getters
  float value() const { return value_; }
  const Date &date() const { return civil_date_; }
};

} // namespace chmu
