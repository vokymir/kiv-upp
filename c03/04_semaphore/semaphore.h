#pragma once

#include <cassert>
#include <condition_variable>
#include <mutex>

class Semaphore {
private:
  int counter_;
  std::mutex mutex_;
  std::condition_variable vodemceno;

public:
  Semaphore(int init) : counter_(init) {}

  void P(int cnt) {
    assert(cnt >= 0 && "Semaphore->P(count): count must be greater than zero.");

    std::unique_lock<std::mutex> lck(mutex_);

    vodemceno.wait(lck, [this, cnt]() { return this->counter_ >= cnt; });

    counter_ -= cnt;
  }

  void V(int cnt) {
    assert(cnt >= 0 && "Semaphore->V(count): count must be greater than zero.");

    std::unique_lock<std::mutex> lck(mutex_);

    counter_ += cnt;
    vodemceno.notify_all();
  }

  int Get() {
    std::lock_guard<std::mutex> lck(mutex_);
    return counter_;
  }
};
