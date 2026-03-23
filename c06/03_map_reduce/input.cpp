#include "mapreduce.h"

constexpr size_t ByteCountMin = 10000;

CInput::CInput(const std::string &filename) : mInput(filename) {
  if (!mInput.is_open()) {
    throw std::invalid_argument{"Nelze najit vstupni soubor"};
  }
}

void CInput::_Start(IChannel & /*input*/, IChannel &output) {

  CInput_Splitter_Channel &out =
      dynamic_cast<CInput_Splitter_Channel &>(output);

  // dokud je co cist...
  while (mInput) {

    std::string batch;
    batch.resize(ByteCountMin);

    // precteme zakladni naloz dat
    mInput.read(const_cast<char *>(batch.data()), ByteCountMin);

    batch.resize(mInput.gcount());

    // a cteme jeste dal do momentu, nez narazime na konec radky
    while (mInput && *batch.rbegin() != '\n') {
      char c;
      mInput.read(&c, 1);
      batch.push_back(c);
    }

    // odesleme davku dal
    out.post(std::move(batch));
  }

  out.terminate();
}
