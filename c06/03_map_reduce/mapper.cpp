#include "mapreduce.h"

#include <cstddef>
#include <sstream>

void CMapper::_Start(IChannel &input, IChannel &output) {
  CSplitter_Mapper_Channel &in =
      dynamic_cast<CSplitter_Mapper_Channel &>(input);
  CMapper_Combiner_Channel &out =
      dynamic_cast<CMapper_Combiner_Channel &>(output);

  // dokud je co cist ze vstupniho kanalu
  std::string line;
  size_t index = 0;

  while (in.get(line)) {

    // istringstream pro parsovani vstupni radky na slova
    std::istringstream iss(line);

    std::string word;
    // parsujeme po slovech
    while (word = "", std::getline(iss, word, ' ')) {
      // pokud slovo neni prazdne, posleme ho dal
      if (!word.empty()) {
        out.post({word, index++});
      }
    }
  }

  out.terminate();
}
