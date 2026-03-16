#include "config.h"
#include "matrix.h"

#include <condition_variable>
#include <mutex>
#include <thread>

#include <cmath>

// redukce - jen nejaky soucet vsech prvku, abychom meli co rychle porovnat
static double Reduce_Matrix(const Matrix &in) {
  double acc = 0;

  for (size_t dst_x = 0; dst_x < in.size(); dst_x++) {
    for (size_t dst_y = 0; dst_y < in[dst_x].size(); dst_y++)
      acc += in[dst_x][dst_y];
  }

  return acc;
}

double perform_parallel(const Matrix &a, const Matrix &b) {
  Matrix c;

  Matrix lcms;

  std::mutex mtx;
  std::condition_variable cv;

  // pro tento pokus musi byt matice stejne velke
  if (a.size() != b.size() || a[0].size() != b[0].size())
    return std::numeric_limits<double>::quiet_NaN();

  c.resize(a.size());
  for (auto &v : c)
    v.resize(a[0].size());

  lcms.resize(a.size());
  for (auto &v : lcms) {
    v.resize(a[0].size());
    std::fill(v.begin(), v.end(), std::numeric_limits<double>::quiet_NaN());
  }

  // inicializujeme prvni prvek
  c[0][0] = a[0][0] + b[0][0];

  // NOTE: toto reseni vyzaduje O(MxN) pameti navic, takze to neni uplne
  // nejefektivnejsi

  // operace post() -> vlozi vypocteny mezivysledek do mezimatice
  auto post = [&](size_t x, size_t y, double val) {
    lcms[x][y] = val;
    cv.notify_one();
  };

  // operace wait() -> pocka, az bude prvek k dispozici a pak ho vrati
  auto wait = [&](size_t x, size_t y) -> double {
    // vyplatilo by se aktivni cekani vice, nez blokovani?
    std::unique_lock<std::mutex> lck(mtx);
    while (std::isnan(lcms[x][y]))
      cv.wait(lck);

    return lcms[x][y];
  };

  // vlakno pocitajici mezivysledky
  std::thread lcm_thread([&]() {
    for (size_t x = 0; x < a.size(); x++) {
      for (size_t y = (x == 0) ? 1 : 0; y < a[x].size(); y++) {
        double tmp = 0;

        if constexpr (UsedVariant == ComputationVariant::LL)
          tmp = lcm(a[x][y], b[x][y]);
        else if constexpr (UsedVariant == ComputationVariant::LG)
          tmp = gcd(a[x][y], b[x][y]);
        else if constexpr (UsedVariant == ComputationVariant::GL)
          tmp = lcm(a[x][y], b[x][y]);
        else if constexpr (UsedVariant == ComputationVariant::GG)
          tmp = gcd(a[x][y], b[x][y]);

        post(x, y, tmp);
      }
    }
  });

  // vlakno ktere finalizuje vysledky
  std::thread calc_thread([&]() {
    for (size_t x = 0; x < a.size(); x++) {
      for (size_t y = (x == 0) ? 1 : 0; y < a[x].size(); y++) {
        const size_t src_x = (y == 0) ? x - 1 : x;
        const size_t src_y = (y == 0) ? a[x].size() - 1 : y - 1;

        if constexpr (UsedVariant == ComputationVariant::LL)
          c[x][y] = lcm(c[src_x][src_y], wait(x, y));
        else if constexpr (UsedVariant == ComputationVariant::LG)
          c[x][y] = lcm(c[src_x][src_y], wait(x, y));
        else if constexpr (UsedVariant == ComputationVariant::GL)
          c[x][y] = gcd(c[src_x][src_y], wait(x, y));
        else if constexpr (UsedVariant == ComputationVariant::GG)
          c[x][y] = gcd(c[src_x][src_y], wait(x, y));
      }
    }
  });

  // pockame na obe vlakna az skonci

  if (lcm_thread.joinable())
    lcm_thread.join();

  if (calc_thread.joinable())
    calc_thread.join();

  return Reduce_Matrix(c);
}
