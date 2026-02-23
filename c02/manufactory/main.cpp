#include <cstddef>
#include <iostream>

#include "drawing.h"
#include "input.h"
#include "optimizer.h"
#include "output.h"
#include "router.h"
#include "slicer.h"

#include <chrono>
#include <vector>

using measurement_clock = std::chrono::steady_clock;
using time_point = measurement_clock::time_point;

time_point start_measurement() { return measurement_clock::now(); }

long end_measurement(const std::string &task_name, const time_point &start) {
  auto diff = measurement_clock::now() - start;
  long time =
      std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
  std::cout << "Uloha " << task_name << ": " << time << "ms" << std::endl;
  return time;
}

int main(int argc, char **argv) {
  CInput input;
  CRouter router;
  COptimizer optimizer;
  CSlicer slicer;
  CDrawing drawer;
  COutput output;

  time_point tp;
  time_point total_tp;

  if (!router.Load_Graph("graph.txt")) {
    std::cerr << "Nelze nacist graf stredisek (graph.txt)." << std::endl;
    return 1;
  }
  constexpr size_t Number_Repeats = 10;
  constexpr size_t Number_Tasks = 8; // A-H
  std::vector<std::vector<long>> repeats;
  repeats.resize(Number_Tasks, std::vector<long>(Number_Repeats, 0));

  // A) prijeti a validace pozadavku
  while (input.Accept_Request()) {

    for (auto rep = 0; rep < Number_Repeats; rep++) {
      std::cout << "A JEDEME, cislo: " << rep + 1 << "/" << Number_Repeats
                << std::endl;

      total_tp = start_measurement();

      tp = start_measurement();

      // B) vygenerovani "soucastky" (gen. algoritmus)
      if (!optimizer.Optimize(input.Get_Diameters())) {
        std::cerr << "Nelze optimalizovat." << std::endl;
        continue;
      }

      repeats[1][rep] = end_measurement("B", tp);
      tp = start_measurement();

      // C) slicing pro vyrobu na 3D tiskarne
      if (!slicer.Slice(optimizer.Get_Optimized_Points())) {
        std::cerr << "Nelze vygenerovat GCode." << std::endl;
        continue;
      }

      repeats[2][rep] = end_measurement("C", tp);
      tp = start_measurement();

      // D) vykresleni pudorysu do souboru (SVG)
      if (!drawer.Generate_Drawing(optimizer.Get_Optimized_Points())) {
        std::cerr << "Nelze vykreslit pudorys." << std::endl;
        continue;
      }

      repeats[3][rep] = end_measurement("D", tp);
      tp = start_measurement();

      // E) pokus o zlepseni poradi uzlu stredisek k expedici
      std::list<size_t> nodes = input.Get_Target_Nodes();
      if (router.Optimize_Node_Order(nodes, 1))
        nodes = router.Get_Optimized_Node_Order();

      repeats[4][rep] = end_measurement("E", tp);
      tp = start_measurement();

      // F) vygenerovani cesty pres N uzlu (stredisek)
      if (!router.Generate_Path(nodes, 1)) {
        std::cerr << "Nelze vygenerovat cestu." << std::endl;
        continue;
      }

      repeats[5][rep] = end_measurement("F", tp);
      tp = start_measurement();

      // G) vykresleni mapy, kudy dodavka pojede
      if (!router.Draw_Path()) {
        std::cerr << "Nelze vykreslit mapu cestu." << std::endl;
        continue;
      }

      repeats[6][rep] = end_measurement("G", tp);
      tp = start_measurement();

      // H) expedice (finalizace reportu do souboru)
      if (!output.Write_Output(slicer.Get_GCode(), drawer.Get_SVG(),
                               router.Get_Path(), router.Get_Total_Cost(),
                               router.Get_Path_Drawing(),
                               [&router](size_t id) -> const std::string & {
                                 return router.Get_Node_Name(id);
                               })) {
        std::cerr << "Nelze zapsat vysledky." << std::endl;
        continue;
      }

      repeats[7][rep] = end_measurement("H", tp);
      end_measurement("Total", total_tp);
    }

    std::cout << "Pozadavek byl zpracovan." << std::endl;

    std::cout << "Number of runs: " << Number_Repeats << std::endl;
    int i = 0;
    for (const auto &task_times : repeats) {
      long sum = 0;
      for (const auto &task_time : task_times) {
        sum += task_time;
      }
      double avg = static_cast<double>(sum) / Number_Repeats;

      std::cout << (char)(65 + i) << " avg: " << avg << std::endl;
      ++i;
    }
  }

  return 0;
}
