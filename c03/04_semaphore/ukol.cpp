#include <iostream>
#include <thread>

#include "semaphore.h"

/*
 * Vas ukol je jednoduchy:
 *  - nejprve opravte implementaci Semaphore, aby fungoval, jak ma
 *  + nemit aktivni cekani ale podminkovou promennou treba
 *  + udelat P,V atomicke
 *  - pote doplnte semafory nutne k synchronizaci nize uvedeneho kodu, aby se
 * vypsalo: Synchronizovat vlakna neni zadna sranda
 *    - neupravujte vypisy samotne! Donutte vlakna synchronizovat se tak, aby se
 * text spravne prostridal
 */

// TODO: SEM DOPLNTE SEMAFORY
Semaphore s1{0};

void A() {
  s1.P(1);
  std::cout << "vlakna "; // 1
  s1.V(2);

  s1.P(4);
  std::cout << "sranda "; // 4
}

void B() {
  std::cout << "Synchronizovat "; // 0
  s1.V(1);

  s1.P(2);
  std::cout << "neni "; // 2
  s1.V(3);
}

void C() {
  s1.P(3);
  std::cout << "zadna "; // 3
  s1.V(4);
}

// tuto funkci pak odnekud zavolejte - vymazte obsah main() a volejte ji
// tamodtud
void ukol() {

  std::cout << "Moudro dne: ";

  std::thread t1(A);
  std::thread t2(B);
  std::thread t3(C);

  t1.join();
  t2.join();
  t3.join();

  std::cout << std::endl;
}
