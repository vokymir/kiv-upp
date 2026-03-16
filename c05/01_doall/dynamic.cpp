#include "matrix.h"

#include <list>
#include <memory>
#include <mutex>
#include <thread>

// trida pracovni fronty pro ziskavani prace
// zakladni zpusob dynamicke delby prace muze vyuzivat treba frontu
class CWork_Queue {
private:
  // fronta prace (par X:Y v matici)
  std::list<std::pair<size_t, size_t>> mWork;
  // zamek fronty
  std::mutex mMtx;

public:
  // vlozi prvek do fronty - nezamykane, predpoklada ze vklada jen jedno vlakno
  // (farmer) predtim, nez se worker vlakna vubec zapnou
  void push(size_t x, size_t y) { mWork.push_back(std::make_pair(x, y)); }

  // vybere z fronty prvek a roztridi ho do vystupnich parametru (x, y); pokud
  // neni nic ve fronte, vraci false, jinak true
  bool pop(size_t &x, size_t &y) {
    std::unique_lock<std::mutex> lck(mMtx);

    if (mWork.empty())
      return false;

    std::tie(x, y) = mWork.front();
    mWork.pop_front();

    return true;
  }
};

static bool Matrix_Multiply(const Matrix &a, const Matrix &b, Matrix &result) {
  // lze vubec matice nasobit?
  if (a.size() != b[0].size())
    return false;

  // pripravme si vyslednou matici
  result.resize(b.size());
  for (auto &v : result)
    v.resize(a[0].size());

  CWork_Queue queue;

  // farmer naplni frontu ukolu
  for (size_t dst_x = 0; dst_x < result.size(); dst_x++) {
    for (size_t dst_y = 0; dst_y < result[dst_x].size(); dst_y++)
      queue.push(dst_x, dst_y);
  }

  std::vector<std::unique_ptr<std::thread>> workers;

  // vytvorime N workeru podle CPU jader
  for (size_t n = 0; n < std::thread::hardware_concurrency(); n++) {
    // kazdy worker ma stejne parametry
    workers.push_back(std::make_unique<std::thread>([&]() {
      size_t dst_x, dst_y;

      // dokud je co zpracovavat...
      while (queue.pop(dst_x, dst_y)) {
        // vyhodnotime jeden prvek vysledne matice
        // (provedeme jeden skalarni soucin radku a sloupce)

        // result[dst_x][dst_y] = 0;
        double tmp = 0;

        for (size_t x = 0; x < a.size(); x++)
          // result[dst_x][dst_y] += a[x][dst_y] * b[dst_x][x];
          tmp += a[x][dst_y] * b[dst_x][x];

        result[dst_x][dst_y] = tmp;
      }
    }));
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

// paralelni verze algoritmu s dynamickym delenim prace
double perform_parallel_dynamic(const Matrix &a, const Matrix &b) {
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
