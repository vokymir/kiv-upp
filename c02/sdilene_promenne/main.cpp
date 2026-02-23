#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <algorithm>

constexpr int Pocet_Cisel = 1'000'000'000; // NOTE: toto zpusobi alokaci cca 4GB pameti

long long soucet_seriove(const std::vector<int>& cisla)
{
	long long soucet = 0;
	for (int i = 0; i < cisla.size(); i++) {
		soucet += cisla[i];
	}
	return soucet;
}

long long soucet_paralelne(const std::vector<int>& cisla)
{
	// POZOR: tato verze scitani neni spravna, protoze se zde pracuje se sdilenou promennou
	//        a timto zpusobem se do ni potencialne zapisuje z vice vlaken zaroven = vysledek nejspis nebude spravny

	// navic: v teto verzi je scitani daleko pomalejsi - proc?

	// sdilena redukcni promenna
	long long soucet = 0;

	std::thread t1([&cisla, &soucet]() {
		for (int i = 0; i < cisla.size() / 2; i++) {
			soucet += cisla[i];
		}
	});

	std::thread t2([&cisla, &soucet]() {
		for (int i = cisla.size() / 2; i < cisla.size(); i++) {
			soucet += cisla[i];
		}
	});

	t1.join();
	t2.join();

	return soucet;
}

int minimum_seriove(const std::vector<int>& cisla)
{
	int minimum = cisla[0];
	for (int i = 1; i < cisla.size(); i++) {
		if (cisla[i] < minimum) {
			minimum = cisla[i];
		}
	}
	return minimum;
}

int minimum_paralelne(const std::vector<int>& cisla)
{
	// POZOR: tato verze hledani minima neni spravna, protoze se zde pracuje se sdilenou promennou
	//        a timto zpusobem se do ni potencialne zapisuje z vice vlaken zaroven = vysledek nemusi byt spravny
	// 
	// ALE... v tomto pripade se to stane jen zridka kdy (pokud vubec na nasem HW a s nasimi cisly),
	//        protoze se do promenne zapisuje jen pokud je hodnota mensi nez aktualni minimum

	// sdilena uzamykatelna promenna
	int minimum = cisla[0];

	std::thread t1([&cisla, &minimum]() {
		for (int i = 1; i < cisla.size() / 2; i++) {
			if (cisla[i] < minimum) {
				minimum = cisla[i];
			}
		}
	});

	std::thread t2([&cisla, &minimum]() {
		for (int i = cisla.size() / 2; i < cisla.size(); i++) {
			if (cisla[i] < minimum) {
				minimum = cisla[i];
			}
		}
	});

	t1.join();
	t2.join();

	return minimum;
}

int main(int argc, char** argv)
{
	std::vector<int> cisla;
	cisla.reserve(Pocet_Cisel);

	std::cout << "Generuji cisla..." << std::endl;

	std::mt19937 g(1000); // aby cisla byla pseudonahodna, ale vzdy stejna (fixni seed = 1000)
	std::uniform_int_distribution<int> d(10, 100000000);
	for (int i = 0; i < Pocet_Cisel; i++) {
		cisla.push_back(d(g));
	}

	std::cout << "Scitam seriove..." << std::endl;
	{
		auto start = std::chrono::steady_clock::now();
		long long soucet = soucet_seriove(cisla);
		auto end = std::chrono::steady_clock::now();
		std::cout << "Seriove: " << soucet << " (" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms)" << std::endl;
	}

	std::cout << "Scitam paralelne..." << std::endl;
	{
		auto start = std::chrono::steady_clock::now();
		long long soucet = soucet_paralelne(cisla);
		auto end = std::chrono::steady_clock::now();
		std::cout << "Paralelne: " << soucet << " (" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms)" << std::endl;
	}

	std::cout << "Hledam minimum seriove..." << std::endl;
	{
		auto start = std::chrono::steady_clock::now();
		int minimum = minimum_seriove(cisla);
		auto end = std::chrono::steady_clock::now();
		std::cout << "Seriove: " << minimum << " (" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms)" << std::endl;
	}

	std::cout << "Hledam minimum paralelne..." << std::endl;
	{
		auto start = std::chrono::steady_clock::now();
		int minimum = minimum_paralelne(cisla);
		auto end = std::chrono::steady_clock::now();
		std::cout << "Paralelne: " << minimum << " (" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms)" << std::endl;
	}

	return 0;
}
