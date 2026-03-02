#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

// globalni mutex
std::mutex mutex;
// globalni podminkova promenna
std::condition_variable condvar;

// pracovni cislo - nejaky kus sdilene pameti
int cislo;
// ktere vlakno je ted "na tahu"? 1 nebo 2
volatile int turn = 1;

volatile bool should_end = false;

// prvni vlakno
void Thread_Fnc_1() {
  while (true) {
    std::unique_lock<std::mutex> lck(mutex);

    // pokud nejsme na tahu, pockame
    // while (turn != 1 && !should_end)
    //   condvar.wait(lck);

    // ekvivalentni zapis
    condvar.wait(lck, []() { return turn == 1 || should_end; });

    if (should_end) {
      condvar.notify_all();
      std::cout << "1 konci..." << std::endl;
      return;
    }

    std::cout << "=== VLAKNO 1" << std::endl;

    std::cout << "Pracuji... ";
    // predstirame nejakou praci
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // potom udelame neco s cislem
    cislo += 1;

    std::cout << "OK! cislo = " << cislo << std::endl;

    if (cislo == 71) {
      should_end = true;
    }

    // prepneme "tah" na druhe vlakno a notifikujeme kohokoliv kdo ted spi nad
    // touto podminkovou promennou
    turn = 2;
    condvar.notify_all();
  }
}

void Thread_Fnc_2() {
  while (true) {
    std::unique_lock<std::mutex> lck(mutex);

    while (turn != 2 && !should_end)
      condvar.wait(lck);

    if (should_end) {
      condvar.notify_all();
      std::cout << "2 konci..." << std::endl;
      return;
    }

    std::cout << "=== VLAKNO 2" << std::endl;

    std::cout << "Pracuji... ";
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    cislo += 2;

    std::cout << "OK! cislo = " << cislo << std::endl;

    if (cislo == 71) {
      should_end = true;
    }

    turn = 3;
    condvar.notify_all();
  }
}

void Thread_Fnc_3() {
  while (true) {
    std::unique_lock<std::mutex> lck(mutex);

    while (turn != 3 && !should_end)
      condvar.wait(lck);

    if (should_end) {
      condvar.notify_all();
      std::cout << "3 konci..." << std::endl;
      return;
    }

    std::cout << "=== VLAKNO 3" << std::endl;

    std::cout << "Pracuji... ";
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    cislo += 3;

    std::cout << "OK! cislo = " << cislo << std::endl;

    if (cislo == 71) {
      should_end = true;
    }

    turn = 1;
    condvar.notify_all();
  }
}

int main(int argc, char **argv) {
  cislo = 20;

  std::thread thr1(&Thread_Fnc_1);
  std::thread thr2(&Thread_Fnc_2);
  std::thread thr3(&Thread_Fnc_3);

  thr1.join();
  thr2.join();
  thr3.join();

  std::cout << "korektni konec " << std::endl;

  return 0;
}
