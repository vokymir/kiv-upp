#include <iostream>

#include "farmer.h"

int main(int argc, char **argv) {
  // vytvorime farmer objekt, nastavime mu vstupni soubor, limity, hledana
  // podslova
  CWord_Farmer farmer;

  farmer.Set_Source_File("./03_dynamic/words.txt");

  farmer.Set_Word_Length_Limit(14);

  farmer.Add_Search_Word("para");
  farmer.Add_Search_Word("spin");
  farmer.Add_Search_Word("pist");
  farmer.Add_Search_Word("play");
  farmer.Add_Search_Word("party");

  // a vse spustime - z volani Run se rizeni vraci po dokonceni ukolu nebo po
  // vzniku chyby
  farmer.Run();

  return 0;
}
