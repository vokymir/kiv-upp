#pragma once

#include <queue>
#include <string>

#include <condition_variable>
#include <mutex>

#include <memory>
#include <thread>

#include <fstream>
#include <list>

// marker trida
class IChannel {
public:
  virtual ~IChannel() = default;

  static IChannel empty;
};

// komunikacni kanal - sablona, parametrem je typ jednotky prace
template <typename TItem> class CChannel : public IChannel {
private:
  // sdileny mutex pro synchronizaci nad frontou
  std::mutex mMtx;
  // podminkova promenna pro notifikaci nad frontou
  std::condition_variable mCv;
  // priznak ukonceni vstupu
  bool mTerminated = false;

  // fronta prace
  std::queue<TItem> mQueue;

public:
  // posle do fronty praci ke zpracovani; vraci true pokud se povedlo, false
  // pokud je kanal uzavreny
  bool post(TItem &&item) {
    std::unique_lock<std::mutex> lck(mMtx);

    if (mTerminated) {
      return false;
    }

    mQueue.push(item);
    mCv.notify_one();

    return true;
  }

  // vybere z fronty praci ke zpracovani; vraci true pokud se povedlo, false
  // pokud je fronta prazdna a kanal uzavreny
  bool get(TItem &item) {
    std::unique_lock<std::mutex> lck(mMtx);

    // dokud je fronta prazdna a zaroven kanal neuzavreny
    while (mQueue.empty() && !mTerminated) {
      mCv.wait(lck);
    }

    // fronta je prazdna a zaroven kanal uzavreny - konec
    if (mQueue.empty() && mTerminated) {
      return false;
    }

    item = std::move(mQueue.front());
    mQueue.pop();

    return true;
  }

  // ukoncime kanal - vola vstupni konec, kdyz uz nema co posilat na vystup
  void terminate() {
    std::unique_lock<std::mutex> lck(mMtx);

    mTerminated = true;
    mCv.notify_all();
  }
};

// komponenta systemu
class IComponent {

protected:
  // vlakno komponenty
  std::unique_ptr<std::thread> mThread;

protected:
  // virtualni metoda, implementuje ji potomek, ale vola rodic skrze vlakno
  virtual void _Start(IChannel &input, IChannel &output) = 0;

public:
  // zapne komponentu (a jeji vlakno)
  void Start(IChannel &input, IChannel &output) {

    // zapnuti vlakna - klicka s lambda funkci, aby se zavolala polymorfni
    // metoda _Start
    mThread = std::make_unique<std::thread>(
        [&](IChannel &input, IChannel &output) { _Start(input, output); },
        std::ref(input), std::ref(output));
  }

  // pocka se na ukonceni komponenty (jejiho vlakna)
  void Wait() {
    if (mThread && mThread->joinable()) {
      mThread->join();
    }
  }
};

// cteci trida - reader
class CInput : public IComponent {
private:
  std::ifstream mInput;

protected:
  virtual void _Start(IChannel &input, IChannel &output) override;

public:
  CInput(const std::string &filename);
};

// typ kanalu pro komunikaci cteci tridy a splitteru - kus vstupu, nekolik
// celych radek
using CInput_Splitter_Channel = CChannel<std::string>;

// splitter trida
class CSplitter : public IComponent {
protected:
  virtual void _Start(IChannel &input, IChannel &output) override;
};

// typ kanalu pro komunikaci splitteru a mapperu - cela jedna radka
using CSplitter_Mapper_Channel = CChannel<std::string>;

// mapper trida
class CMapper : public IComponent {
protected:
  virtual void _Start(IChannel &input, IChannel &output) override;
};

// typ kanalu pro komunikaci mapperu a combineru - pary slovo + pocet vyskytu
using CMapper_Combiner_Channel = CChannel<std::pair<std::string, size_t>>;

// combiner trida
class CCombiner : public IComponent {
protected:
  virtual void _Start(IChannel &input, IChannel &output) override;
};

// typ kanalu pro komunikaci combineru a reduceru - pary slovo + seznam poctu
// vyskytu
using CCombiner_Reducer_Channel =
    CChannel<std::pair<std::string, std::list<size_t>>>;

// reducer trida
class CReducer : public IComponent {
protected:
  virtual void _Start(IChannel &input, IChannel &output) override;
};

// typ kanalu pro komunikaci reduceru a vystupni tridy - pary slovo + pocet
// vyskytu
using CReducer_Output_Channel = CChannel<std::pair<std::string, size_t>>;

// vystupni trida
class COutput : public IComponent {
protected:
  virtual void _Start(IChannel &input, IChannel &output) override;
};
