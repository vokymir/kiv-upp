#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <vector>

#include <barrier>

class CBarrier {
private:
  // pocatecni pocet
  const int mInitial;

  // POZOR: tady je klicove slovo volatile naduzito! V soucasne implementaci
  // (aktivni cekani) je ale potreba, jinak kompilator cyklus vyoptimalizuje
  //        radi bychom se ho zbavili, tady nema svoje misto. Jak na to?

  // pocitadlo procesu
  int mCounter;
  // odpocet vlaken uvnitr
  int mLeft_counter; // je bariera signalizovana?
  bool mSignalized;

  std::condition_variable left_cv;
  std::condition_variable signal_cv;

  // synchronizacni zamek
  std::mutex mLock;

public:
  CBarrier(int initial) : mInitial(initial) {
    mCounter = initial;
    mLeft_counter = 0;
    mSignalized = false;
  }

  void count_down_and_wait() {
    std::unique_lock<std::mutex> lck(mLock);

    if (mCounter == mInitial) {
      left_cv.wait(lck, [&]() { return mLeft_counter == 0; });

      mSignalized = false;
    }

    mCounter--;
    if (mCounter == 0) {
      mLeft_counter = mInitial;
      mCounter = mInitial;
      mSignalized = true;
      signal_cv.notify_all();
    }

    signal_cv.wait(lck, [&]() { return mSignalized; });

    mLeft_counter--;
    if (mLeft_counter == 0) {
      left_cv.notify_all();
    }
  }
};

constexpr size_t Thread_Count = 12;

std::random_device rdev;
std::default_random_engine reng(rdev());
std::uniform_int_distribution<size_t> rdist(200, 2000);

void Thread_Func(CBarrier &barrier) {
  int iter = 0;

  while (true) {
    // simulujeme nejakou praci
    std::this_thread::sleep_for(std::chrono::milliseconds(rdist(reng)));

    // a pockame na ostatni vlakna
    barrier.count_down_and_wait();

    // vypis -- tohle by mela vypisovat vsechna vlakna "najednou"
    std::cout << "ITERACE: " << iter++ << std::endl;
  }
}

int main(int argc, char **argv) {
  CBarrier barrier(Thread_Count);

  std::vector<std::unique_ptr<std::thread>> threads;

  for (size_t i = 0; i < Thread_Count; i++)
    threads.push_back(
        std::make_unique<std::thread>(&Thread_Func, std::ref(barrier)));

  for (size_t i = 0; i < Thread_Count; i++) {
    if (threads[i]->joinable())
      threads[i]->join();
  }

  return 0;
}
