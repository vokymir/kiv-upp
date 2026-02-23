#pragma once

#include <vector>
#include <string>

#include "geometry.h"
#include "optimizer.h"

/*
 * Trida obstaravajici primitivni slicing do GCode pro 3D tiskarnu
 */
class CSlicer
{
	public:
		CSlicer() = default;

		// vygeneruje GCode pro zadany polygon
		bool Slice(const std::vector<Point>& points);

		// ziska vygenerovany GCode
		std::string Get_GCode() const
		{
			return m_gcode;
		}

	private:
		// vygenerovany GCode
		std::string m_gcode;
};
