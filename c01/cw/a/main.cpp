#include <iostream>

#include <array>
#include <vector>
#include <list>
#include <map>
#include <set>

/*
 * Ziska textovy prepis cifer cisla do retezce
 */
std::string get_description(int cislo)
{
	std::string out = "";

	// caste pouziti mapy - lookup tabulka
	static std::map<int, std::string> cifry = {
		{0, "nula"},
		{1, "jedna"},
		{2, "dva"},
		{3, "tri"},
		{4, "ctyri"},
		{5, "pet"},
		{6, "sest"},
		{7, "sedm"},
		{8, "osm"},
		{9, "devet"},
	};

	while (cislo >= 0)
	{
		if (!out.empty())
			out = " " + out;
		out = cifry[cislo % 10] + out;
		cislo /= 10;

		if (cislo == 0)
			break;
	}

	return out;
}

int main(int argc, char** argv)
{
	// 1) ukazka prace s array - kdyz znam pocet prvku predem

	std::array<int, 8> cisla = { 1, 2, 5, 6, 8, 9, 24, 26 };

	// 2) vector - kdyz pocet prvku znat uplne nemusim, nebo potrebuji haldu

	std::vector<int> vypoctene;
	// jen pro nazornost - kazde cislo vlozime tolikrat, kolik je jeho hodnota
	for (size_t i = 0; i < cisla.size(); i++)
	{
		for (int j = 0; j < cisla[i]; j++)
			vypoctene.push_back(cisla[i]);
	}

	// 3) list - vhodny treba pro frontu ukolu
	std::list<int> prace;
	// vlozime vsechnu praci do fronty
	for (auto cislo : vypoctene)
		prace.push_back(cislo);

	// 4) set - vlozime vsechny vysledky sem
	std::set<int> mnozina_cisel;

	int total_soucet = 0;

	while (!prace.empty())
	{
		auto c = prace.front();
		prace.pop_front();

		total_soucet += c;

		mnozina_cisel.insert(total_soucet % c);
	}

	// 5) vsechna cisla z mnoziny prevedeme na nejaky retezec
	std::map<int, std::string> popis;

	for (auto& c : mnozina_cisel)
		popis[c] = get_description(c);

	// 6) vypis vseho na vystup
	for (const auto& popisky : popis)
		std::cout << popisky.first << " = " << popisky.second << std::endl;

	return 0;
}
