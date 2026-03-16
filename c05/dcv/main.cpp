#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <utility>
#include <vector>

// velikost obrazku MNIST datove sady
constexpr int ImageWidth = 28;
constexpr int ImageHeight = 28;

// predpocitane vahy pro klasifikaci; zjisteny z trenovaci sady
std::array<double, 10> avgrs = {1.65594, 1.00886, 1.27979, 1.19485, 1.42585,
                                1.19124, 1.39466, 1.14478, 1.48488, 1.32505};
std::array<double, 10> avgcs = {1.62732, 1.08876, 1.75115, 2.07454, 1.30706,
                                1.86389, 1.67646, 1.45069, 1.95342, 1.70459};

// nacteni BMP souboru ze souboru; bereme klasicky BMP soubor s 8 bity na pixel
std::vector<unsigned char> Load_BMP(const std::string &filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    std::cout << "Soubor " << filename << " se nepodarilo otevrit!"
              << std::endl;
    return {};
  }

  // nacteni BMP do matice (cernobily obrazek)
  unsigned char header[54];
  file.read(reinterpret_cast<char *>(header), 54);
  int width = header[18] + (header[19] << 8);
  int height = header[22] + (header[23] << 8);

  // preskocime offset na data
  file.seekg(header[10] + (header[11] << 8));

  const int size = width * height;
  std::vector<unsigned char> data(size);
  file.read(reinterpret_cast<char *>(data.data()), size);

  return data;
}

// otoceni obrazku - v BMP jsou kodovane "obracene"
void Transform(std::vector<unsigned char> &vec) {

  for (size_t i = 0; i < ImageHeight / 2; i++) {
    for (size_t j = 0; j < ImageWidth; j++) {
      // otocit vertikalne
      std::swap(vec[i * ImageWidth + j],
                vec[(ImageHeight - i - 1) * ImageWidth + j]);
    }
  }
}

// binarizace obrazku - zprahujeme na cernou a bilou, tj. true nebo false
std::vector<std::vector<bool>> Binarize(const std::vector<unsigned char> &vec) {
  std::vector<std::vector<bool>> result(ImageHeight,
                                        std::vector<bool>(ImageWidth));
  for (size_t i = 0; i < ImageHeight; i++) {
    for (size_t j = 0; j < ImageWidth; j++) {
      result[i][j] = (vec[i * ImageWidth + j] > 50);
    }
  }
  return result;
}

// jednoduchy klasifikator; druhy nejstupidnejsi hned po nahodnem hadani
int Classify(const std::vector<std::vector<bool>> &data) {

  // spocitame neprerusene sekvence jednicek v radcich a sloupcich
  std::vector<int> row_counts(ImageHeight);
  for (int i = 0; i < ImageHeight; i++) {
    int count = 0;
    for (int j = 0; j < ImageWidth; j++) {
      if (data[i][j]) {
        count++;
      } else {
        if (count > 0) {
          row_counts[i]++;
          count = 0;
        }
      }
    }
    if (count > 0) {
      row_counts[i]++;
    }
  }

  std::vector<int> col_counts(ImageWidth);
  for (int j = 0; j < ImageWidth; j++) {
    int count = 0;
    for (int i = 0; i < ImageHeight; i++) {
      if (data[i][j]) {
        count++;
      } else {
        if (count > 0) {
          col_counts[j]++;
          count = 0;
        }
      }
    }
    if (count > 0) {
      col_counts[j]++;
    }
  }

  // zprumerujeme nenulove hodnoty pro radky a sloupce
  double avg_row = 0;
  double avg_col = 0;
  int row_count = 0;
  int col_count = 0;
  for (int i = 0; i < ImageHeight; i++) {
    if (row_counts[i] > 0) {
      avg_row += row_counts[i];
      row_count++;
    }
  }
  for (int j = 0; j < ImageWidth; j++) {
    if (col_counts[j] > 0) {
      avg_col += col_counts[j];
      col_count++;
    }
  }
  avg_row /= row_count;
  avg_col /= col_count;

  // spocitame vzdalenost od predtrenovanych vah
  double dists[10];
  for (int i = 0; i < 10; i++) {
    dists[i] = sqrt((avg_row - avgrs[i]) * (avg_row - avgrs[i]) +
                    (avg_col - avgcs[i]) * (avg_col - avgcs[i]));
  }

  // nalezneme nejblizsi vahy, tj. nejblizsi cislo
  int min_index = 0;
  double min_dist = dists[0];
  for (int i = 1; i < 10; i++) {
    if (dists[i] < min_dist) {
      min_dist = dists[i];
      min_index = i;
    }
  }

  return min_index;
}

int main(int argc, char **argv) {

  if (!std::filesystem::is_directory("mnist")) {
    std::cout << "Slozka mnist neexistuje!" << std::endl;
    return 1;
  }

  /*
   * Ukazkovy priklad na pipelined multi-threading (PMT)
   * V soucasnem stavu je program single-threaded, tj. zpracovava obrazky
   * postupne Vasim ukolem je zpracovat obrazky paralelne, tj. vytvorit vlakna,
   * ktera budou zpracovavat obrazky v pipeline Napr.: zatimco jedno vlakno
   * klasifikuje, druhe muze nacitat dalsi obrazek
   *
   * Pokuste se maximalizovat pipelining, tj. vidite 4 vypocetni faze, takze
   * vytvorte 4 vlakna, kde kazde bude cekat na dokonceni predchozi faze a
   * jakmile bude dokoncena, spusti svou praci
   */

  /*
   * Predava si ve frontach ukoly. Neresil jsem soubehy pri pristupu do
   * front (u me to ma tak cca 20% sanci spadnout na bad_alloc).
   *
   * Hloupe aktivni cekani, protoze ocekavam, ze vsechny prace budou rychly a CV
   * by to akorat jeste zpomalila. Zaujalo me, ze aktivni cekani nefungovalo,
   * dokud jsem nepridal cekani (1ms) pri prazdne fronte. To si aktualne
   * nedokazu vysvetlit jinak, nez ze to kompilator nejak divne vyoptimalizoval
   * (-O3 -DNDEBUG).
   *
   * Seriove: ~120ms
   * Paralelne: ~480ms
   * */
  std::queue<std::pair<int, std::vector<unsigned char>>> data_loaded{};
  bool all_data_loaded = false;
  std::queue<std::pair<int, std::vector<unsigned char>>> data_transformed{};
  bool all_data_transformed = false;
  std::queue<std::pair<int, std::vector<std::vector<bool>>>> data_binarized{};
  bool all_data_binarized = false;
  std::queue<std::pair<int, int>> data_labeled{};
  bool all_data_labeled = false;

  auto tp_start = std::chrono::steady_clock::now();

  int total = 0;
  int correct = 0;

  std::thread t_loading([&]() {
    for (int num = 0; num <= 9; ++num) {
      for (auto d : std::filesystem::directory_iterator("mnist/" +
                                                        std::to_string(num))) {
        if (d.path().extension() != ".bmp") {
          continue;
        }
        auto data = Load_BMP(d.path().string());

        data_loaded.push({num, data});
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    all_data_loaded = true;
  });

  std::thread t_transform([&]() {
    while (!all_data_loaded || !data_loaded.empty()) {
      if (data_loaded.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        continue;
      }
      auto [num, data] = data_loaded.front();
      data_loaded.pop();

      Transform(data);

      data_transformed.push({num, data});
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    all_data_transformed = true;
  });

  std::thread t_binarize([&]() {
    while (!all_data_transformed || !data_transformed.empty()) {
      if (data_transformed.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        continue;
      }
      auto [num, data] = data_transformed.front();
      data_transformed.pop();

      auto binarized = Binarize(data);

      data_binarized.push({num, binarized});
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    all_data_binarized = true;
  });

  std::thread t_label([&]() {
    while (!all_data_binarized || !data_binarized.empty()) {
      if (data_binarized.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        continue;
      }
      auto [num, data] = data_binarized.front();
      data_binarized.pop();

      auto label = Classify(data);

      data_labeled.push({num, label});
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    all_data_labeled = true;
  });

  std::thread t_comp([&]() {
    while (!all_data_labeled || !data_labeled.empty()) {
      if (data_labeled.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        continue;
      }
      auto [num, label] = data_labeled.front();
      data_labeled.pop();

      if (label == num) {
        correct++;
      }
      total++;
    }
  });

  if (t_loading.joinable()) {
    t_loading.join();
  }
  if (t_transform.joinable()) {
    t_transform.join();
  }
  if (t_binarize.joinable()) {
    t_binarize.join();
  }
  if (t_label.joinable()) {
    t_label.join();
  }
  if (t_comp.joinable()) {
    t_comp.join();
  }

  std::cout << "Presnost: " << static_cast<double>(correct) / total * 100.0
            << "%" << std::endl;

  auto tp_end = std::chrono::steady_clock::now();

  std::cout << "Doba zpracovani: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(tp_end -
                                                                     tp_start)
                   .count()
            << "ms" << std::endl;

  return 0;
}
