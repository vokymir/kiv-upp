#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

constexpr size_t NumberCount =
    1'000'000'000; // tuto konstantu si kdyztak zmente dle sve konfigurace PC

int main(int argc, char **argv) {

  double prumer = 0;
  int maximum = std::numeric_limits<int>::min();
  int minimum = std::numeric_limits<int>::max();
  int pocetSudych = 0;
  double standardniOdchylka = 0;

  /*
   * Vas ukol:
   * 1) vygenerujte paralelne pole ruznych celych cisel o velikosti NumberCount
   *    - generovani nemusi byt uplne nahodne, jak se vam bude chtit; podstatou
   * je, aby cisla byla ruzna (klidne generujte cisla od 1 do NumberCount)
   *    - dbejte na spravnou deklaraci typu promenne, apod
   *
   * 2) spocitejte/najdete paralelne:
   *    - aritmeticky prumer
   *    - maximum
   *    - minimum
   *    - pocet sudych cisel
   *    ! Kazdy ukol bude zpracovavany nanejvys jednim vlaknem (tj. nebude jeden
   * cyklus, ktery bude delat vse)
   *        --> takze jedno vlakno bude pocitat prumer, druhe bude hledat
   * maximum, treti minimum, ctvrte pocitat pocet sudych cisel (samozrejme v
   * praxi se tento pristup neamortizuje) ! nejde o datovy paralelismus, ale o
   * task-level paralelismus
   *
   * 3) spocitejte standardni (smerodatnou) odchylku (NOTE: je potreba mit
   * aritmeticky prumer)
   *    - zde uz je zadouci pouzit datovy paralelismus
   *
   * *) dodrzte format vypisu nize
   */

  std::cout << "Generuji cisla..." << std::endl;

  std::vector<int> numbers;
  numbers.resize(NumberCount);

#pragma omp parallel for
  for (int i = 0; i < NumberCount; i++) {
    numbers[i] = i;
  }

  std::cout << "Pocitam pozadovane hodnoty..." << std::endl;

#pragma omp parallel sections
  {
#pragma omp section
    {
      auto sum = std::ranges::fold_left(numbers, 0.0, std::plus{});
      prumer = sum / std::ranges::distance(numbers);
    }
#pragma omp section
    {
      maximum = std::ranges::max(numbers);
    }

#pragma omp section
    {
      minimum = std::ranges::min(numbers);
    }

#pragma omp section
    {
      pocetSudych =
          std::ranges::count_if(numbers, [](int x) { return x % 2 == 0; });
    }
  }

  std::cout << "Pocitam standardni odchylku..." << std::endl;

#pragma omp parallel for reduction(+ : standardniOdchylka)
  for (int i = 0; i < NumberCount; i++) {
    double diff = (numbers[i] - prumer);
    double deviation = diff * diff;
    // normalized here for better precision - it might be slower because
    // division in for-loop
    double normalized_deviation = deviation / NumberCount;

    standardniOdchylka += normalized_deviation;
  }

  standardniOdchylka = std::sqrt(standardniOdchylka);

  std::cout << "Aritmeticky prumer:  " << prumer << std::endl;
  std::cout << "Standardni odchylka: " << standardniOdchylka << std::endl;
  std::cout << "Maximum:             " << maximum << std::endl;
  std::cout << "Minimum:             " << minimum << std::endl;
  std::cout << "Pocet sudych cisel:  " << pocetSudych << std::endl;

  return 0;
}
