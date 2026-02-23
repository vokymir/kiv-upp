#include "output.h"

#include <fstream>

bool COutput::Write_Output(const std::string& gcode, const std::string& drawing, const std::list<size_t> node_ids, const double path_cost, const std::string& path_svg, FName_Resolver name_resolver)
{
	// vystup pudorysu do souboru
	std::ofstream svg("out.svg");
	svg << drawing;
	svg.close();

	// vystup GCode do souboru
	std::ofstream print("out.gcode");
	print << gcode;
	print.close();

	// vystup cesty pro distribuci do souboru
	std::ofstream pathsvg("out.path.svg");
	pathsvg << path_svg;
	pathsvg.close();

	// textovy prepis cesty do souboru
	std::ofstream pathtxt("out.path.txt");
	pathtxt << "Total path cost: " << path_cost << std::endl;
	pathtxt << "Nodes: " << std::endl;
	for (auto id : node_ids)
		pathtxt << id << " - " << name_resolver(id) << std::endl;
	pathtxt.close();

	return true;
}
