#include "slicer.h"

#include <sstream>
#include <algorithm>
#include <cmath>

namespace slice
{
	// kolik filamentu vytlacit na jednotku pohybu []
	constexpr double Extrusion_Factor = 0.2;
	// vyska vrstvy [mm]
	constexpr double Layer_Height = 0.3;
	// tloustka cary (prumer trysky) [mm]
	constexpr double Line_Width = 0.4;

	// maximalni velikost po jakekoliv ose
	constexpr double Thing_Max_Size = 100.0;
}

bool CSlicer::Slice(const std::vector<Point>& points)
{
	std::ostringstream oss;

	auto insert_gcode = [&oss](const std::string& str) {
		oss << str << std::endl;
	};

	// nejaky genericky start code
	insert_gcode("G28");
	insert_gcode("G1 Z15 F100");
	insert_gcode("M107");
	insert_gcode("G90");
	insert_gcode("M82");
	insert_gcode("M190 S50");
	insert_gcode("M104 T0 S210");
	insert_gcode("G92 E0");
	insert_gcode("M109 T0 S210");
	insert_gcode(";Layer count: 50");
	insert_gcode(";LAYER:0");
	insert_gcode("M107");
	insert_gcode("G0 F9000 X91.000 Y91.000 Z0.300");

	// nalezeni minima a maxima
	Point ptmin{ points[0].x, points[0].y }, ptmax{ points[0].x, points[0].y };
	for (size_t i = 1; i < points.size(); i++)
	{
		if (points[i].x < ptmin.x)
			ptmin.x = points[i].x;
		if (points[i].x > ptmax.x)
			ptmax.x = points[i].x;
		if (points[i].y < ptmin.y)
			ptmin.y = points[i].y;
		if (points[i].y > ptmax.y)
			ptmax.y = points[i].y;
	}

	// skalovani na maximalni velikost
	const auto mdiff = std::max( ptmax.x - ptmin.x, ptmax.y - ptmin.y );
	const double scaling = slice::Thing_Max_Size / mdiff;

	// transformace bodu
	std::vector<Point> transformed;
	for (auto& pt : points)
		transformed.push_back({ (pt.x - ptmin.x)*scaling, (pt.y - ptmin.y) * scaling });

	Point tmp;

	// naskalujeme minimum a maximum
	ptmax.x = (ptmax.x - ptmin.x) * scaling;
	ptmax.y = (ptmax.y - ptmin.y) * scaling;
	ptmin.x = 0;
	ptmin.y = 0;

	double epos = 0;

	// naslicujeme 5 vrstev
	for (double z = 0; z < 5; z += slice::Layer_Height)
	{
		oss << "G1 F1000 Z" << z << std::endl;

		for (double x = 0; x <= ptmax.x - ptmin.x; x += slice::Line_Width)
		{
			const Point a1{ x, ptmin.y };
			const Point a2{ x, ptmax.y };

			std::vector<Point> intersections;

			for (size_t i = 0; i < transformed.size(); i++)
			{
				const Point& b1 = transformed[i];
				const Point& b2 = transformed[(i + 1) % transformed.size()];

				if (Get_Segment_Intersection(a1, a2, b1, b2, tmp))
					intersections.push_back(tmp);
			}

			std::sort(intersections.begin(), intersections.end(), [](const Point& a, const Point& b) {
				return a.y < b.y;
			});

			if (intersections.empty() || intersections.size() % 2 != 0)
				continue;

			for (size_t i = 0; i < intersections.size(); i += 2)
			{
				const double dist = std::sqrt(std::pow(intersections[i].x - intersections[i + 1].x, 2.0) + std::pow(intersections[i].y - intersections[i + 1].y, 2.0));

				epos += dist;

				oss << "G1 F1000 X" << (intersections[i].x - ptmin.x) << " Y" << (intersections[i].y - ptmin.y) << std::endl;
				oss << "G1 F1000 X" << (intersections[i + 1].x - ptmin.x) << " Y" << (intersections[i + 1].y - ptmin.y) << " E" << epos * slice::Extrusion_Factor << std::endl;
			}
		}
	}

	// nejaky genericky end code
	insert_gcode("G91");
	insert_gcode("T0");
	insert_gcode("G1 E-1");
	insert_gcode("M104 T0 S0");
	insert_gcode("G90");
	insert_gcode("G92 E0");
	insert_gcode("M140 S0");
	insert_gcode("M84");

	m_gcode = oss.str();

	return true;
}
