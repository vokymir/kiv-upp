#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#include "matrix.h"

// nacte matici o znamych rozmerech ze souboru do zadane struktury
bool Load_Matrix(Matrix &target, const std::string &filename, size_t width,
                 size_t height) {
  std::ifstream ifs(filename);
  if (!ifs.is_open())
    return false;

  // zvetsit na pozadovanou velikost
  target.resize(width);
  for (auto &v : target)
    v.resize(height);

  // a precist vsechny prvky matice
  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < height; j++) {
      if (!(ifs >> target[i][j]))
        return false;
    }
  }

  return true;
}

Matrix Blow_Up_Matrix(const Matrix &in) {
  Matrix out;
  out.resize(in.size() * 2);
  for (auto &v : out)
    v.resize(in[0].size() * 2);
  for (size_t i = 0; i < in.size(); i++) {
    for (size_t j = 0; j < in[i].size(); j++) {
      out[i * 2][j * 2] = in[i][j];
      out[i * 2 + 1][j * 2] = in[i][j];
      out[i * 2][j * 2 + 1] = in[i][j];
      out[i * 2 + 1][j * 2 + 1] = in[i][j];
    }
  }
  return out;
}

int main(int argc, char **argv) {
  Matrix m1, m2;

  // nacteni dvou matic
  if (!Load_Matrix(m1, "./01_doall/matrix1.txt", 200, 200) ||
      !Load_Matrix(m2, "./01_doall/matrix2.txt", 200, 200)) {
    std::cerr << "Chyba pri nacitani matic: nelze najit matrix1.txt a/nebo "
                 "matrix2.txt"
              << std::endl;
    return 1;
  }

  // umele zvetseni matic, abyste nestahovali vetsi archiv, nez je treba pro
  // testovani (a zaroven aby se dalo porovnat s vysledky)
  m1 = Blow_Up_Matrix(m1);
  m2 = Blow_Up_Matrix(m2);

  auto t1 = std::chrono::steady_clock::now();

  // (1) seriova verze
  // double res = perform_serial(m1, m2);
  // (2) paralelni verze se statickym rozdelenim prace
  double res = perform_parallel_static(m1, m2);
  // (3) paralelni verze s dynamickym delenim prace
  // double res = perform_parallel_dynamic(m1, m2);

  auto t2 = std::chrono::steady_clock::now();

  std::cout << res << std::endl;
  std::cout
      << "Cas vypoctu: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
      << "ms" << std::endl;

  return 0;
}
