#include "mapreduce.h"
#include <algorithm>
#include <cstddef>

void CReducer::_Start(IChannel &input, IChannel &output) {
  CCombiner_Reducer_Channel &in =
      dynamic_cast<CCombiner_Reducer_Channel &>(input);
  CReducer_Output_Channel &out =
      dynamic_cast<CReducer_Output_Channel &>(output);

  // dokud je na vstupu seznam vyskytu slova...
  std::pair<std::string, std::list<size_t>> item;
  while (in.get(item)) {

    // najdeme nejvetsi index
    size_t max_idx = *std::max_element(item.second.begin(), item.second.end());

    // a odesleme to na vystup
    out.post({item.first, max_idx});
  }

  out.terminate();
}
