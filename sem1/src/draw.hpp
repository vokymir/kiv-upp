#pragma once

#include "stations.hpp"
#include <string>
#include <vector>
namespace chmu {

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

// draw 12 svgs, one for each month
void draw_svg__serial(const Stations &stations);

// prepare all points which should be drawn to the svg
std::vector<Point> prepare_svg_month__serial(const Stations &stations,
                                             int month_1_indexed,
                                             const Temperature_Range &range);

// write all points into svg format
void write_svg_month__serial(const std::vector<Point> &points,
                             int month_1_indexed);

// get the global min & max from all measurements
Temperature_Range get_temperature_range(const Stations &stations);
} // namespace chmu
