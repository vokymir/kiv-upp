#pragma once

// inzenyrsky dostatecne presna hodnota Ludolfova cisla
constexpr double Pi = 3.14;

/*
 * Struktura ulozeneho bodu
 */
struct Point
{
	double x;
	double y;
};

// vypocte euklidovskou vzdalenost dvou bodu
double Get_Distance(const Point& a, const Point& b);

// zjisti, zda se zadane useky protinaji
bool Has_Segment_Intersection(const Point& as, const Point& a2, const Point& bs, const Point& b2);

// zjisti, zda se zadane useky protinaji a pokud ano, vygeneruje strukturu bodu jejich pruseciku
bool Get_Segment_Intersection(const Point& as, const Point& a2, const Point& bs, const Point& b2, Point& result);

// vypocte prunik dvou primek
Point Get_Line_Intersection(const Point& as, const Point& bs, const Point& ad, const Point& bd);
