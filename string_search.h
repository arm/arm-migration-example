#ifndef STRING_SEARCH_H
#define STRING_SEARCH_H

#include <string>

// SIMD-accelerated string search using x86 SSE2
int simd_string_search(const std::string& text, const std::string& pattern);

// Benchmark function
void benchmark_string_ops();

#endif // STRING_SEARCH_H
