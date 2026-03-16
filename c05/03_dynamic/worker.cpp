#include "worker.h"

#include <algorithm>
#include <vector>

void CWord_Worker::Run() {
  mThread = std::make_unique<std::thread>(&CWord_Worker::_Run, this);
}

void CWord_Worker::Wait() {
  if (mThread && mThread->joinable())
    mThread->join();
}

void CWord_Worker::_Run() {
  TWork_Item item;

  // dokud jde z kanalu vyziskat nejaka prace... (temer vzdy blokuje)
  while (mChannel.Request_Work(item)) {
    // zpracujeme co je treba
    Work(item);
  }
}

void CWord_Worker::Work(const TWork_Item &item) {
  // lambda funkce - vyzkousi permutaci, zda vyhovuje (obsahuje nektere z
  // podslov); pokud ano, oznami vysledek a vraci true
  auto try_permutation = [&](const std::string &str) -> bool {
    for (auto &s : mSearch_Words) {
      if (str.find(s) != std::string::npos) {
        mChannel.Store_Result({item.word, str, s});
        return true;
      }
    }

    return false;
  };

  std::string tmp = item.word;

  std::vector<int> c(tmp.size());
  std::fill(c.begin(), c.end(), 0);

  // musime zkusit i zakladni slovo (0. permutace)
  if (try_permutation(tmp))
    return;

  // vyzkousime vsechny permutace (pozor, potencialne faktorialni slozitost)
  while (std::next_permutation(tmp.begin(), tmp.end())) {
    if (try_permutation(tmp))
      break;
  }
}
