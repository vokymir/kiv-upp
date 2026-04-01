#pragma once

#include "model/model.hpp"
#include <string>
#include <vector>
namespace chmu::draw {

struct Temperature_Range {
  float min;
  float max;
};

struct Point {
  float x;
  float y;
  std::string color_code;

  // put on the correct x and y
  void put(float lat, float lon);
  // fill with correct color
  void color(float temperature, const Temperature_Range &range);

  // construct all
  Point(float lat, float lon, float temp, const Temperature_Range &range) {
    put(lat, lon);
    color(temp, range);
  }
};

// get the global min & max from all measurements
Temperature_Range get_temperature_range(const std::vector<Station> &stations);

namespace serial {

// draw 12 svgs, one for each month
void work(const std::vector<Station> &stations);

// prepare all points which should be drawn to the svg
std::vector<Point> prepare_month(const std::vector<Station> &stations,
                                 int month_1_indexed,
                                 const Temperature_Range &range);

// write all points into svg format
void write_month(const std::vector<Point> &points, int month_1_indexed);

} // namespace serial

} // namespace chmu::draw
