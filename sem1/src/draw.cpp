
#include "draw.hpp"
#include "station.hpp"
#include "stations.hpp"
#include <algorithm>
#include <array>
#include <format>
#include <fstream>
#include <iterator>
#include <ranges>
#include <string>
#include <vector>
namespace chmu {

constexpr const std::array<std::string, 12> month_names = {
    "leden",    "unor",  "brezen", "duben", "kveten",   "cerven",
    "cervenec", "srpen", "zari",   "rijen", "listopad", "prosinec"};

void draw_svg__serial(const Stations &stations) {
  auto range = get_temperature_range(stations);

  for (int month = 1; month <= 12; ++month) {
    auto pts = prepare_svg_month__serial(stations, month, range);
    write_svg_month__serial(std::move(pts), month);
  }
}

std::vector<Point> prepare_svg_month__serial(const Stations &stations,
                                             int month_1_indexed,
                                             const Temperature_Range &range) {
  int month = month_1_indexed - 1;
  std::vector<Point> points;

  // prepare all station points to draw
  for (const auto &st : stations.stations) {
    points.emplace_back(st.lat(), st.lon(), st.averages_by_month_const()[month],
                        range);
  }

  return points;
}

void write_svg_month__serial(const std::vector<Point> &points,
                             int month_1_indexed) {
  int month = month_1_indexed - 1;

  std::string svg;
  { // load svg "template"
    std::ifstream file("./src/czmap.svg");
    svg = std::string(std::istreambuf_iterator<char>(file),
                      std::istreambuf_iterator<char>());
  }

  auto pos = svg.find("</svg>");

  // write to file
  std::ofstream file(month_names[month] + ".svg");
  file << svg.substr(0, pos);

  for (const auto &p : points) {
    file << std::format("<circle cx=\"{}\" cy=\"{}\" r=\"5\" fill=\"{}\"/>\n",
                        p.x, p.y, p.color_code);
  }

  file << svg.substr(pos);
}

Temperature_Range get_temperature_range(const Stations &stations) {
  // get all values across all stations
  auto all_values =
      stations.stations |
      std::views::transform([](const Station &st) -> const auto & {
        return st.measurements_const();
      }) |
      std::views::join;

  // get the lowest and highest temperature measurement across all stations
  auto [min, max] = std::ranges::minmax_element(
      all_values, [](const auto &mes1, const auto &mes2) {
        return mes1.value() < mes2.value();
      });

  return {min->value(), max->value()};
}

void Point::put(float lat, float lon) {
  constexpr float lat_max = 51.03806105663445f;  // upper left
  constexpr float lat_min = 48.521003814763994f; // bottom right
  constexpr float lon_max = 18.866923511078615f; // bottom right
  constexpr float lon_min = 12.10220905426906f;  // upper left

  constexpr float width = 5338;  // hardcoded from SVG
  constexpr float height = 3056; // also

  x = (lon - lon_min) / (lon_max - lon_min) * width;
  y = (lat_max - lat) / (lat_max - lat_min) * height;
}

void Point::color(float temperature, const Temperature_Range &range) {
  float t = (temperature - range.min) / (range.max - range.min);
  int red, green, blue;

  if (t < 0.5f) {
    // from blue to yellow
    float brightness = t * 2;

    red = (255 * brightness); // these two together are yellow
    green = (255 * brightness);
    blue = (255 * (1 - brightness)); // from blue
  } else {
    // from yellow to red
    float brightness = (t - 0.5) * 2;

    red = 255;                        // the same in both Y and R
    green = (255 * (1 - brightness)); // from Y to R
    blue = 0;                         // the same
  }

  color_code = "rgb(" + std::to_string(red) + "," + std::to_string(green) +
               "," + std::to_string(blue) + ")";
}

} // namespace chmu
