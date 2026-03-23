#include <iostream>

#include <fstream>
#include <random>
#include <string>

#include "reductor.h"

constexpr size_t NumCount = 10'000'000;

// samotna implementace redukcniho operatoru "MIN" pres obsah souboru
class CFile_Min_Reduction_Operator
    : public IReduction_Operator<double, CFile_Min_Reduction_Operator> {
private:
  // nazev souboru k analyze
  std::string mFilename;

  // lokalni minimum - abychom nemodifikovali sdilenou zavislou promennou
  double mLocal_Min = std::numeric_limits<double>::max();

public:
  // vychozi konstruktor - prazdny
  CFile_Min_Reduction_Operator() = default;

  // pocatecni konstruktor
  CFile_Min_Reduction_Operator(const std::string &filename)
      : mFilename(filename) {
    //
  }

  // split - pro oddeleni prace z nadrazene urovne
  void split(const CFile_Min_Reduction_Operator &work) override {
    std::cout << "split!" << std::endl;
    mFilename = work.mFilename;
  }

  // join - pro spojeni dilu prace
  void join(const CFile_Min_Reduction_Operator &other) override {
    std::cout << "join!" << std::endl;
    mLocal_Min =
        (other.mLocal_Min < mLocal_Min) ? other.mLocal_Min : mLocal_Min;
  }

  // vlastni prace - hledani minima v souboru
  void work(const TRange &range) override {
    std::cout << "work " << range.begin << " - " << range.end << std::endl;

    // otevreme soubor a doseekujeme se na pocatecni pozici
    std::ifstream fs(mFilename, std::ios::binary | std::ios::in);
    fs.seekg(range.begin, std::ios::beg);

    double num;

    // cteme, dokud jsme pred koncovou zarazkou
    while (fs.tellg() < range.end) {
      fs.read(reinterpret_cast<char *>(&num), sizeof(double));
      // updatovat lokalni minimum pokud je to treba
      mLocal_Min = num < mLocal_Min ? num : mLocal_Min;
    }
  }

  // ziskani vysledku
  double Get_Result() const override { return mLocal_Min; }
};

int main(int argc, char **argv) {
  // otevreme soubor
  std::ifstream ifs("data.bin", std::ios::binary | std::ios::in);
  if (!ifs.is_open()) {
    std::cerr << "Nelze nacist soubor data.bin! Bude nyni vygenerovan."
              << std::endl;

    std::cout << "Generator generuje " << NumCount << " cisel ..." << std::endl;

    std::random_device rdev;
    std::default_random_engine reng(rdev());
    std::normal_distribution<double> rdist(0, 90.0);

    std::ofstream of("data.bin", std::ios::binary | std::ios::out);
    for (size_t i = 0; i < NumCount; i++) {
      const double num = rdist(reng);
      of.write(reinterpret_cast<const char *>(&num), sizeof(double));
    }

    std::cout << "Cisla byla vygenerovana!" << std::endl;
    return 0;
  }

  // zjistime jak je velky
  ifs.seekg(0, std::ios::end);
  const size_t size = static_cast<size_t>(ifs.tellg());
  ifs.seekg(0, std::ios::beg);

  // prvni (bazova) instance operatoru
  CFile_Min_Reduction_Operator op("data.bin");
  // ridici kod - reduktor
  CReductor<CFile_Min_Reduction_Operator> reductor;

  // spustime redukci! Tohle bude prubezne spawnovat vlakna dle potreby
  reductor.Reduce(op, {0, size});

  std::cout << "Vysledek: " << op.Get_Result() << std::endl;

  return 0;
}
