#include "string_search.h"
#include <iostream>
#include <chrono>

#ifdef __aarch64__
#include <arm_neon.h>
#define USE_ARM_NEON 1
#else
#define USE_ARM_NEON 0
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

#if USE_ARM_NEON
    // ARM64 optimized path using NEON
    uint8x16_t first_char_vec = vdupq_n_u8(static_cast<uint8_t>(first_char));

    for (; i + 16 <= text_len - pattern_len + 1; i += 16) {
        uint8x16_t text_chunk = vld1q_u8(reinterpret_cast<const uint8_t*>(text.data() + i));
        uint8x16_t cmp = vceqq_u8(text_chunk, first_char_vec);

        // Store comparison result to array to avoid variable lane index
        uint8_t cmp_bytes[16];
        vst1q_u8(cmp_bytes, cmp);

        // Check each potential match
        for (int bit = 0; bit < 16 && i + bit <= text_len - pattern_len; bit++) {
            if (cmp_bytes[bit] != 0) {
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
#endif

    // Handle remaining characters (or all on non-ARM)
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
