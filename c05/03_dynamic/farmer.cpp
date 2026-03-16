#include "farmer.h"
#include "work.h"

#include <fstream>
#include <iostream>
#include <string>

void CWord_Farmer::Run() {
  // otevrit soubor
  std::ifstream words(mSource_File);
  if (!words.is_open())
    return;

  // vytvorime si pool workeru, predame jim pracovni kanal a seznam slov
  for (size_t i = 0; i < std::thread::hardware_concurrency(); i++)
    mWorkers.emplace_back(std::ref(mChannel), std::ref(mSearch_Words));

  // workery spustime (spusti si vlakno)
  for (auto &w : mWorkers)
    w.Run();

  // dokud jsou nejaka slova v souboru...
  while (words.good() && !words.eof()) {
    // pockame az si nekdo rekne o praci
    if (!mChannel.Wait_For_Request())
      break;

    // TODO: pridelovat praci po jednom slovu... neni to nahodou trochu
    // synchronizacni overkill? => zavedte granularitu prideleni prace
    TWork_Item item{{}};
    for (int i = 0; i < CWork_Channel::GRANULARITY; ++i) {
      std::string word;
      do {
        // precteme slovo
        if (!std::getline(words, word))
          break;

        // dokud nejake slovo je, popr. do doby nez nacteme takove, ktere ma
        // "dovoleny" pocet znaku
      } while (item.words.size() > mWord_Len_Limit);
      item.words.push_back(word);
    }

    // kdyz dojdeme na konec souboru...
    if (words.eof() || words.bad() || item.words.empty())
      break;

    // nabidneme praci - od momentu, kdy jsme se odblokovali z Wait_For_Request
    // nejaky worker ceka (mozna ceka i dele, pokud nebyl sam)
    if (!mChannel.Offer_Work(std::move(item)))
      break;
  }

  // po poslednim kusu prace ukoncime kanal (a tedy i workery)
  mChannel.Terminate();

  // a pockame, az se workeri ukonci
  for (auto &w : mWorkers)
    w.Wait();

  // ziskame a zobrazime vysledky
  auto &results = mChannel.Get_Results();

  for (auto &r : results)
    std::cout << r.word << " - " << r.permutation << " - " << r.substr
              << std::endl;
}

void CWord_Farmer::Set_Source_File(const std::string &filename) {
  mSource_File = filename;
}

void CWord_Farmer::Set_Word_Length_Limit(size_t s) { mWord_Len_Limit = s; }

void CWord_Farmer::Add_Search_Word(const std::string &str) {
  mSearch_Words.push_back(str);
}
