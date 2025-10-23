#include "memory_operations.h"
#include <iostream>
#include <vector>
#include <chrono>

#ifdef __x86_64__
#include <immintrin.h>
#define USE_X86_SIMD 1
#else
#define USE_X86_SIMD 0
#endif

void fast_memcpy(void* dest, const void* src, size_t n) {
    char* d = static_cast<char*>(dest);
    const char* s = static_cast<const char*>(src);
    size_t i = 0;

#if USE_X86_SIMD
    // x86-64 optimized path using SSE2
    for (; i + 16 <= n; i += 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s + i));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(d + i), chunk);
    }
#endif

    // Copy remaining bytes (or all on non-x86)
    for (; i < n; i++) {
        d[i] = s[i];
    }
}

void benchmark_memory_ops() {
    std::cout << "\n=== Memory Operations Benchmark ===" << std::endl;

    const size_t size = 50 * 1024 * 1024; // 50 MB
    std::vector<char> src(size, 'A');
    std::vector<char> dest(size);

    auto start = std::chrono::high_resolution_clock::now();
    fast_memcpy(dest.data(), src.data(), size);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Memory size: " << size / 1024 / 1024 << " MB" << std::endl;
    std::cout << "Time: " << duration.count() << " ms" << std::endl;
    std::cout << "Throughput: " << (size / 1024.0 / 1024.0) / (duration.count() / 1000.0)
              << " MB/s" << std::endl;
}
