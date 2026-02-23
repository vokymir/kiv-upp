#pragma once

#include <list>
#include <vector>
#include <string>
#include <functional>

/*
 * Trida starajici se o vystup do souboru ze zpracovanych vstupu
 */
class COutput
{
	public:
		// typovy alias lambda funkce pro resolve jmena na zaklade ID
		using FName_Resolver = std::function<const std::string& (size_t)>;

	public:
		COutput() = default;

		// zapisuje vsechny vstupni parametry do souboru
		bool Write_Output(const std::string& gcode, const std::string& drawing, const std::list<size_t> node_ids, const double path_cost, const std::string& path_svg, FName_Resolver resolver);
};
