
#include "csv.hpp"
#include <fstream>
namespace chmu {

void write_csv__serial(const std::vector<Station> &stations) {
  std::ofstream file(DEFAULT_EXPORT_CSV_PATH);

  file << "id;month;year;diff\n";

  for (const auto &st : stations) {
    for (const auto &fluct : st.fluctuations_const()) {
      file << st.id() << ";" << fluct.month << ";" << fluct.year << ";"
           << fluct.temp_diff << "\n";
    }
  }
}

} // namespace chmu
