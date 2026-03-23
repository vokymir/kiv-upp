#include <chrono>
#include <functional>
#include <iostream>
#include <random>

#include "reduce.h"

// kolik cisel se ma vygenerovat?
constexpr size_t NumCount = 100'000'000;

// kolik mereni se ma provest? aby bylo mereni fer
constexpr size_t NumMeasurements = 5;

// funkce pro mereni
void Do_Measure(const std::string &name, const std::vector<double> &numbers,
                double (*fnc)(const std::vector<double> &)) {
  std::chrono::steady_clock::time_point start, end;
  unsigned long long ms = 0;
  double sum = 0;

  for (size_t i = 0; i < NumMeasurements; i++) {
    start = std::chrono::steady_clock::now();

    // spustime zkoumanou funkci
    volatile double local_sum = fnc(numbers);

    end = std::chrono::steady_clock::now();

    // v prvnim prubehu ulozime vysledek jako referencni
    if (i == 0)
      sum = local_sum;
    else // ostatni prubehy porovname s referencnim
    {
      // zde vznikne zajimava situace - v jedne z verzi bude program hlasit, ze
      // je vysledek nejspise spatny proc tomu tak je?

      if (sum != local_sum)
        std::cerr << "Algoritmus nejspise dava spatny vysledek! " << sum
                  << " != " << local_sum << std::endl;
    }

    ms += std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
              .count();
  }
  // prumer casu
  ms /= NumMeasurements;

  std::cout << name << ": suma = " << sum << ", prumerny cas = " << ms << "ms"
            << std::endl;
}

int main(int argc, char **argv) {

  std::random_device rdev;
  std::default_random_engine reng(rdev());
  std::uniform_real_distribution<double> rdist(0, 1.0);

  std::vector<double> numbers(NumCount);
  for (size_t i = 0; i < NumCount; i++)
    numbers[i] = rdist(reng);

  Do_Measure("Seriova verze", numbers, sum_serial);
  Do_Measure("Paralelni verze (redukcni promenna)", numbers,
             sum_parallel_atomic);
  Do_Measure("Paralelni verze (nezavisla promenna)", numbers,
             sum_parallel_independent);

  return 0;
}
