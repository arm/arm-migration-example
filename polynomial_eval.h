#ifndef POLYNOMIAL_EVAL_H
#define POLYNOMIAL_EVAL_H

#include <vector>

// Vectorized polynomial evaluation using x86 SSE2
double polynomial_eval_sse(double x, const std::vector<double>& coeffs);

// Benchmark function
void benchmark_polynomial();

#endif // POLYNOMIAL_EVAL_H
