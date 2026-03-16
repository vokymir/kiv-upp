#pragma once

#include <list>
#include <vector>

#include "work.h"
#include "worker.h"

// trida implementujici farmera (a zaroven arbitra prace)
class CWord_Farmer {
private:
  // pracovni kanal
  CWork_Channel mChannel;

  // vektor workeru, vytvareji se a zanikaji v metode Run
  std::vector<CWord_Worker> mWorkers;

  // zdrojovy soubor slov
  std::string mSource_File;

  // limit delky slova (abychom nemeli moc permutaci)
  size_t mWord_Len_Limit = 6;

  // seznam hledanych slov
  std::list<std::string> mSearch_Words;

public:
  // spusti farmer proces a samotny vypocet; tato metoda vytvari, spravuje a
  // ukoncuje workery
  void Run();

  // nastavi zdrojovy soubor slov
  void Set_Source_File(const std::string &filename);

  // nastavi limit velikosti slova
  void Set_Word_Length_Limit(size_t s);

  // prida slovo do seznamu vyhledavanych
  void Add_Search_Word(const std::string &str);
};
