#include "drawing.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

namespace draw
{
	// pomer, jak moc je nakres vetsi, nez realne rozmery
	constexpr double Scaling = 100.0;
}

bool CDrawing::Generate_Drawing(const std::vector<Point>& points)
{
	std::ostringstream oss;

	// najdeme minimum a maximum, transformujeme vsechny body faktorem skalovani
	Point ptmin{ points[0].x * draw::Scaling,points[0].y * draw::Scaling }, ptmax{ points[0].x * draw::Scaling,points[0].y * draw::Scaling };
	std::vector<Point> transformed;
	for (auto& pt : points)
	{
		transformed.push_back({
			pt.x * draw::Scaling,
			pt.y * draw::Scaling
		});

		const Point& pt2 = *transformed.rbegin();
		if (pt2.x > ptmax.x)
			ptmax.x = pt2.x;
		if (pt2.x < ptmin.x)
			ptmin.x = pt2.x;
		if (pt2.y > ptmax.y)
			ptmax.y = pt2.y;
		if (pt2.y < ptmin.y)
			ptmin.y = pt2.y;
	}

	// posuneme vsechny body do noveho pocatku, kterym je minimum
	for (auto& pt : transformed)
	{
		pt.x += -ptmin.x;
		pt.y += -ptmin.y;
	}

	// hlavicka SVG
	oss << "<svg width=\"" << (ptmax.x - ptmin.x) << "\" height=\"" << (ptmax.y - ptmin.y) << "\" xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;

	// samotny polygon
	oss << "<polygon points=\"";

	for (auto& pt : transformed)
		oss << pt.x << "," << pt.y << " ";
	
	oss << "\" style=\"fill:yellow; stroke:black; stroke-width:1\" />";


	// krokovani mrizky po ose X a Y
	const double stepping_x = ((ptmax.x - ptmin.x) / 20.0) / draw::Scaling;
	const double stepping_y = ((ptmax.y - ptmin.y) / 20.0) / draw::Scaling;
	// vybereme to mensi z obou pro obe osy
	const double stepping = std::min(stepping_x, stepping_y);

	Point tmp;

	// svisle cary mrizky
	oss << "<g>" << std::endl;
	for (double x = ptmin.x / draw::Scaling; x <= ptmax.x / draw::Scaling; x += stepping)
	{
		const Point a1{ x, ptmin.y / draw::Scaling };
		const Point a2{ x, ptmax.y / draw::Scaling };

		oss << "<line x1=\"" << (a1.x * draw::Scaling - ptmin.x) << "\" y1=\"" << (a1.y * draw::Scaling - ptmin.y) << "\" "
		          << "x2=\"" << (a2.x * draw::Scaling - ptmin.x) << "\" y2=\"" << (a2.y * draw::Scaling - ptmin.y) << "\" stroke=\"red\" />" << std::endl;

		std::vector<Point> intersections;

		for (size_t i = 0; i < points.size(); i++)
		{
			const Point& b1 = points[i];
			const Point& b2 = points[(i + 1) % points.size()];

			oss << "<line x1=\"" << (b1.x * draw::Scaling - ptmin.x) << "\" y1=\"" << (b1.y * draw::Scaling - ptmin.y) << "\" "
			          << "x2=\"" << (b2.x * draw::Scaling - ptmin.x) << "\" y2=\"" << (b2.y * draw::Scaling - ptmin.y) << "\" stroke=\"green\" />" << std::endl;

			// vypocteme prunik cary s kazdou stranou polygonu, pokud existuje
			if (Get_Segment_Intersection(a1, a2, b1, b2, tmp))
				intersections.push_back(tmp);
		}

		// pro kazdy prusecik vykreslime kruh
		for (auto& pt : intersections)
			oss << "<circle cx=\"" << (pt.x*draw::Scaling - ptmin.x) << "\" cy=\"" << (pt.y * draw::Scaling - ptmin.y) << "\" r=\"15\" />" << std::endl;
	}
	oss << "</g>" << std::endl;

	// vodorovne cary mrizky
	oss << "<g>" << std::endl;
	for (double y = ptmin.y / draw::Scaling; y <= ptmax.y / draw::Scaling; y += stepping)
	{
		const Point a1{ ptmin.x / draw::Scaling, y };
		const Point a2{ ptmax.x / draw::Scaling, y };

		oss << "<line x1=\"" << (a1.x * draw::Scaling - ptmin.x) << "\" y1=\"" << (a1.y * draw::Scaling - ptmin.y) << "\" "
		          << "x2=\"" << (a2.x * draw::Scaling - ptmin.x) << "\" y2=\"" << (a2.y * draw::Scaling - ptmin.y) << "\" stroke=\"red\" />" << std::endl;

		std::vector<Point> intersections;

		for (size_t i = 0; i < points.size(); i++)
		{
			const Point& b1 = points[i];
			const Point& b2 = points[(i + 1) % points.size()];

			oss << "<line x1=\"" << (b1.x * draw::Scaling - ptmin.x) << "\" y1=\"" << (b1.y * draw::Scaling - ptmin.y) << "\" "
			          << "x2=\"" << (b2.x * draw::Scaling - ptmin.x) << "\" y2=\"" << (b2.y * draw::Scaling - ptmin.y) << "\" stroke=\"green\" />" << std::endl;

			if (Get_Segment_Intersection(a1, a2, b1, b2, tmp))
				intersections.push_back(tmp);
		}

		for (auto& pt : intersections)
			oss << "<circle cx=\"" << (pt.x * draw::Scaling - ptmin.x) << "\" cy=\"" << (pt.y * draw::Scaling - ptmin.y) << "\" r=\"15\" />" << std::endl;
	}
	oss << "</g>" << std::endl;

	// konec SVG
	oss << "</svg>" << std::endl;

	// preklopeni ze streamu do retezce
	m_drawing = oss.str();

	return true;
}
