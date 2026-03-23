#include "mapreduce.h"

#include <list>
#include <map>

void CCombiner::_Start(IChannel &input, IChannel &output) {

  CMapper_Combiner_Channel &in =
      dynamic_cast<CMapper_Combiner_Channel &>(input);
  CCombiner_Reducer_Channel &out =
      dynamic_cast<CCombiner_Reducer_Channel &>(output);

  std::map<std::string, std::list<size_t>> combiner;

  // dokud je co cist...
  std::pair<std::string, size_t> token;
  while (in.get(token)) {
    // zarazujeme postupne slova do mapy
    combiner[token.first].push_back(token.second);
  }

  // tady je videt, ze combiner vyzaduje kompletni vstup, nez zacne posilat
  // cokoliv na vystup; jde tedy o potencialni bottleneck

  // na konci celou mapu presypeme na vystup
  for (auto &c : combiner) {
    out.post({c.first, std::move(c.second)});
  }

  out.terminate();
}
