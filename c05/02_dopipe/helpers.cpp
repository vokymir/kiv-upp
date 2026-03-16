#include "config.h"
#include "matrix.h"

#include <cmath>
#undef min

// naivni zpusob, jak najit nejmensi spolecny nasobek
// pozn. kod je umyslne trochu zpraseny, abychom nafoukli casy zpracovani
double lcm(double a, double b) {
  size_t ia = static_cast<size_t>(a);
  size_t ib = static_cast<size_t>(b);

  if (ia == 0 || ib == 0)
    return 0;

  size_t lcm;
  for (lcm = std::min(ia, ib); lcm <= ia * ib; lcm++) {
    if ((lcm % ia) == 0 && (lcm % ib) == 0)
      return static_cast<double>(lcm);
  }

  return static_cast<double>(ia * ib);
}

// naivni zpusob, jak najit nejvetsi spolecny delitel
// pozn. kod je umyslne trochu zpraseny, abychom nafoukli casy zpracovani
double gcd(double a, double b) {
  size_t ia = static_cast<size_t>(a);
  size_t ib = static_cast<size_t>(b);

  if (ia == 0 || ib == 0)
    return 0;

  size_t gcd;
  for (gcd = std::min(ia, ib); gcd > 0; gcd--) {
    if ((ia % gcd) == 0 && (ib % gcd) == 0)
      return static_cast<double>(gcd);
  }

  return static_cast<double>(1);
}
