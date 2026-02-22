#include "string_search.h"
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

int simd_string_search(const std::string& text, const std::string& pattern) {
    int count = 0;
    size_t text_len = text.length();
    size_t pattern_len = pattern.length();

    if (pattern_len == 0 || pattern_len > text_len) {
        return 0;
    }

    const char first_char = pattern[0];
    size_t i = 0;

#if USE_X86_SIMD
    // x86-64 optimized path using SSE2
    __m128i first_char_vec = _mm_set1_epi8(first_char);

    for (; i + 16 <= text_len - pattern_len + 1; i += 16) {
        __m128i text_chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(text.data() + i));
        __m128i cmp = _mm_cmpeq_epi8(text_chunk, first_char_vec);
        int mask = _mm_movemask_epi8(cmp);

        // Check each potential match
        for (int bit = 0; bit < 16 && i + bit <= text_len - pattern_len; bit++) {
            if (mask & (1 << bit)) {
                bool match = true;
                for (size_t j = 1; j < pattern_len; j++) {
                    if (text[i + bit + j] != pattern[j]) {
                        match = false;
                        break;
                    }
                }
                if (match) count++;
            }
        }
    }
#elif USE_ARM_NEON
    // ARM64 optimized path using NEON
    uint8x16_t first_char_vec = vdupq_n_u8(first_char);
    const uint8_t bit_mask_data[16] = {1, 2, 4, 8, 16, 32, 64, 128, 1, 2, 4, 8, 16, 32, 64, 128};
    uint8x16_t bit_mask = vld1q_u8(bit_mask_data);

    for (; i + 16 <= text_len - pattern_len + 1; i += 16) {
        uint8x16_t text_chunk = vld1q_u8(reinterpret_cast<const uint8_t*>(text.data() + i));
        uint8x16_t cmp = vceqq_u8(text_chunk, first_char_vec);
        
        // Emulate _mm_movemask_epi8
        uint8x16_t masked = vandq_u8(cmp, bit_mask);
        uint8x8_t res = vpadd_u8(vget_low_u8(masked), vget_high_u8(masked));
        res = vpadd_u8(res, res);
        res = vpadd_u8(res, res);
        uint16_t mask = vget_lane_u16(vreinterpret_u16_u8(res), 0);

        if (mask) {
            // Check each potential match
            for (int bit = 0; bit < 16 && i + bit <= text_len - pattern_len; bit++) {
                if (mask & (1 << bit)) {
                    bool match = true;
                    for (size_t j = 1; j < pattern_len; j++) {
                        if (text[i + bit + j] != pattern[j]) {
                            match = false;
                            break;
                        }
                    }
                    if (match) count++;
                }
            }
        }
    }
#endif

    // Handle remaining characters (or all on non-x86)
    for (; i <= text_len - pattern_len; i++) {
        bool match = true;
        for (size_t j = 0; j < pattern_len; j++) {
            if (text[i + j] != pattern[j]) {
                match = false;
                break;
            }
        }
        if (match) count++;
    }

    return count;
}

void benchmark_string_ops() {
    std::cout << "\n=== String Search Benchmark ===" << std::endl;

    // Create a large text
    std::string text;
    for (int i = 0; i < 100000; i++) {
        text += "The quick brown fox jumps over the lazy dog. ";
    }

    std::string pattern = "fox";

    auto start = std::chrono::high_resolution_clock::now();
    int count = simd_string_search(text, pattern);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Text size: " << text.length() << " characters" << std::endl;
    std::cout << "Pattern: \"" << pattern << "\"" << std::endl;
    std::cout << "Occurrences found: " << count << std::endl;
    std::cout << "Time: " << duration.count() << " ms" << std::endl;
}
