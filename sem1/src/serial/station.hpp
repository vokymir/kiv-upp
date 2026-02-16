
#include "measurement.hpp"
#include <string>
#include <vector>
namespace serial {

class Station {
  int id_;
  std::string name_;
  float lat_, lon_;
  std::vector<Measurement> measurements_;
};

} // namespace serial
