#include "reduce.h"

#include <atomic>
#include <thread>

#include <cassert>
#include <memory>

void summator(const std::vector<double> &numbers,
              std::atomic<double> &accumulator, size_t begin, size_t end) {
  // promenna pro starou hodnotu
  double old;

  for (size_t i = begin; i < end; i++) {
    // pozor: std::atomic<double> do C++20 nema atomickou variantu +=, ani
    // fetch_add, ani nic podobneho
    //        invokace CAS nebo LL/SC je tedy nutna

    // od C++20 lze delat tohle:
    // accumulator.fetch_add(numbers[i]);
    // nebo i tohle:
    // accumulator += numbers[i];
    // pro neuplnou podporu napric prekladaci ponechme "rozbalenou" verzi,
    // alespon si na tom ukazeme praci s CAS, resp. LL/SC

    do {
      // ulozime starou hodnotu, abychom meli pak co porovnavat (lze i bez
      // load())
      old = accumulator.load();

      // a pokusime se priradit novou hodnotu
      // compare_exchange_strong - pokud je accumulator rovno old, tak ho
      // nastavi na accumulator + numbers[i] tohle je vlastne neprima invokace
      // CAS nebo LL/SC instrukce na x86_64 se to napr. u me prelozilo na
      // instrukci: lock cmpxchg qword ptr [rbp],rcx
    } while (
        !accumulator.compare_exchange_strong(old, accumulator + numbers[i]));
  }
}

double sum_parallel_atomic(const std::vector<double> &numbers) {
  // sdilena uzamykana promenna - sem se nascita cele pole
  std::atomic<double> accumulator = 0;

  // vektor workeru
  std::vector<std::unique_ptr<std::thread>> workers;

  const size_t worker_count = std::thread::hardware_concurrency();
  const size_t portion = numbers.size() / worker_count;

  // vytvorime workery
  for (size_t i = 0; i < worker_count; i++) {
    workers.push_back(std::make_unique<std::thread>(
        &summator, std::ref(numbers), std::ref(accumulator), i * portion,
        (i + 1) * portion));
  }

  // pockame, az vsichni dokonci svou praci
  for (size_t i = 0; i < worker_count; i++) {
    if (workers[i]->joinable())
      workers[i]->join();
  }

  return accumulator;
}
