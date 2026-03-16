#include "matrix.h"

#include <memory>
#include <thread>

static bool Matrix_Multiply(const Matrix &a, const Matrix &b, Matrix &result) {
  // lze vubec matice nasobit?
  if (a.size() != b[0].size())
    return false;

  // pripravme si vyslednou matici
  result.resize(b.size());
  for (auto &v : result)
    v.resize(a[0].size());

  // vektor vlaken (workeru)
  std::vector<std::unique_ptr<std::thread>> workers;

  // jakou cast prace bude kazde vlakno zpracovavat? pro ted neresme zarovnani
  // (tedy pokud by velikost byla necelym nasobkem casti)
  // std::thread::hardware_concurrency() vraci pocet jader procesoru
  // (virtualnich, pokud je CPU ma, popr. jen fyzickych, pokud ne)
  const size_t portion = result.size() / std::thread::hardware_concurrency();

  // vytvorime N workeru podle poctu jader CPU
  for (size_t n = 0; n < std::thread::hardware_concurrency(); n++) {
    // a kazde vlakno nechame vykonavat funkci nasobeni matice pro cast radku
    // cast radku je vymezena vstupnimi parametry begin a end - ty se do lambda
    // funkce predavaji nize
    workers.push_back(std::make_unique<std::thread>(
        [&](size_t begin, size_t end) {
          for (size_t dst_x = begin; dst_x < end; dst_x++) {
            for (size_t dst_y = 0; dst_y < result[dst_x].size(); dst_y++) {
              result[dst_x][dst_y] = 0;

              for (size_t x = 0; x < a.size(); x++)
                result[dst_x][dst_y] += a[x][dst_y] * b[dst_x][x];
            }
          }
        },
        n * portion,      // -> begin
        (n + 1) * portion // -> end
        ));
  }

  // pockame na vsechny workery, az dokonci co maji
  for (auto &w : workers) {
    if (w->joinable())
      w->join();
  }

  return true;
}

// redukce - jen nejaky soucet vsech prvku, abychom meli co rychle porovnat
static double Reduce_Matrix(const Matrix &in) {
  double acc = 0;

  for (size_t dst_x = 0; dst_x < in.size(); dst_x++) {
    for (size_t dst_y = 0; dst_y < in[dst_x].size(); dst_y++)
      acc += in[dst_x][dst_y];
  }

  return acc;
}

// paralelni verze algoritmu se statickym delenim prace
double perform_parallel_static(const Matrix &a, const Matrix &b) {
  Matrix c;
  Matrix d;
  for (size_t i = 0; i < 20; i++) {
    if (!Matrix_Multiply(a, b, c))
      return std::numeric_limits<double>::quiet_NaN();

    if (!Matrix_Multiply(a, c, d))
      return std::numeric_limits<double>::quiet_NaN();

    if (!Matrix_Multiply(a, d, c))
      return std::numeric_limits<double>::quiet_NaN();

    if (!Matrix_Multiply(b, c, d))
      return std::numeric_limits<double>::quiet_NaN();

    if (!Matrix_Multiply(a, c, d))
      return std::numeric_limits<double>::quiet_NaN();

    if (!Matrix_Multiply(b, d, c))
      return std::numeric_limits<double>::quiet_NaN();
  }

  return Reduce_Matrix(c);
}
