/*
 * High-Performance Compute Benchmark Suite
 * Optimized for x86-64 (SSE2) and AArch64 (NEON) architectures
 */

#include <iostream>
#include "matrix_operations.h"
#include "hash_operations.h"
#include "string_search.h"
#include "memory_operations.h"
#include "polynomial_eval.h"

#ifdef __x86_64__
#define USE_X86_SIMD 1
#else
#define USE_X86_SIMD 0
#endif

#ifdef __aarch64__
#define USE_ARM_NEON 1
#else
#define USE_ARM_NEON 0
#endif

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Compute Benchmark Suite" << std::endl;
#if USE_X86_SIMD
    std::cout << "  x86-64 with SSE2 Optimizations" << std::endl;
#elif USE_ARM_NEON
    std::cout << "  AArch64 with NEON Optimizations" << std::endl;
#else
    std::cout << "  Generic Build (No SIMD)" << std::endl;
#endif
    std::cout << "========================================" << std::endl;

    // Run all benchmarks
    benchmark_matrix_ops();
    benchmark_hashing();
    benchmark_string_ops();
    benchmark_memory_ops();
    benchmark_polynomial();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  All benchmarks completed!" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
