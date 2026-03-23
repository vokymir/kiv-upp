#pragma once

#include <future>
#include <thread>

// rozsah
struct TRange {
  size_t begin;
  size_t end;
};

// redukcni operator - abstraktni predek (respektuje navrhovy vzor CRTP)
template <typename TResult_Type, typename TChild> class IReduction_Operator {
public:
  // rozdeli praci na vice kusu
  virtual void split(const TChild &work) = 0;

  // spoji vysledkyy prace do jednoho
  virtual void join(const TChild &result) = 0;

  // zpracuje kus prace prideleny ridicim vlaknem
  virtual void work(const TRange &range) = 0;

  // ziska vysledek
  virtual TResult_Type Get_Result() const = 0;
};

// ridici kod redukce
template <typename TOp> class CReductor {

public:
  // spousti redukce
  void Reduce(TOp &base, const TRange &range, int concurrency = -1) {

    // pokud nikdo nezadal miru paralelizace, vyplnime to "optimalni" variantou
    if (concurrency == -1)
      concurrency = std::thread::hardware_concurrency();

    // pokud uz nelze dale delit, spustime praci na nasem dale nedelitelnem dilu
    // prace
    if (concurrency == 1 || concurrency >= range.end - range.begin) {
      base.work(range);
      return;
    }

    // vytvorime potomka operatoru
    TOp child;
    // rozdelime potomka
    child.split(base);

    // cachovana hodnota
    size_t range_half = (range.end + range.begin) / 2;

    // async - asynchronne vykonavana funkce (lambda)
    std::future<bool> fut = std::async(std::launch::async, [&]() -> bool {
      // vola Reduce s jinym range
      Reduce(child, {range_half, range.end}, concurrency / 2);
      return true;
    });

    // v tomto vlakne taktez provadime reduce
    Reduce(base, {range.begin, range_half}, concurrency / 2);

    // pockame na vysledek
    fut.wait();

    // spojime vysledky metodou join
    base.join(child);
  }
};
