#pragma once

#include "station.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>
namespace chmu {

// store all stations in vector - continuous memory
// add mapping to O(1) for id access
struct Stations {
  // all stations here
  std::vector<Station> stations;
  // if you know station ID, here you can get the IDX the station has in
  // stations vector
  std::unordered_map<size_t, size_t> mapping;

  // if you know station ID, get its index in the stations vector
  std::optional<size_t> index(size_t id) const {
    auto it = mapping.find(id);
    if (it != mapping.end()) {
      return it->second;
    }
    return std::nullopt;
  }

  // get reference to station by its ID if it exists
  std::optional<std::reference_wrapper<Station>> get_station(size_t id) {
    if (auto idx = index(id)) {
      return stations[*idx];
    }
    return std::nullopt;
  }

  // add station to both stations and mapping
  template <typename... Args> void add_station(Args &&...args) {
    size_t idx = stations.size();
    stations.emplace_back(std::forward<Args>(args)...);
    size_t id = stations.back().id();
    mapping[id] = idx;
  }

  // delete station from mapping - to really remove it, you must call cleanup.
  // this is O(1), cleanup is O(N)
  void del_station(size_t id) { mapping.erase(id); }

  void cleanup() {
    // really remove all not *existing* stations
    stations.erase(std::remove_if(stations.begin(), stations.end(),
                                  [this](const Station &s) {
                                    return !mapping.contains(s.id());
                                  }),
                   stations.end());

    // rebuild mapping
    mapping.clear();
    for (size_t i = 0; i < stations.size(); ++i) {
      mapping[stations[i].id()] = i;
    }
  }
};

} // namespace chmu
