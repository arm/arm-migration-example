#include "matrix_operations.h"
#include <iostream>
#include <random>
#include <chrono>
#include <stdexcept>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#define USE_X86_SIMD 1
#elif defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#define USE_ARM_NEON 1
#else
#define USE_X86_SIMD 0
#define USE_ARM_NEON 0
#endif

Matrix::Matrix(size_t r, size_t c) : rows(r), cols(c) {
    data.resize(rows, std::vector<double>(cols, 0.0));
}

void Matrix::randomize() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 10.0);

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            data[i][j] = dis(gen);
        }
    }
}

Matrix Matrix::multiply(const Matrix& other) const {
    if (cols != other.rows) {
        throw std::runtime_error("Invalid matrix dimensions for multiplication");
    }

    Matrix result(rows, other.cols);

#if USE_X86_SIMD
    // x86-64 optimized path using SSE2
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < other.cols; j++) {
            __m128d sum_vec = _mm_setzero_pd();
            size_t k = 0;

            // Process 2 elements at a time with SSE2
            for (; k + 1 < cols; k += 2) {
                __m128d a_vec = _mm_loadu_pd(&data[i][k]);
                __m128d b_vec = _mm_set_pd(other.data[k+1][j], other.data[k][j]);
                sum_vec = _mm_add_pd(sum_vec, _mm_mul_pd(a_vec, b_vec));
            }

            // Horizontal add
            double sum_arr[2];
            _mm_storeu_pd(sum_arr, sum_vec);
            double sum = sum_arr[0] + sum_arr[1];

            // Handle remaining element
            if (k < cols) {
                sum += data[i][k] * other.data[k][j];
            }

            result.data[i][j] = sum;
        }
    }
#elif USE_ARM_NEON
    // ARM64 optimized path using NEON
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < other.cols; j++) {
            float64x2_t sum_vec = vdupq_n_f64(0.0);
            size_t k = 0;

            // Process 2 elements at a time with NEON
            for (; k + 1 < cols; k += 2) {
                float64x2_t a_vec = vld1q_f64(&data[i][k]);
                double b_vals[2] = {other.data[k][j], other.data[k+1][j]};
                float64x2_t b_vec = vld1q_f64(b_vals);
                sum_vec = vfmaq_f64(sum_vec, a_vec, b_vec);
            }

            // Horizontal add
            double sum = vgetq_lane_f64(sum_vec, 0) + vgetq_lane_f64(sum_vec, 1);

            // Handle remaining element
            if (k < cols) {
                sum += data[i][k] * other.data[k][j];
            }

            result.data[i][j] = sum;
        }
    }
#else
    // Fallback scalar implementation
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < other.cols; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < cols; k++) {
                sum += data[i][k] * other.data[k][j];
            }
            result.data[i][j] = sum;
        }
    }
#endif

    return result;
}

double Matrix::sum() const {
    double total = 0.0;
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            total += data[i][j];
        }
    }
    return total;
}

void benchmark_matrix_ops() {
    std::cout << "\n=== Matrix Multiplication Benchmark ===" << std::endl;

    const size_t size = 200;
    Matrix a(size, size);
    Matrix b(size, size);

    a.randomize();
    b.randomize();

    auto start = std::chrono::high_resolution_clock::now();
    Matrix c = a.multiply(b);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Matrix size: " << size << "x" << size << std::endl;
    std::cout << "Time: " << duration.count() << " ms" << std::endl;
    std::cout << "Result sum: " << c.sum() << std::endl;
}
