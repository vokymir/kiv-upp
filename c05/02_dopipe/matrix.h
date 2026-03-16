#pragma once

#include <limits>
#include <vector>

using Matrix = std::vector<std::vector<double>>;

double perform_serial(const Matrix &a, const Matrix &b);
double perform_parallel(const Matrix &a, const Matrix &b);

double lcm(double a, double b);
double gcd(double a, double b);
