#pragma once

#include <vector>
#include <limits>

#include "geometry.h"

/*
 * Trida obstaravajici optimalizaci soucastky dle pozadavku
 */
class COptimizer
{
	private:
		// repulzor - interni struktura optimizeru - jde o bod, ktery byl zadany z vnejsku obohaceny o veci k optimalizaci (uhel)
		struct Repulsor
		{
			// kde repulzor je?
			Point origin;
			// uhel primky vyslane z tohoto bodu
			double angle;
		};

		// clen populace (N repulzoru podle poctu zadanych vstupu)
		struct Member
		{
			// seznam repulzoru (vektor parametru)
			std::vector<Repulsor> repulsors;
			// metrika daneho clena populace
			double fitness = std::numeric_limits<double>::max();
		};

	public:
		COptimizer() = default;

		// optimalizuje soucastku dle zadanych kriterii
		bool Optimize(const std::vector<double>& repulsors);

		// vraci body polygonu soucastky
		const std::vector<Point>& Get_Optimized_Points() const
		{
			return m_points;
		}

	protected:
		std::pair<bool, Point> Get_Repulsor_Line_Intersection(const Repulsor& r1, const Repulsor& r2);
		std::vector<Point> Extract_Points(const Member& member);
		void Calculate_Fitness(Member& member);

	private:
		// vektor bodu polygonu soucastky
		std::vector<Point> m_points;
};
