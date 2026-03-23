#include "reduce.h"

double sum_serial(const std::vector<double> &numbers) {

  // asi netreba vice komentovat - proste suma cisel ve vektoru

  double accumulator = 0;

  for (size_t i = 0; i < numbers.size(); i++)
    accumulator += numbers[i];

  return accumulator;
}
