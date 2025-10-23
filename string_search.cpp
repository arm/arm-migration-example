#include "string_search.h"
#include <iostream>
#include <chrono>

#ifdef __x86_64__
#include <immintrin.h>
#define USE_X86_SIMD 1
#else
#define USE_X86_SIMD 0
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
