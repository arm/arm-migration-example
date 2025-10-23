#include "hash_operations.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

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

unsigned long long compute_hash(const char* data, size_t len) {
    unsigned long long hash = 5381;
    size_t i = 0;

#if USE_X86_SIMD
    // x86-64 optimized path using SSE2
    for (; i + 16 <= len; i += 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));

        // Extract bytes and update hash - use direct byte access
        const char* chunk_bytes = reinterpret_cast<const char*>(&chunk);
        for (int j = 0; j < 16; j++) {
            unsigned char byte = chunk_bytes[j];
            hash = ((hash << 5) + hash) + byte;
        }
    }
#elif USE_ARM_NEON
    // ARM64 optimized path using NEON
    for (; i + 16 <= len; i += 16) {
        uint8x16_t chunk = vld1q_u8(reinterpret_cast<const uint8_t*>(data + i));
        
        // Process each byte from the NEON vector
        for (int j = 0; j < 16; j++) {
            unsigned char byte = vgetq_lane_u8(chunk, j);
            hash = ((hash << 5) + hash) + byte;
        }
    }
#endif

    // Process remaining bytes (or all bytes on scalar path)
    for (; i < len; i++) {
        hash = ((hash << 5) + hash) + data[i];
    }

    return hash;
}

void benchmark_hashing() {
    std::cout << "\n=== Hashing Benchmark ===" << std::endl;

    const size_t data_size = 10 * 1024 * 1024; // 10 MB
    std::vector<char> data(data_size);

    // Fill with sequential data
    for (size_t i = 0; i < data_size; i++) {
        data[i] = static_cast<char>(i % 256);
    }

    auto start = std::chrono::high_resolution_clock::now();
    unsigned long long hash = compute_hash(data.data(), data_size);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Data size: " << data_size / 1024 << " KB" << std::endl;
    std::cout << "Time: " << duration.count() << " ms" << std::endl;
    std::cout << "Hash: 0x" << std::hex << hash << std::dec << std::endl;
}
