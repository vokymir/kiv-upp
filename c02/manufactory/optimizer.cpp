#include "optimizer.h"

#include <cmath>
#include <random>
#include <limits>
#include <tuple>
#include <iostream>

#include <algorithm>
#include <numeric>

namespace opt
{
	// velikost populace pro optimalizaci
	constexpr size_t Population_Size = 1000;
	// pocet generaci pro optimalizaci
	constexpr size_t Generations_Count = 10000;

	// pravdepodobnost krizeni s horsim clenem populace
	constexpr double Crossover_Chance = 0.2;
	// pravdepodobnost uplneho nahrazeni clena populace
	constexpr double Refresh_Chance_Low = 0.4;
	// pravdepodobnost mutace kterehokoliv jineho clena populace
	constexpr double Mutation_Chance = 0.05;

	// maximalni rozdil dvou car tak, aby melo cenu resit, zda maji prusecik
	// tohle je dobre omezit, aby se rovnou zamezilo praci s kandidatnimi resenimi, ktera nemaji sanci byt dobra
	constexpr double Limit_Angle_Diff = 0.2;
}

std::pair<bool, Point> COptimizer::Get_Repulsor_Line_Intersection(const Repulsor& r1, const Repulsor& r2)
{
	// rovnobezky nebo skoro-rovnobezky - takove se neprotinaji vubec nebo az nekde hooodne daleko
	if (std::fabs(r1.angle - r2.angle) < opt::Limit_Angle_Diff)
		return { false, {0,0} };

	// vychozi body
	const Point& as = r1.origin;
	const Point& bs = r2.origin;
	// smerove vektory
	const Point ad = { std::cos(r1.angle), std::sin(r1.angle) };
	const Point bd = { std::cos(r2.angle), std::sin(r2.angle) };

	return { true, Get_Line_Intersection(as, bs, ad, bd)};
}

std::vector<Point> COptimizer::Extract_Points(const Member& member)
{
	std::vector<Point> pts(member.repulsors.size());

	bool success;
	// vygeneruje body protnutim primek vedenych ze vsech dvojic po sobe jdoucich repulzoru
	for (size_t i = 0; i < member.repulsors.size(); i++)
	{
		std::tie(success, pts[i]) = Get_Repulsor_Line_Intersection(member.repulsors[i], member.repulsors[(i + 1) % member.repulsors.size()]);
		if (!success)
			return {};
	}

	return pts;
}

void COptimizer::Calculate_Fitness(Member& member)
{
	// extrahuje body ze clena populace
	auto pts = Extract_Points(member);
	if (pts.empty())
	{
		member.fitness = std::numeric_limits<double>::max();
		return;
	}

	// check jestli se strany polygonu neprotinaji (kdyby polygon sam sebe protinal, nesel by vyrobit)
	for (size_t i = 0; i < pts.size(); i++)
	{
		// pro kazdou dvojici po sobe jdoucich bodu (krajnich bodu usecky strany polygonu)
		const Point& a1 = pts[i];
		const Point& a2 = pts[(i + 1) % pts.size()];

		// vypocteme prunik s kazdou useckou reprezentujici stranu polygonu
		for (size_t j = 0; j < pts.size(); j++)
		{
			// vyjma se sebou samym a sousednimi stranami - s temi ma jeden dobre definovany prunik a ten nas nezajima, s tim pocitame
			if (i == j || j == (i+1)%pts.size() || i == (j+1)%pts.size())
				continue;

			const Point& b1 = pts[j];
			const Point& b2 = pts[(j + 1) % pts.size()];

			if (Has_Segment_Intersection(a1, a2, b1, b2))
			{
				member.fitness = std::numeric_limits<double>::max();
				return;
			}
		}
	}

	// vynulujeme fitness kazdeho clena, budeme k ni pricitat
	member.fitness = 0;

	double tmpfit;
	Point origin{ 0, 0 };

	for (size_t i = 0; i < pts.size(); i++)
	{
		// maximalizujeme vzdalenost od pocatku
		tmpfit = 1.0 / Get_Distance(pts[i], origin);

		// minimalizujeme vzdalenost bodu od sebe
		for (size_t j = 0; j < pts.size(); j++)
		{
			if (i == j || j == (i + 1) % pts.size() || i == (j + 1) % pts.size())
				continue;

			tmpfit *= Get_Distance(pts[i], pts[j]);
		}

		// zapocitame do celkove metriky clena populace
		member.fitness += tmpfit;
	}
}

bool COptimizer::Optimize(const std::vector<double>& repulsors)
{
	std::vector<Member> population(opt::Population_Size);

	// vynegerovani bodu repulzoru - jejich pozice jsou pro vsechny prvky stejne
	auto& init = population[0].repulsors;
	init.resize(repulsors.size());
	double angleStep = 2*Pi / static_cast<double>(repulsors.size());
	for (size_t i = 0; i < init.size(); i++)
	{
		// repulzory se nachazi na kruznicich pod fixnimi uhly
		init[i].origin.x = repulsors[i] * std::cos(angleStep * static_cast<double>(i));
		init[i].origin.y = repulsors[i] * std::sin(angleStep * static_cast<double>(i));
	}
	// zkopirujeme zakladni repulzorovou sadu do cele populace
	for (size_t i = 1; i < population.size(); i++)
		population[i].repulsors = population[0].repulsors;

	// vygenerujeme nahodne uhly vsech repulzoru
	std::random_device rdev;
	std::default_random_engine reng(rdev());
	std::uniform_real_distribution<double> gen_angle(0, Pi);

	for (auto& member : population)
	{
		for (auto& rep : member.repulsors)
			rep.angle = gen_angle(reng);
	}

	// rozdeleni pro pravdepodobnost mutace/regenerace
	std::uniform_real_distribution<double> chance(0, 1.0);
	// rozdeleni pro krizeni a bod krizeni
	std::uniform_int_distribution<size_t> dist_crossover(5, population.size() - 1);
	std::uniform_int_distribution<size_t> dist_crossover_point(0, repulsors.size() - 1);

	auto calc_population_fitness = [&]() {
		// prepocitat metriky cele populace
		for (auto& mem : population)
			Calculate_Fitness(mem);

		// seradit - na zacatku bude vzdy prvek s nejlepsi metrikou
		std::sort(population.begin(), population.end(), [](const Member& m1, const Member& m2) {
			return m1.fitness < m2.fitness;
		});
	};

	calc_population_fitness();

	// hlavni optimalizacni smycka
	size_t current_gen = 0;
	while (++current_gen < opt::Generations_Count)
	{
		// TODO: prakticky je lepsi udelat prechod mezi generacemi na dve faze - vygenerovani nove populace ze stare a nahrazeni stare populace novou
		//       takto se muzeme pripravit o dobra reseni, ale to je nam pro takto vymysleny problem uplne jedno

		// krizeni nejlepsich 5 se zbytkem populace
		for (size_t i = 0; i < 5 && i < population.size(); i++)
		{
			if (chance(reng) < opt::Crossover_Chance)
			{
				// vygenerovat bod krizeni
				const size_t crossover_point = dist_crossover_point(reng);
				// 50% sance na leve i na prave krizeni
				if (chance(reng) < 0.5)
				{
					for (size_t j = crossover_point; j < repulsors.size(); j++)
						population[dist_crossover(reng)].repulsors[j].angle = population[i].repulsors[j].angle;
				}
				else
				{
					for (size_t j = 0; j < crossover_point; j++)
						population[dist_crossover(reng)].repulsors[j].angle = population[i].repulsors[j].angle;
				}
			}
		}

		// pregenerovat nejhorsi
		for (auto& rep : population[population.size() - 1].repulsors)
			rep.angle = gen_angle(reng);

		// pregenerovat cast nejhorsich
		for (size_t i = population.size() - 5; i < population.size() - 1; i++)
		{
			for (auto& rep : population[i].repulsors)
			{
				if (chance(reng) < opt::Refresh_Chance_Low)
					rep.angle = gen_angle(reng);
			}
		}

		// mutace ve zbytku populace
		for (size_t i = 1; i < population.size() - 5; i++)
		{
			for (auto& rep : population[i].repulsors)
			{
				if (chance(reng) < opt::Mutation_Chance)
					rep.angle = gen_angle(reng);
			}
		}

		// prepocitat metriky
		calc_population_fitness();
	}

	// z nejlepsiho vytvorime vysledek (polygon)
	m_points = Extract_Points(*population.begin());

	return !m_points.empty();
}
