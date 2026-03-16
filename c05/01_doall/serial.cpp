#include "matrix.h"

static bool Matrix_Multiply(const Matrix &a, const Matrix &b, Matrix &result) {
  // lze vubec matice nasobit?
  if (a.size() != b[0].size())
    return false;

  // pripravme si vyslednou matici
  result.resize(b.size());
  for (auto &v : result)
    v.resize(a[0].size());

  // pro kazdy sloupec vysledne matice
  for (size_t dst_x = 0; dst_x < result.size(); dst_x++) {
    // pro kazdy radek v kazdem sloupci vysledne matice
    for (size_t dst_y = 0; dst_y < result[dst_x].size(); dst_y++) {
      // vynulovat
      result[dst_x][dst_y] = 0;

      // a skalarne pronasobit radek matice A se sloupcem matice B (soucin prvku
      // na stejnych pozicich a soucet)
      for (size_t x = 0; x < a.size(); x++)
        result[dst_x][dst_y] += a[x][dst_y] * b[dst_x][x];
    }
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

// seriova verze algoritmu
double perform_serial(const Matrix &a, const Matrix &b) {
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
