#include <iostream>

#include <algorithm>
#include <chrono>
#include <future>
#include <thread>
#include <vector>

// nejaka relativne neuzitecna funkce, ktera vygeneruje pole cisel, seradi ho a
// vrati nahodny prvek z nej
int merena_funkce_1() {
  std::vector<int> numbers(10000000);

  auto f = []() -> int { return rand() % 10000 - 5000; };

  std::generate(numbers.begin(), numbers.end(), f);
  std::sort(numbers.begin(), numbers.end());

  return numbers[rand() % numbers.size()];
}

// nejaka relativne neuzitecna funkce, ktera vygeneruje pole cisel, seradi ho a
// vrati nahodny prvek z nej
int merena_funkce_2() {
  std::vector<int> numbers(10000000);

  auto f = []() -> int { return rand() % 100000 - 50000; };

  std::generate(numbers.begin(), numbers.end(), f);
  std::sort(numbers.begin(), numbers.end());

  return numbers[rand() % numbers.size()];
}

int main(int argc, char **argv) {
  int result1, result2, result;
  std::chrono::steady_clock::time_point start, end;
  long long time_ms;
  srand(static_cast<unsigned int>(time(nullptr)));

  /*
   * Ukol pro vas: pocitat zrychleni z jednoho behu neni "fer" - na systemu s
   * preemptivnim planovacem, na kterem navic bezi spousty nejen systemovych
   * sluzeb a jinych aplikaci, jsou casy ruzne. Provedte totez mereni pro kazdou
   * z metod vicekrat a vypoctete urychleni 2. a 3. zpusobu vuci 1. Pokuste se
   * zduvodnit namerene/vypoctene hodnoty.
   */

  //// Mereni 1 - bez paralelizace
  const int runs = 10;
  long long sum = 0;
  for (auto i = 0; i < runs; i++) {
    start = std::chrono::steady_clock::now();

    result1 = merena_funkce_1();
    result2 = merena_funkce_2();
    result = result1 + result2;

    end = std::chrono::steady_clock::now();
    time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();
    sum += time_ms;

    // std::cout << "(1) Result: " << result << " in " << time_ms << "ms"
    //           << std::endl;
  }
  double avg = sum / static_cast<double>(runs);
  std::cout << "(1) in 10 runs: " << avg << std::endl;
  double first = avg;

  //// Mereni 2 - thread + join
  sum = 0;
  for (auto i = 0; i < runs; i++) {
    start = std::chrono::steady_clock::now();

    std::thread calc_thread([&result1]() { result1 = merena_funkce_1(); });
    result2 = merena_funkce_2();

    if (calc_thread.joinable())
      calc_thread.join();

    result = result1 + result2;

    end = std::chrono::steady_clock::now();
    time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();

    sum += time_ms;
    // std::cout << "(2) Result: " << result << " in " << time_ms << "ms"
    //           << std::endl;
  }
  avg = sum / static_cast<double>(runs);
  std::cout << "(2) in 10 runs: " << avg << std::endl;
  double second = avg;

  //// Mereni 3 - async
  sum = 0;
  for (auto i = 0; i < runs; i++) {
    start = std::chrono::steady_clock::now();

    auto res1_future = std::async(std::launch::async, &merena_funkce_1);
    result2 = merena_funkce_2();

    result1 = res1_future.get();

    result = result1 + result2;

    end = std::chrono::steady_clock::now();
    time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();

    sum += time_ms;
    // std::cout << "(3) Result: " << result << " in " << time_ms << "ms"
    //           << std::endl;
  }
  avg = sum / static_cast<double>(runs);
  std::cout << "(3) in 10 runs: " << avg << std::endl;
  double third = avg;

  double fst_to_scd = first / second;
  double fst_to_thd = first / third;

  std::cout << "1/2 = " << fst_to_scd << std::endl;
  std::cout << "1/3 = " << fst_to_thd << std::endl;

  return 0;
}
