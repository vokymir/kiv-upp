#pragma once

#include <limits>
#include <vector>

using Matrix = std::vector<std::vector<double>>;

double perform_serial(const Matrix &a, const Matrix &b);
double perform_parallel_static(const Matrix &a, const Matrix &b);
double perform_parallel_dynamic(const Matrix &a, const Matrix &b);
