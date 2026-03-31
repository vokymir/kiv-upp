#pragma once

#include "model/model.hpp"
#include <vector>
namespace chmu {

// filter out stations which has (a || b)
// a) less than 5 years of continuous data
// b) reported less than 100 values a year on average
void filter__serial(std::vector<Station> &stations);

} // namespace chmu
