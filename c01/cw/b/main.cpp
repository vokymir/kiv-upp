#include <iostream>

#include <thread>
#include <chrono>
#include <algorithm>
#include <vector>
#include <future>

// nejaka relativne neuzitecna funkce, ktera vygeneruje pole cisel, seradi ho a vrati nahodny prvek z nej
int merena_funkce_1()
{
	std::vector<int> numbers(1000000);

	auto f = []() -> int { return rand() % 10000 - 5000; };

	std::generate(numbers.begin(), numbers.end(), f);
	std::sort(numbers.begin(), numbers.end());

	return numbers[rand() % numbers.size()];
}

// nejaka relativne neuzitecna funkce, ktera vygeneruje pole cisel, seradi ho a vrati nahodny prvek z nej
int merena_funkce_2()
{
	std::vector<int> numbers(1000000);

	auto f = []() -> int { return rand() % 100000 - 50000; };

	std::generate(numbers.begin(), numbers.end(), f);
	std::sort(numbers.begin(), numbers.end());

	return numbers[rand() % numbers.size()];
}

int main(int argc, char** argv)
{
	int result1, result2, result;
	std::chrono::steady_clock::time_point start, end;
	long long time_ms;
	srand(static_cast<unsigned int>(time(nullptr)));

	/*
	 * Ukol pro vas: pocitat zrychleni z jednoho behu neni "fer" - na systemu s preemptivnim planovacem, na kterem navic bezi spousty
	 * nejen systemovych sluzeb a jinych aplikaci, jsou casy ruzne. Provedte totez mereni pro kazdou z metod vicekrat a vypoctete
	 * urychleni 2. a 3. zpusobu vuci 1. Pokuste se zduvodnit namerene/vypoctene hodnoty.
	 */

	//// Mereni 1 - bez paralelizace
	{
		start = std::chrono::steady_clock::now();

		result1 = merena_funkce_1();
		result2 = merena_funkce_2();
		result = result1 + result2;

		end = std::chrono::steady_clock::now();
		time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		std::cout << "(1) Result: " << result << " in " << time_ms << "ms" << std::endl;
	}

	//// Mereni 2 - thread + join
	{
		start = std::chrono::steady_clock::now();

		std::thread calc_thread([&result1]() {
			result1 = merena_funkce_1();
		});
		result2 = merena_funkce_2();

		if (calc_thread.joinable())
			calc_thread.join();

		result = result1 + result2;

		end = std::chrono::steady_clock::now();
		time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		std::cout << "(2) Result: " << result << " in " << time_ms << "ms" << std::endl;
	}

	//// Mereni 3 - async
	{
		start = std::chrono::steady_clock::now();

		auto res1_future = std::async(std::launch::async, &merena_funkce_1);
		result2 = merena_funkce_2();

		result1 = res1_future.get();

		result = result1 + result2;

		end = std::chrono::steady_clock::now();
		time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		std::cout << "(3) Result: " << result << " in " << time_ms << "ms" << std::endl;
	}

	return 0;
}
