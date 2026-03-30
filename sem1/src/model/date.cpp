#include "date.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
namespace chmu {

/* There are used algorithm from:
 * https://howardhinnant.github.io/date_algorithms.html
 *
 * To implement custom date converter. Some useful comments might be omitted in
 * this file due to being easy to read the linked page.
 * */

// https://howardhinnant.github.io/date_algorithms.html#days_from_civil
constexpr int64_t days_from_civil(int64_t year, int64_t month, int64_t day) {
  year -= month <= 2;
  const int64_t era = (year >= 0 ? year : year - 399) / 400;
  const int64_t yoe = year - era * 400;
  const int64_t doy = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
  const int64_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097 + doe - 719468;
}

// https://howardhinnant.github.io/date_algorithms.html#civil_from_days
constexpr Date civil_from_days(int64_t dayz) {
  dayz += 719468;
  const int64_t era = (dayz >= 0 ? dayz : dayz - 146096) / 146097;
  const int64_t doe = dayz - era * 146097;
  const int64_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  int64_t y = yoe + era * 400;
  const int64_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  const int64_t mp = (5 * doy + 2) / 153;
  const int64_t d = doy - (153 * mp + 2) / 5 + 1;
  const int64_t m = mp + (mp < 10 ? 3 : -9);
  y += (m <= 2);
  return Date{static_cast<int>(y), static_cast<int>(m), static_cast<int>(d)};
}

// theoretically changeable if neccessary
constexpr int BASE_YEAR = 1900;
constexpr int BASE_MONTH = 1;
constexpr int BASE_DAY = 1;
constexpr int64_t BASE_DAYS = days_from_civil(BASE_YEAR, BASE_MONTH, BASE_DAY);

size_t Date::from_date(Date date) {
  return from_date(date.year_, date.month_, date.day_);
}

size_t Date::from_date(int year, int month, int day) {
  int64_t date_days = days_from_civil(year, month, day);

  if (date_days < BASE_DAYS) {
    throw std::runtime_error("Date before base date.");
  }

  return static_cast<size_t>(date_days - BASE_DAYS);
}

Date Date::to_date(size_t date) {
  return civil_from_days(BASE_DAYS + static_cast<int64_t>(date));
}

} // namespace chmu
