#pragma once

#include <cstddef>
namespace chmu {

// base date is 1900-01-01 because it's unreasonable to expect any older date
// for this task (also experimentally found it's actually 1961 but that is OK)
struct Date {
  int year_;
  int month_;
  int day_;

  Date(int year, int month, int day) : year_(year), month_(month), day_(day) {}

  // get number from date - the date is number of days between base date
  // (1.1.1990) and given date
  static size_t from_date(Date date);
  static size_t from_date(int year, int month, int day);

  // get date from number
  static Date to_date(size_t date);
};

} // namespace chmu
