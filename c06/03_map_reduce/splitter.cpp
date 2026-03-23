#include "mapreduce.h"

#include <sstream>

void CSplitter::_Start(IChannel &input, IChannel &output) {
  CInput_Splitter_Channel &in = dynamic_cast<CInput_Splitter_Channel &>(input);
  CSplitter_Mapper_Channel &out =
      dynamic_cast<CSplitter_Mapper_Channel &>(output);

  // dokud je z kanalu co cist...
  std::string batch;
  while (in.get(batch)) {

    // istringstream pro parsovani vstupu
    std::istringstream iss(batch);

    std::string line;
    // dokud cteme radky z istringstreamu...
    while (line = "", std::getline(iss, line)) {
      // a pokud ty radky nejsou prazdne, posleme je dale
      if (!line.empty())
        out.post(std::move(line));
    }
  }

  out.terminate();
}
