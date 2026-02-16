#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <execution>
#include <numeric>

constexpr size_t NumbersCount = 100'000'000;

double soucet_seriove(const std::vector<double>& numbers) {
	double sum = 0;

	// klasicky soucet cisel
	for (const auto& number : numbers) {
		sum += number;

		// proc tohle neni dobry napad?
		//std::cout << "Mezisoucet: " << sum << std::endl;
	}

	return sum;
}

double soucet_paralelne(const std::vector<double>& numbers) {
	double sum = 0;

	// pro ted zatim prijmete fakt, ze radek nize secte vsechna cisla *nejakym* paralelnim zpusobem
	sum = std::reduce(std::execution::par, numbers.begin(), numbers.end());

	return sum;
}

int main(int argc, char* argv[]) {
	std::random_device rd;
	std::default_random_engine g(rd());
	std::uniform_real_distribution<double> dist(0, 100.0);

	std::cout << "Generuji cisla ... ";

	std::vector<double> numbers;
	numbers.reserve(NumbersCount);
	for (size_t i = 0; i < NumbersCount; ++i) {
		numbers.push_back(dist(g));
	}

	std::cout << "OK" << std::endl;

	// seriovy vypocet a mereni casu
	{
		auto start = std::chrono::high_resolution_clock::now();

		double sum = soucet_seriove(numbers);

		auto duration = std::chrono::high_resolution_clock::now() - start;
		std::cout << "Suma seriove: " << sum << ", zabralo to: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	}

	// paralelni vypocet a mereni casu
	{
		auto tp = std::chrono::high_resolution_clock::now();

		double sum = soucet_paralelne(numbers);

		auto duration = std::chrono::high_resolution_clock::now() - tp;
		std::cout << "Suma paralelne: " << sum << ", zabralo to: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	}
	
	return 0;
}
