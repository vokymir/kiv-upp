#include "work.h"

bool CWork_Channel::Wait_For_Request() {
  // zamkneme sdileny mutex
  std::unique_lock<std::mutex> lck(mShared_Mtx);

  // dokud nikdo neceka na praci, zablokujeme se na podminkove promenne
  // popr. pokud nekdo ukonci vypocet, taktez se odblokujeme
  while (mWaiting_Count == 0 && mRunning)
    mFarmer_Cv.wait(lck);

  // pokud nekdo ukoncil vypocet, vracime false --> indikace k ukonceni
  if (!mRunning)
    return false;

  return true;
}

bool CWork_Channel::Request_Work(TWork_Item &target) {
  // zamkneme sdileny mutex
  std::unique_lock<std::mutex> lck(mShared_Mtx);

  // zvysime pocet cekajicich
  mWaiting_Count++;

  // dokud neni platna prace k dispozici, notifikujeme farmer proces a
  // zablokujeme se na nasi podminkove promenne popr. pokud nekdo ukonci
  // vypocet, odblokujeme se a ukoncime
  while (!mValid && mRunning) {
    mFarmer_Cv.notify_one();
    mWorker_Cv.wait(lck);
  }

  // dekrementujeme pocet cekajicich
  mWaiting_Count--;

  // nekdo ukoncil vypocet - vracime false, coz ukonci worker vlakno
  if (!mRunning)
    return false;

  // presuneme platnou praci
  target = std::move(mStored_Work_Item);
  mValid = false;

  return true;
}

bool CWork_Channel::Offer_Work(TWork_Item &&item) {
  // zamkneme sdileny zamek
  std::unique_lock<std::mutex> lck(mShared_Mtx);

  // v tento moment farmer predpoklada, ze pokud nekdo zrovna neukoncuje
  // vypocet, tak nejaky worker ceka na praci a je tedy bezpecne mu ji pridelit
  // pokud ale byli workeri dva (a vice), je treba pockat, az si prvni z nich
  // praci vyzvedne lze ocekavat, ze tomu tak bude hned pri dalsim naplanovani
  // (uz je davno probuzeny) a tak muzeme cekat bezpecne a efektivne takto:

  while (mValid && mRunning) {
    lck.unlock(); // toto umozni naplanovani workera - bude moci do kriticke
                  // sekce chranene stejnym mutexem
    std::this_thread::yield(); // predame rizeni jinemu vlaknu, s trochou stesti
                               // to bude brzy inkriminovane worker vlakno
    lck.lock(); // hned po probuzeni vsak musime mutex zase zamknout
  }

  // opet musime myslet na situaci, kdyby nas nekdo chtel ukoncit v prubehu
  if (!mRunning)
    return false;

  // ulozime praci do vnitrniho bufferu
  mStored_Work_Item = item;
  mValid = true;

  // a notifikujeme jednoho workera
  mWorker_Cv.notify_one();

  return true;
}

void CWork_Channel::Store_Result(TWork_Result &&result) {
  // vysledky jsou zamykany jinym mutexem, abychom neblokovali zbytecne ostatni
  // - jedna se o jinou pamet
  std::unique_lock<std::mutex> lck(mResult_Mtx);

  mResults.push_back(result);
}

const std::list<TWork_Result> &CWork_Channel::Get_Results() const {
  return mResults;
}

void CWork_Channel::Terminate() {
  // zamkneme sdileny mutex
  std::unique_lock<std::mutex> lck(mShared_Mtx);

  // priznak
  mRunning = false;
  // a notifikujeme vsechna vlakna
  mWorker_Cv.notify_all();
  mFarmer_Cv.notify_all();

  // farmer nebo worker jsou bud:
  // 1) mimo kritickou sekci --> pak se budou snazit do ni brzy vejit, a zjisti,
  // ze mRunning je false a ukonci se 2) v kriticke sekci --> pak se brzy
  // zablokuje nad podminkovou promennou (a az teprve potom je zde notifikovan)
  // nebo brzy odejde (a zjisti to v dalsi iteraci) 3) blokovany nad podminkovou
  // promennou --> pak hned po probuzeni zjisti, ze mRunning je false a ukonci
  // se
}
