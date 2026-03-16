#include "config.h"
#include "matrix.h"

#include <cmath>
#undef min

// redukce - jen nejaky soucet vsech prvku, abychom meli co rychle porovnat
static double Reduce_Matrix(const Matrix &in) {
  double acc = 0;

  for (size_t dst_x = 0; dst_x < in.size(); dst_x++) {
    for (size_t dst_y = 0; dst_y < in[dst_x].size(); dst_y++)
      acc += in[dst_x][dst_y];
  }

  return acc;
}

double perform_serial(const Matrix &a, const Matrix &b) {
  Matrix c;

  // pro tento pokus musi byt matice stejne velke
  if (a.size() != b.size() || a[0].size() != b[0].size())
    return std::numeric_limits<double>::quiet_NaN();

  c.resize(a.size());
  for (auto &v : c)
    v.resize(a[0].size());

  // inicializujeme prvni prvek
  c[0][0] = a[0][0] + b[0][0];

  // kazdy prvek vyhodnotime
  for (size_t x = 0; x < a.size(); x++) {
    for (size_t y = (x == 0) ? 1 : 0; y < a[x].size(); y++) {
      const size_t src_x = (y == 0) ? x - 1 : x;
      const size_t src_y = (y == 0) ? a[x].size() - 1 : y - 1;

      if constexpr (UsedVariant == ComputationVariant::LL)
        c[x][y] = lcm(c[src_x][src_y], lcm(a[x][y], b[x][y]));
      else if constexpr (UsedVariant == ComputationVariant::LG)
        c[x][y] = lcm(c[src_x][src_y], gcd(a[x][y], b[x][y]));
      else if constexpr (UsedVariant == ComputationVariant::GL)
        c[x][y] = gcd(c[src_x][src_y], lcm(a[x][y], b[x][y]));
      else if constexpr (UsedVariant == ComputationVariant::GG)
        c[x][y] = gcd(c[src_x][src_y], gcd(a[x][y], b[x][y]));
    }
  }

  return Reduce_Matrix(c);
}
