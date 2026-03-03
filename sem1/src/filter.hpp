#pragma once

#include "stations.hpp"
namespace chmu {

// filter out stations which has
// 1. less than 5 years of continuous data
// 2. reported less than 100 values a year on average
void filter__serial(Stations &stations);

} // namespace chmu
