#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include "config.h"
#include "matrix.h"

constexpr size_t Matrix_Size = 10000;

// vygeneruje nahodnou matici s fixnim seedem
bool Generate_Matrix(Matrix &target, size_t seed, size_t width, size_t height) {
  std::mt19937 gen(seed);
  std::uniform_real_distribution<double> dist(1.0, 7.0);

  // zvetsit na pozadovanou velikost
  target.resize(width);
  for (auto &v : target)
    v.resize(height);

  // a naplnit nahodnymi hodnotami
  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < height; j++) {
      target[i][j] = dist(gen);
    }
  }

  return true;
}

int main(int argc, char **argv) {
  Matrix m1, m2;

  // vygenerovani dvou matic
  if (!Generate_Matrix(m1, 999, Matrix_Size, Matrix_Size) ||
      !Generate_Matrix(m2, 888, Matrix_Size, Matrix_Size))
    return 1;

  auto t1 = std::chrono::steady_clock::now();

  double res;

  if (IsParallel) {
    // paralelni verze
    res = perform_parallel(m1, m2);
  } else {
    // seriova verze
    res = perform_serial(m1, m2);
  }

  auto t2 = std::chrono::steady_clock::now();

  std::cout << res << std::endl;
  std::cout
      << "Cas vypoctu: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
      << "ms" << std::endl;

  return 0;
}
