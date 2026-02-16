#include <chrono>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

constexpr uint64_t Number_Of_Numbers = 100'000'000;
constexpr uint64_t Number_Of_Runs = 10;

// Collatz conjecture
// https://en.wikipedia.org/wiki/Collatz_conjecture
//
// Tohle je pro nas jen priklad problemu, ktery nema predem znamy pocet kroku a
// tedy muze trvat kratkou i dlouhou dobu.
uint64_t collatz(uint64_t n) {
  uint64_t steps = 0;
  while (n != 1) {

    if (n % 2 == 0) {
      n /= 2;
    } else {
      n = 3 * n + 1;
    }
    steps++;
  }

  return steps;
}

void pre_cout(const std::string_view &title) {
  int line_length = 8 + 4 * 12 + 4 * 3;

  std::cout << std::string(line_length, '-') << "\n"
            << std::format("{:^{}}", title, line_length) << "\n"
            << std::string(line_length, '-') << "\n"

            << std::format("{:<8} | {:>12} | {:>12} | {:>12} | {:>12}", "index",
                           "max steps", "number", "avg step", "time")
            << "\n"

            << std::string(line_length, '-') << std::endl;
}

double post_cout(const std::vector<long> &run_times) {
  long sum = 0;
  for (auto n : run_times) {
    sum += n;
  }
  double avg = sum / static_cast<double>(Number_Of_Runs);
  std::cout << "\n======> Prumerna doba vypoctu: " << avg << "ms\n"
            << std::endl;

  return avg;
}

long do_the_thing(int run_number) {

  std::vector<uint64_t> stepCount{};
  stepCount.reserve(Number_Of_Numbers);

  uint64_t max = 0;
  uint64_t sum = 0;
  uint64_t maxIndex = 0;

  // TIMER START NOW
  auto tp = std::chrono::high_resolution_clock::now();

  // only one for-loop
  for (uint64_t i = 0; i < Number_Of_Numbers; i++) {
    stepCount.push_back(collatz(i + 1));

    if (stepCount[i] > max) {
      max = stepCount[i];
      maxIndex = i;
    }
    sum += stepCount[i];
  }

  // TIMER END NOW
  auto dur = std::chrono::high_resolution_clock::now() - tp;
  auto elapsed_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

  std::cout << std::format("{:<8} | {:>12} | {:>12} | {:>12} | {:>12}",
                           run_number, max, maxIndex + 1,
                           sum / stepCount.size(), elapsed_ms)
            << std::endl;

  return elapsed_ms;
}

long do_the_thing_parallel(int run_number) {

  std::vector<uint64_t> step_count{};
  step_count.resize(Number_Of_Numbers);

  uint64_t max = 0;
  uint64_t sum = 0;
  uint64_t max_index = 0;

  // TIMER START NOW
  auto tp = std::chrono::high_resolution_clock::now();

  constexpr size_t Number_Of_Workers = 8;

  // distribute the work in equal chunks
  std::vector<std::thread> workers{};
  size_t a, b = 0;
  for (int i = 0; i < Number_Of_Workers; i++) {
    a = b;
    b = i * Number_Of_Numbers / Number_Of_Workers;

    workers.emplace_back([&step_count, a, b]() {
      for (size_t i = a; i < b; i++) {
        step_count[i] = collatz(i + 1);
      }
    });
  }

  // wait on workers
  while (workers.size() > 0) {
    if (workers.back().joinable()) {
      workers.back().join();
      workers.pop_back();
    }
  }

  for (uint64_t i = 0; i < Number_Of_Numbers; ++i) {
    if (step_count[i] > max) {
      max = step_count[i];
      max_index = i;
    }
    sum += step_count[i];
  }

  // TIMER END NOW
  auto dur = std::chrono::high_resolution_clock::now() - tp;
  auto elapsed_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

  std::cout << std::format("{:<8} | {:>12} | {:>12} | {:>12} | {:>12}",
                           run_number, max, max_index + 1,
                           sum / step_count.size(), elapsed_ms)
            << std::endl;

  return elapsed_ms;
}

int main(int argc, char **argv) {

  // Vas ukol:
  // 1) implementujte mereni casu pro cely vypocet
  // 2) zmerte, jak dlouho u vas trva vypocet seriove, tj. ve stavu, ve kterem
  // je ted. 3) pokuste se alespon drobne optimalizovat vypocet tak, aby trval
  // co nejmene casu i kdyz je seriovy 4) pokuste se vypocet paralelizovat a
  // zmerit, jak dlouho trva vypocet v paralelnim rezimu 5) spocitejte urychleni
  // (viz prednasky)

  /* PROSTOR PRO VYSLEDKY:
   * - seriova verze:   23437.3ms
   * - paralelni verze:  4966.3ms
   * - urychleni:       471.927 %
   */

  // NOTE: samozrejme, ze na to existuji efektivni zpusoby; dnes je ale cilem,
  // abyste si vyzkouseli paralelizovat "naivne", tedy tak, jak si myslite, ze
  // by to mohlo jit s vyuzitim prostredku, ktere znate. V lepsim pripade
  // zjistite, ze jste to mysleli spravne.
  //
  // Nekteri z Vas, kteri jiz maji zkusenosti s paralelnim programovanim budou
  // zde urcite ve vyhode. Pokuste se na reseni prijit doopravdy sami. Pokud
  // budete mit pocit, ze jste se zasekli, nebojte se zeptat. Ale zkuste to
  // nejdrive sami. V kazdem pripade i vy zkusenejsi, zkuste pouzit jen to, co
  // doted zname (tj. std::thread, join, ...)

  // Hinty:
  // - nehledejte moznost optimalizace ve funkci collatz, nebo alespon ne hned
  // zkraje
  // - pouzijte std::thread pro vytvoreni vlaken; vytvorte staticky pocet
  // vlaken, idealne tolik, kolik mate CPU jader
  // - zmerit cas vypoctu muzete pomoci std::chrono::high_resolution_clock a
  // zpusobu, jaky jsme si ukazovali na prvnim cviceni
  // - alokace a dealokace pameti trva nejakou dobu; cim vetsi mnozstvi pameti,
  // tim delsi je prace s ni (zalezi, ale zpravidla to tak je)
  // - vzpomente si, ze na ukonceni vlakna muzete cekat pomoci metody join()

  // VAROVANI: vystup po optimalizaci a paralelizaci musi byt identicky! Tj.
  // porad chcete zjistit, ktere cislo ma nejvetsi pocet kroku a jaky je
  // prumerny pocet kroku. jedinou dovolenou odchylkou ve vystupu je cislo, pro
  // ktere byl zaznamenan maximalni pocet kroku (ale pocet kroku musi byt
  // stejny)

  std::vector<long> run_times{};
  run_times.reserve(Number_Of_Runs);

  pre_cout("PARALLEL");
  for (int run = 0; run < Number_Of_Runs; ++run) {
    run_times.push_back(do_the_thing_parallel(run));
  }
  auto parallel_avg = post_cout(run_times);

  run_times.clear();
  run_times.reserve(Number_Of_Runs);

  pre_cout("SERIAL");
  for (int run = 0; run < Number_Of_Runs; ++run) {
    run_times.push_back(do_the_thing(run));
  }
  auto serial_avg = post_cout(run_times);

  std::cout << "<=====\n ~~~> Urychleni = " << serial_avg / parallel_avg * 100
            << " % <~~~\n=====>" << std::endl;

  return 0;
}
