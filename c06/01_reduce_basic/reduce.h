#pragma once

#include <vector>

// plne seriova verze
double sum_serial(const std::vector<double> &numbers);

// paralelni verze s atomickou sdilenou promennou
double sum_parallel_atomic(const std::vector<double> &numbers);

// paralelni verze s nezavislou promennou pro mezisoucty
double sum_parallel_independent(const std::vector<double> &numbers);
