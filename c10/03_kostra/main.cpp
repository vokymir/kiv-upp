#include <algorithm>
#include <array>
#include <execution>
#include <iostream>
#include <random>
#include <vector>

constexpr size_t NumbersCount = 1'000'000'00;

void fill_vector(std::vector<double> &vec, size_t num) {
  vec.resize(num);

  // generovani nahodnych cisel gaussovskym rozdelenim
  std::default_random_engine reng(4567890);
  std::normal_distribution<double> rdist(100.0, 3.0);
  // NOTE: generovat cisla chceme doopravdy seriove - generovani cisel jednim
  // generatorem neni thread-safe
  std::for_each(std::execution::seq, vec.begin(), vec.end(),
                [&](double &trg) { trg = rdist(reng); });
}

int main(int argc, char **argv) {
  std::vector<double> numbers;
  fill_vector(numbers, NumbersCount);

  /*
   * Itinerar algoritmu:
   *  - std::reduce - redukcni operace
   *  - std::transform - zmena prvku ve vektoru
   *  - std::for_each - prochazeni prvku vektoru
   *  - std::sort - serazeni prvku vektoru
   *  - std::minmax_element - najde minimum a maximum vektoru
   */

  /*
   * Vas ukol (vse paralelne):
   *  1) najdete minimum a maximum ve vektoru
   *  2) skalujte vsechny prvky vektoru na 0-1 (tj. minimalni prvek bude
   * odpovidat cislu 0, maximalni prvek cislu 1, vsechny ostatni budou v
   * rozmezi) 3) seradte vektor vzestupne
   */

  auto [min_p, max_p] = std::minmax_element(std::execution::par_unseq,
                                            numbers.begin(), numbers.end());
  auto min_val = *min_p;
  auto max_val = *max_p;
  auto diff = max_val - min_val;

  std::transform(std::execution::par_unseq, numbers.begin(), numbers.end(),
                 numbers.begin(), [min_val, diff](const double &val) {
                   return (val - min_val) / diff;
                 });

  std::sort(std::execution::par_unseq, numbers.begin(), numbers.end());

  // Vytiskneme prvnich 10 prvku pro kontrolu - mely by to byt same nuly
  std::cout << "Prvnich 10 prvku: ";
  for (size_t i = 0; i < 10 && i < numbers.size(); ++i) {
    std::cout << numbers[i] << " ";
  }
  // a poslednich 10 prvku pro kontrolu - mely by to byt same jednotky
  std::cout << std::endl << "Poslednich 10 prvku: ";
  for (size_t i = numbers.size() - 10; i < numbers.size(); ++i) {
    std::cout << numbers[i] << " ";
  }
  std::cout << std::endl;

  /*
   * Bonusovy ukol na doma: vykreslete histogram hodnot, cisla normalizovana na
   * rozsah 0-1 "roztridte" do binu po 0.05 (tj. prvni budou cisla od 0 do 0.05,
   * dalsi od 0.05 do 0.1 a tak dale) V nejvetsim binu (tj. nejvice hodnot)
   * vypisete 20 hvezdicek, ostatni proporcne zmensite Priklad vystupu:
   *
   * **
   * ****
   * ******
   * *********
   * *************
   * ****************
   * ******************
   * *******************
   * ********************
   * *******************
   * ******************
   * ****************
   * ************
   * *********
   * *****
   * **
   * *
   *
   */

  // config
  constexpr size_t Bin_Count = 20;
  constexpr double Bin_Size = 1.0 / Bin_Count;
  constexpr size_t Stars_Max = 20;

  // helper: sort val into correct bin, return bin index
  auto to_bin = [Bin_Count, Bin_Size](double val) -> size_t {
    return std::min(static_cast<size_t>(val / Bin_Size), Bin_Count - 1);
  };

  std::array<size_t, Bin_Count> bins = std::transform_reduce(
      std::execution::par, numbers.begin(), numbers.end(),

      // reduce into 0 initialized array
      std::array<size_t, Bin_Count>{},

      // reduce: sum histograms
      [](std::array<size_t, Bin_Count> a,
         const std::array<size_t, Bin_Count> &b) {
        for (size_t i = 0; i < Bin_Count; ++i)
          a[i] += b[i];
        return a;
      },

      // transform: histogram with single 1
      [&to_bin](double val) {
        std::array<size_t, Bin_Count> h{};
        h[to_bin(val)] = 1;
        return h;
      });

  size_t max_bin =
      *std::max_element(std::execution::par, bins.begin(), bins.end());

  for (size_t i = 0; i < Bin_Count; ++i) {
    size_t stars = (bins[i] * Stars_Max) / max_bin;

    for (size_t j = 0; j < stars; ++j) {
      std::cout << "*";
    }

    std::cout << "\n";
  }

  return 0;
}
