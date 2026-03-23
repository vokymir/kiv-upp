#include "reduce.h"

#include <atomic>
#include <thread>

#include <memory>

void summator(const std::vector<double> &numbers, double &partialSumTarget,
              size_t begin, size_t end) {
  // nyni nepotrebujeme zadnou slozitejsi logiku (ani atomic) - scitame do
  // nezavisle promenne

  for (size_t i = begin; i < end; i++)
    partialSumTarget += numbers[i];
}

double sum_parallel_independent(const std::vector<double> &numbers) {
  // vektor workeru
  std::vector<std::unique_ptr<std::thread>> workers;

  const size_t worker_count = std::thread::hardware_concurrency();
  const size_t portion = numbers.size() / worker_count;

  // vektor castecnych souctu (nezavisle promenne, kazde vlakno ma svuj index)
  std::vector<double> partialSums(worker_count);
  std::fill(partialSums.begin(), partialSums.end(), 0); // vynulujeme

  // vytvorime workery
  for (size_t i = 0; i < worker_count; i++) {
    workers.push_back(std::make_unique<std::thread>(
        &summator, std::ref(numbers), std::ref(partialSums[i]), i * portion,
        (i + 1) * portion));
  }

  // vnejsi vlakno ("farmer") pak nascita mezisoucty
  double accumulator = 0;

  for (size_t i = 0; i < worker_count; i++) {
    // pockame na workera
    if (workers[i]->joinable())
      workers[i]->join();

    // a pak hned pricteme jeho mezivysledek
    accumulator += partialSums[i];
  }

  return accumulator;
}
