#include <print>
#include <string_view>

using namespace std::literals;

int main(int argc, char **argv) {
  bool parallel = false;

  for (int i = 1; i < argc; ++i) {
    if (argv[i] == "--parallel"sv || argv[i] == "-p"sv) {
      parallel = true;
    }
  }

  std::print("{}", parallel ? "Is parallel" : "Is serial");

  return 0;
}
