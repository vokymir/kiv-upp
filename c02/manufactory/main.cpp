#include <iostream>

#include "router.h"
#include "input.h"
#include "optimizer.h"
#include "slicer.h"
#include "drawing.h"
#include "output.h"

#include <chrono>

using measurement_clock = std::chrono::steady_clock;
using time_point = measurement_clock::time_point;

time_point start_measurement()
{
	return measurement_clock::now();
}

void end_measurement(const std::string& task_name, const time_point& start)
{
	auto diff = measurement_clock::now() - start;
	std::cout << "Uloha " << task_name << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << "ms" << std::endl;
}

int main(int argc, char** argv)
{
	CInput input;
	CRouter router;
	COptimizer optimizer;
	CSlicer slicer;
	CDrawing drawer;
	COutput output;

	time_point tp;
	time_point total_tp;

	if (!router.Load_Graph("graph.txt")) {
		std::cerr << "Nelze nacist graf stredisek (graph.txt)." << std::endl;
		return 1;
	}
	
	// A) prijeti a validace pozadavku
	while (input.Accept_Request())
	{
		total_tp = start_measurement();

		tp = start_measurement();

		// B) vygenerovani "soucastky" (gen. algoritmus)
		if (!optimizer.Optimize(input.Get_Diameters()))
		{
			std::cerr << "Nelze optimalizovat." << std::endl;
			continue;
		}

		end_measurement("B", tp);
		tp = start_measurement();

		// C) slicing pro vyrobu na 3D tiskarne
		if (!slicer.Slice(optimizer.Get_Optimized_Points()))
		{
			std::cerr << "Nelze vygenerovat GCode." << std::endl;
			continue;
		}

		end_measurement("C", tp);
		tp = start_measurement();

		// D) vykresleni pudorysu do souboru (SVG)
		if (!drawer.Generate_Drawing(optimizer.Get_Optimized_Points()))
		{
			std::cerr << "Nelze vykreslit pudorys." << std::endl;
			continue;
		}

		end_measurement("D", tp);
		tp = start_measurement();

		// E) pokus o zlepseni poradi uzlu stredisek k expedici
		std::list<size_t> nodes = input.Get_Target_Nodes();
		if (router.Optimize_Node_Order(nodes, 1))
			nodes = router.Get_Optimized_Node_Order();

		end_measurement("E", tp);
		tp = start_measurement();

		// F) vygenerovani cesty pres N uzlu (stredisek)
		if (!router.Generate_Path(nodes, 1))
		{
			std::cerr << "Nelze vygenerovat cestu." << std::endl;
			continue;
		}

		end_measurement("F", tp);
		tp = start_measurement();

		// G) vykresleni mapy, kudy dodavka pojede
		if (!router.Draw_Path())
		{
			std::cerr << "Nelze vykreslit mapu cestu." << std::endl;
			continue;
		}

		end_measurement("G", tp);
		tp = start_measurement();

		// H) expedice (finalizace reportu do souboru)
		if (!output.Write_Output(slicer.Get_GCode(), drawer.Get_SVG(), router.Get_Path(), router.Get_Total_Cost(), router.Get_Path_Drawing(), [&router](size_t id) -> const std::string& { return router.Get_Node_Name(id); }))
		{
			std::cerr << "Nelze zapsat vysledky." << std::endl;
			continue;
		}

		end_measurement("H", tp);
		end_measurement("Total", total_tp);

		std::cout << "Pozadavek byl zpracovan." << std::endl;
	}

	return 0;
}
