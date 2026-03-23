#include "mapreduce.h"
#include <chrono>
#include <iostream>

IChannel IChannel::empty;

int main(int argc, char **argv) {
  // input = cteni ze souboru "surove" po blocich, vzdy do konce radky
  // splitter = deleni vstupu na radky
  // mapper = radky rozdeli na slova, vytvori par s jednickou
  // combiner = seskupi slova
  // reducer = secte jednotlive vyskyty
  // output = vypise vystup na konzoli - slovo a pocet jeho vyskytu

  // vytvorime komponenty
  CInput input("genesis_filtered.txt");
  CSplitter splitter;
  CMapper mapper;
  CCombiner combiner;
  CReducer reducer;
  COutput output;

  // vytvorime komunikacni kanaly
  CInput_Splitter_Channel c1;
  CSplitter_Mapper_Channel c2;
  CMapper_Combiner_Channel c3;
  CCombiner_Reducer_Channel c4;
  CReducer_Output_Channel c5;

  /*
   * pro skutecne paralelni potencial map/reduce modelu je treba vybrane
   * komponenty spoustet s nejakou nasobnosti typicky splitter, mapper a reducer
   * by mohli mit vice instanci
   *
   * k tomu je ale potreba dvou uprav:
   *   - CChannel by mel vedet, kolikrat je otevreny pro zapis (a tedy kolik
   * procesu bude volat terminate() - az posledni bude nastavovat mTerminated)
   *   - inicialiacni kod by pochopitelne mel vytvaret vice instanci komponent a
   * nejak je spravovat
   */

  input.Start(IChannel::empty, c1);
  splitter.Start(c1, c2);
  mapper.Start(c2, c3);
  combiner.Start(c3, c4);
  reducer.Start(c4, c5);
  output.Start(c5, IChannel::empty);

  input.Wait();
  splitter.Wait();
  mapper.Wait();
  combiner.Wait();
  reducer.Wait();
  output.Wait();

  return 0;
}
