#pragma once

#include <condition_variable>
#include <cstddef>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

// jednotka prace - zadane slovo
struct TWork_Item {
  std::vector<std::string> words;
};

// jednotka vysledku - nalezene podslovo v permutaci
struct TWork_Result {
  // puvodni slovo
  std::string word;
  // jeho permutace
  std::string permutation;
  // nalezeny podretezec
  std::string substr;
};

class CWork_Channel {
private:
  // na teto podminkove promenne se blokuji workeri pri zadani o praci
  std::condition_variable mWorker_Cv;
  // na teto podminkove promenne se blokuje farmer pri cekani na pozadavek o
  // praci
  std::condition_variable mFarmer_Cv;
  // na tomto mutexu se blokuji workeri pri zadani o praci
  std::mutex mShared_Mtx;
  // mutex pro vkladani do fronty vysledku
  std::mutex mResult_Mtx;

  // pocet cekajicich workeru v Request_Work
  size_t mWaiting_Count = 0;

  // ulozeny work item
  TWork_Item mStored_Work_Item;
  // je ulozeny work item platny?
  bool mValid;

  // je stale v provozu?
  bool mRunning = true;

  // shromazdene vysledky
  std::list<TWork_Result> mResults;

public:
  constexpr static size_t GRANULARITY = 1;
  // farmer ceka na request o praci
  bool Wait_For_Request();

  // worker si zada o praci
  bool Request_Work(TWork_Item &target);

  // farmer nabizi praci - toto vyzaduje, aby nejaky worker byl zablokovany v
  // metode Request_Work
  bool Offer_Work(TWork_Item &&item);

  // worker odevzdava praci
  void Store_Result(TWork_Result &&result);

  // ziska vysledky
  const std::list<TWork_Result> &Get_Results() const;

  // ukonci praci
  void Terminate();
};
