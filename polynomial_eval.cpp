#include "polynomial_eval.h"
#include <iostream>
#include <chrono>

#ifdef __aarch64__
#include <arm_neon.h>
#define USE_ARM_NEON 1
#else
#define USE_ARM_NEON 0
#endif

double polynomial_eval_sse(double x, const std::vector<double>& coeffs) {
#if USE_ARM_NEON
    // ARM64 optimized path using NEON
    float64x2_t result_vec = vdupq_n_f64(0.0);
    double init[2] = {1.0, x};
    float64x2_t power_vec = vld1q_f64(init);
    float64x2_t power_mult = vdupq_n_f64(x * x);

    size_t i = 0;

    // Process 2 coefficients at a time
    for (; i + 1 < coeffs.size(); i += 2) {
        float64x2_t coeff_vec = vld1q_f64(&coeffs[i]);
        result_vec = vaddq_f64(result_vec, vmulq_f64(coeff_vec, power_vec));
        power_vec = vmulq_f64(power_vec, power_mult);
    }

    // Horizontal add
    double result = vaddvq_f64(result_vec);

    // Handle remaining coefficient
    if (i < coeffs.size()) {
        double power_arr[2];
        vst1q_f64(power_arr, power_vec);
        result += coeffs[i] * power_arr[0];
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
