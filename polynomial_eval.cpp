#include "polynomial_eval.h"
#include <iostream>
#include <chrono>

#if defined(__x86_64__)
#include <immintrin.h>
#define USE_X86_SIMD 1
#elif defined(__aarch64__)
#include <arm_neon.h>
#define USE_ARM_NEON 1
#else
#define USE_SCALAR 1
#endif

double polynomial_eval_sse(double x, const std::vector<double>& coeffs) {
#if USE_X86_SIMD
    // x86-64 optimized path using SSE2
    __m128d result_vec = _mm_setzero_pd();
    __m128d x_vec = _mm_set1_pd(x);
    __m128d power_vec = _mm_set_pd(x, 1.0);  // [x, 1.0]
    __m128d power_mult = _mm_set1_pd(x * x);

    size_t i = 0;

    // Process 2 coefficients at a time
    for (; i + 1 < coeffs.size(); i += 2) {
        __m128d coeff_vec = _mm_set_pd(coeffs[i + 1], coeffs[i]);
        __m128d term = _mm_mul_pd(coeff_vec, power_vec);
        result_vec = _mm_add_pd(result_vec, term);
        power_vec = _mm_mul_pd(power_vec, power_mult);
    }

    // Horizontal add
    double result_arr[2];
    _mm_storeu_pd(result_arr, result_vec);
    double result = result_arr[0] + result_arr[1];

    // Handle remaining coefficient
    if (i < coeffs.size()) {
        double power_arr[2];
        _mm_storeu_pd(power_arr, power_vec);
        result += coeffs[i] * power_arr[0];
    }

    return result;
#elif USE_ARM_NEON
    // ARM64 optimized path using NEON
    float64x2_t result_vec = vdupq_n_f64(0.0);
    float64x2_t x_vec = vdupq_n_f64(x);
    float64x2_t power_vec = {1.0, x};
    float64x2_t power_mult = vdupq_n_f64(x * x);

    size_t i = 0;

    // Process 2 coefficients at a time
    for (; i + 1 < coeffs.size(); i += 2) {
        float64x2_t coeff_vec = {coeffs[i], coeffs[i + 1]};
        float64x2_t term = vmulq_f64(coeff_vec, power_vec);
        result_vec = vaddq_f64(result_vec, term);
        power_vec = vmulq_f64(power_vec, power_mult);
    }

    // Horizontal add
    double result = vgetq_lane_f64(result_vec, 0) + vgetq_lane_f64(result_vec, 1);

    // Handle remaining coefficient
    if (i < coeffs.size()) {
        result += coeffs[i] * vgetq_lane_f64(power_vec, 0);
    }

    return result;
#else
    // Fallback scalar implementation
    double result = 0.0;
    double power = 1.0;
    for (size_t i = 0; i < coeffs.size(); i++) {
        result = result + coeffs[i] * power;
        power = power * x;
    }
    return result;
#endif
}

void benchmark_polynomial() {
    std::cout << "\n=== Polynomial Evaluation Benchmark ===" << std::endl;

    std::vector<double> coeffs = {1.0, 2.5, -3.2, 4.8, -1.5, 2.0, -0.5};
    const int iterations = 10000000;

    auto start = std::chrono::high_resolution_clock::now();
    double sum = 0.0;
    for (int i = 0; i < iterations; i++) {
        sum += polynomial_eval_sse(1.5 + i * 0.0001, coeffs);
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Iterations: " << iterations << std::endl;
    std::cout << "Time: " << duration.count() << " ms" << std::endl;
    std::cout << "Result sum: " << sum << std::endl;
}
