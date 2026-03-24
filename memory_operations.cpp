#include "memory_operations.h"
#include <iostream>
#include <vector>
#include <chrono>

#ifdef __aarch64__
#include <arm_neon.h>
#define USE_ARM_NEON 1
#else
#define USE_ARM_NEON 0
#endif

void fast_memcpy(void* dest, const void* src, size_t n) {
    char* d = static_cast<char*>(dest);
    const char* s = static_cast<const char*>(src);
    size_t i = 0;

#if USE_ARM_NEON
    // ARM64 optimized path using NEON
    for (; i + 16 <= n; i += 16) {
        uint8x16_t chunk = vld1q_u8(reinterpret_cast<const uint8_t*>(s + i));
        vst1q_u8(reinterpret_cast<uint8_t*>(d + i), chunk);
    }
#endif

    // Copy remaining bytes (or all on non-ARM)
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
