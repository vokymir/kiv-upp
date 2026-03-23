#include "mapreduce.h"

#include <iostream>

void COutput::_Start(IChannel &input, IChannel & /*output*/) {
  CReducer_Output_Channel &in = dynamic_cast<CReducer_Output_Channel &>(input);

  // dokud nam reducer posila vstupy, budeme je vypisovat
  std::pair<std::string, size_t> item;
  while (in.get(item)) {
    std::cout << item.first << " - " << item.second << "." << std::endl;
  }
}
