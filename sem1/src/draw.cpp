
#include "draw.hpp"
#include "model/model.hpp"
#include <algorithm>
#include <array>
#include <execution>
#include <format>
#include <fstream>
#include <iterator>
#include <ranges>
#include <string>
#include <vector>
namespace chmu::draw {

// =====
// POINT

void Point::put(float lat, float lon) {
  constexpr float lat_max = 51.03806105663445f;  // upper left
  constexpr float lat_min = 48.521003814763994f; // bottom right
  constexpr float lon_max = 18.866923511078615f; // bottom right
  constexpr float lon_min = 12.10220905426906f;  // upper left

  constexpr float width = 1412; // hardcoded from SVG (viewBox)
  constexpr float height = 809; // also

  x = (lon - lon_min) / (lon_max - lon_min) * width;
  y = (lat_max - lat) / (lat_max - lat_min) * height;
}

void Point::color(float temperature, const Temperature_Range &range) {
  float t = (temperature - range.min) / (range.max - range.min);
  t = std::clamp(t, 0.0f, 1.0f);

  int red, green, blue;

  if (t <= 0.5f) {
    red = static_cast<int>(t * 2 * 255);
    green = static_cast<int>(t * 2 * 255);
    blue = 255;
  } else {
    red = 255;
    green = static_cast<int>((1.0f - t) * 2 * 255);
    blue = 0;
  }

  color_code = "rgb(" + std::to_string(red) + "," + std::to_string(green) +
               "," + std::to_string(blue) + ")";
}

// /POINT
// =====

constexpr const std::array<std::string, 12> month_names = {
    "leden",    "unor",  "brezen", "duben", "kveten",   "cerven",
    "cervenec", "srpen", "zari",   "rijen", "listopad", "prosinec"};

Temperature_Range get_temperature_range(const std::vector<Station> &stations) {
  float min_val = std::numeric_limits<float>::infinity();
  float max_val = -std::numeric_limits<float>::infinity();

  for (const auto &st : stations) {
    const auto &s = st.stats_const();

    min_val = std::min(min_val, s.global_min_);
    max_val = std::max(max_val, s.global_max_);
  }

  return {min_val, max_val};
}

namespace serial {

void work(const std::vector<Station> &stations) {
  auto range = get_temperature_range(stations);

  for (int month = 1; month <= 12; ++month) {
    auto pts = prepare_month(stations, month, range);
    write_month(std::move(pts), month);
  }
}

std::vector<Point> prepare_month(const std::vector<Station> &stations,
                                 int month_1_indexed,
                                 const Temperature_Range &range) {
  int month = month_1_indexed - 1;
  std::vector<Point> points;

  // prepare all station points to draw
  for (const auto &st : stations) {
    points.emplace_back(st.lat(), st.lon(),
                        st.stats_const().monthly_mean_[month], range);
  }

  return points;
}

void write_month(const std::vector<Point> &points, int month_1_indexed) {
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

} // namespace serial

namespace parallel {

void work(const std::vector<Station> &stations) {
  const auto range = get_temperature_range(stations);

  // iterate over all 12 months (1-indexed)
  const auto months = std::views::iota(1, 13);

  std::for_each(std::execution::par, months.begin(), months.end(),
                [&range, &stations](int month) {
                  auto pts = serial::prepare_month(stations, month, range);
                  serial::write_month(std::move(pts), month);
                });
}

} // namespace parallel

} // namespace chmu::draw
