#include "mapreduce.h"

void CReducer::_Start(IChannel &input, IChannel &output) {
  CCombiner_Reducer_Channel &in =
      dynamic_cast<CCombiner_Reducer_Channel &>(input);
  CReducer_Output_Channel &out =
      dynamic_cast<CReducer_Output_Channel &>(output);

  // dokud je na vstupu seznam vyskytu slova...
  std::pair<std::string, std::list<size_t>> item;
  while (in.get(item)) {

    // redukujeme pocty do jednoho souctu
    size_t total = 0;
    for (auto &t : item.second) {
      total += t;
    }

    // a odesleme to na vystup
    out.post({item.first, total});
  }

  out.terminate();
}
