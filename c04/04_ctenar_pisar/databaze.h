#pragma once

#include <vector>

constexpr size_t Velikost_Databaze = 100'000;

inline constexpr long long Spravny_Soucet() {
	long long sum = 0;
	for (size_t i = 0; i < Velikost_Databaze; i++) {
		sum += i;
	}
	return sum;
}

class Database {
	private:
		std::vector<long long> mValues;

	public:
		Database();

		long long read(int index) const;

		void write(int index, long long value);
};
