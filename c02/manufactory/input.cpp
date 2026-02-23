#include "input.h"

#include <iostream>
#include <string>
#include <random>

bool CInput::Accept_Request()
{
	std::cout << "Cekam na vstup. Zadejte 'input' pro rucni zadani nebo 'random' pro nahodne. Pozor, nahodne zadani nemusi byt uspesne - optimalizace muze skoncit chybou." << std::endl;
	std::cout << ":> ";

	std::string cmd;

	while (std::cin >> cmd)
	{
		// ukonceni cteni
		if (cmd == "exit")
			return false;

		// rucni zadani
		if (cmd == "input")
		{
			std::cout << "Zadej pocet hran obrazce (3-50): ";

			if (!(std::cin >> m_lineCount))
				continue;

			if (m_lineCount < 3 || m_lineCount > 50)
			{
				std::cout << "Hran musi byt 3 - 50" << std::endl;
				continue;
			}

			std::cout << "Zadej vahy repulzoru (10-50)." << std::endl;

			m_diameters.clear();
			double dbl;
			for (size_t i = 0; i < m_lineCount; i++)
			{
				std::cout << (i+1) << ": ";
				std::cin >> dbl;

				m_diameters.push_back(dbl);
			}

			std::cout << "Zadej pocet stredisek k expedici (min. 3, hodnoty 2-10000): ";

			size_t expedCnt = 0;

			if (!(std::cin >> expedCnt))
				continue;

			if (expedCnt < 3)
			{
				std::cout << "Strediska musi byt alespon 3" << std::endl;
				continue;
			}

			size_t tmp;

			m_targetNodes.clear();
			for (size_t i = 0; i < expedCnt; i++)
			{
				std::cout << (i + 1) << ": ";
				std::cin >> tmp;

				m_targetNodes.push_back(tmp);
			}

			return true;
		}
		// staly zakaznik cislo 1
		else if (cmd == "c1") {
			std::cout << "Nastaveni pro staleho zakaznika cislo 1." << std::endl;
			m_lineCount = 15;
			m_diameters = { 8.19897, 3.93871, 5.288, 9.77493, 5.7534, 9.33614, 3.43609, 1.83335, 8.9514, 4.69257, 5.48636, 3.97047, 7.77966, 7.24328, 6.2867 };
			m_targetNodes = { 7333, 6543, 3835, 4965, 4586, 2878, 5468, 5302, 1432, 3367 };
			return true;
		}
		// staly zakaznik cislo 2
		else if (cmd == "c2") {
			std::cout << "Nastaveni pro staleho zakaznika cislo 2." << std::endl;
			m_lineCount = 16;
			m_diameters = { 7.02753, 6.77681, 6.12348, 2.4253, 6.61087, 1.09456, 8.8596, 2.63013, 8.4831, 5.40757, 9.25254, 9.87618, 1.75919, 1.29982, 7.60446, 7.69856 };
			m_targetNodes = { 6445, 8068, 5699, 8044, 51, 4772, 4892, 12, 3589, 2416, 8475, 9950, 1280, 7480, 1258, 9300, 1472, 1980, 189 };
			return true;
		}
		// nahodne generovani vstupu v nejakych mezich - POZOR! Muze vygenerovat kombinace, ktere nemusi byt realne, a tedy nekdy muze optimalizace skoncit chybou
		else if (cmd == "random")
		{
			static std::random_device rdev;
			std::default_random_engine reng(rdev());

			std::uniform_real_distribution<double> dist_diameter(0.5, 10.0);
			std::uniform_int_distribution<size_t> dist_diameter_count(10, 50);
			std::uniform_int_distribution<size_t> dist_node_count(3, 10);
			std::uniform_int_distribution<size_t> dist_node_ids(2, 10000);

			m_lineCount = dist_diameter_count(reng);
			m_diameters.clear();
			for (size_t i = 0; i < m_lineCount; i++)
				m_diameters.push_back(dist_diameter(reng));

			// vypsat vahy
			std::cout << "Vygenerovane vahy: ";
			for (auto& d : m_diameters) {
				std::cout << d << " ";
			}
			std::cout << std::endl;

			size_t cnt = dist_node_count(reng);
			m_targetNodes.clear();
			for (size_t i = 0; i < cnt; i++) {
				m_targetNodes.push_back(dist_node_ids(reng));
			}

			// vypsat strediska
			std::cout << "Vygenerovana strediska: ";
			for (auto& d : m_targetNodes) {
				std::cout << d << " ";
			}
			std::cout << std::endl;

			return true;
		}
	}

	return false;
}
