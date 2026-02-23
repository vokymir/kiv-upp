#pragma once

#include <vector>
#include <string>
#include "geometry.h"

/*
 * Trida urcena k vykreslovani polygonu (pudorys soucastky) do SVG
 */
class CDrawing
{
	public:
		CDrawing() = default;

		// vygeneruje vykres a ulozi ho do vnitrni promenne
		bool Generate_Drawing(const std::vector<Point>& points);

		// ziska nakreslenou soucastku ve formatu SVG
		std::string Get_SVG() const
		{
			return m_drawing;
		}

	private:
		// ulozene SVG
		std::string m_drawing = "";
};