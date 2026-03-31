#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
namespace parallel {

class Thread_Pool {
public:
  Thread_Pool(size_t num_threads = std::thread::hardware_concurrency()) {

    // Creating worker threads
    for (size_t i = 0; i < num_threads; ++i) {
      threads_.emplace_back([this] {
        // === WORKER THREAD ===
        while (true) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(queue_mutex_);

            cv_.wait(lock, [this] { return !tasks_.empty() || stop_; });

            if (stop_ && tasks_.empty()) {
              return;
            }

            task = std::move(tasks_.front());
            tasks_.pop();
          }

          task();
        }
      });
    }
  }

  ~Thread_Pool() {
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      stop_ = true;
    }

    cv_.notify_all();

    for (auto &thread : threads_) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  void enqueue(std::function<void()> task) {
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      tasks_.emplace(std::move(task));
    }
    cv_.notify_one();
  }

private:
  // store worker threads
  std::vector<std::thread> threads_;

  std::queue<std::function<void()>> tasks_;

  // synchronize access to shared data
  std::mutex queue_mutex_;

  // signal changes in the state of the tasks queue
  std::condition_variable cv_;

  bool stop_ = false;
};

} // namespace parallel
