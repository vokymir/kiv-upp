#pragma once

#include "model/model.hpp"
#include <vector>
namespace chmu::filter {

namespace serial {

// filter out stations which has (a || b)
// a) less than 5 years of continuous data
// b) reported less than 100 values a year on average
void work(std::vector<Station> &stations);

} // namespace serial

namespace parallel {

// filter stations, uses standard library parallel functions for parallel
// execution
void work(std::vector<Station> &stations);

} // namespace parallel

} // namespace chmu::filter
