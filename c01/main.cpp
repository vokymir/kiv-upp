#include <print>
#include <thread>

int main(int argc, char **argv) {

  std::thread t1([]() {
    for (int i = 0; i < 100; ++i)
      std::print("A");
  });

  if (t1.joinable())
    t1.join();

  for (int i = 0; i < 100; ++i)
    std::print("B");

  return 0;
}
