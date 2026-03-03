#pragma once

#include <charconv>
#include <cstddef>
#include <string_view>
namespace chmu {

// base date is 1900-01-01 because it's unreasonable to expect any older date
// for this task (also experimentally found it's actually 1961 but that is OK)
struct Date {
  int year_ = 0;
  int month_ = 12;
  int day_ = 25;

  // the default date is the birth of Christ
  Date() {}
  // set date on creation
  Date(int year, int month, int day) : year_(year), month_(month), day_(day) {}
  // set date from string_views - undefined behaviour on invalid string_views
  Date(const std::string_view &year_sv, const std::string_view &month_sv,
       const std::string_view &day_sv) {
    int y = -1, m = -1, d = -1;

    std::from_chars(year_sv.data(), year_sv.data() + year_sv.size(), y);
    std::from_chars(month_sv.data(), month_sv.data() + month_sv.size(), m);
    std::from_chars(day_sv.data(), day_sv.data() + day_sv.size(), d);

    year_ = y;
    month_ = m;
    day_ = d;
  }

  // get number from date - the date is number of days between base date
  // (1.1.1990) and given date
  static size_t from_date(Date date);
  static size_t from_date(int year, int month, int day);

  // get date from number
  static Date to_date(size_t date);
};

} // namespace chmu
