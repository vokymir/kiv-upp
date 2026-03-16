#pragma once

#include <thread>

#include "work.h"

// trida reprezentujici workera
class CWord_Worker {
private:
  // odkaz na pracovni kanal
  CWork_Channel &mChannel;

  // vlakno workera
  std::unique_ptr<std::thread> mThread;

  // odkaz na seznam hledanych slov
  const std::list<std::string> &mSearch_Words;

protected:
  // metoda vlakna
  void _Run();

  // zpracuje kus prace
  void Work(const TWork_Item &item);

public:
  CWord_Worker(CWork_Channel &channel, const std::list<std::string> &words)
      : mChannel(channel), mSearch_Words(words) {}

  // spusti workera (jeho vlakno)
  void Run();

  // pocka, az je worker ukoncen (ukonceni spousti zavreni pracovniho kanalu)
  void Wait();
};
