#include "geometry.h"

#include <cmath>

double Get_Distance(const Point& a, const Point& b)
{
	return std::sqrt(std::pow(a.x - b.x, 2.0) + std::pow(a.y - b.y, 2.0));
}

bool Get_Segment_Intersection(const Point& as, const Point& a2, const Point& bs, const Point& b2, Point& result)
{
	const Point ad = { a2.x - as.x, a2.y - as.y };
	const Point bd = { b2.x - bs.x, b2.y - bs.y };

	auto dx = bs.x - as.x;
	auto dy = bs.y - as.y;
	auto det = bd.x * ad.y - bd.y * ad.x;
	if (std::fabs(det) > 0.0001)
	{
		double u = (dy * bd.x - dx * bd.y) / det;
		double v = (dy * ad.x - dx * ad.y) / det;
		if (u >= 0 && v >= 0 && u <= 1.0 && v <= 1.0)
		{
			result.x = as.x + ad.x * u;
			result.y = as.y + ad.y * u;
			return true;
		}
	}
	return false;
}

bool Has_Segment_Intersection(const Point& as, const Point& a2, const Point& bs, const Point& b2)
{
	const Point ad = { a2.x - as.x, a2.y - as.y };
	const Point bd = { b2.x - bs.x, b2.y - bs.y };

	auto dx = bs.x - as.x;
	auto dy = bs.y - as.y;
	auto det = bd.x * ad.y - bd.y * ad.x;
	if (std::fabs(det) > 0.0001)
	{
		double u = (dy * bd.x - dx * bd.y) / det;
		double v = (dy * ad.x - dx * ad.y) / det;
		if (u >= 0 && v >= 0 && u <= 1.0 && v <= 1.0)
			return true;
	}
	return false;
}

Point Get_Line_Intersection(const Point& as, const Point& bs, const Point& ad, const Point& bd)
{
	auto u = (as.y * bd.x + bd.y * bs.x - bs.y * bd.x - bd.y * as.x) / (ad.x * bd.y - ad.y * bd.x);

	return Point{ as.x + ad.x * u, as.y + ad.y * u };
}
