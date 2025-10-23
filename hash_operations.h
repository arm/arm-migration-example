#ifndef HASH_OPERATIONS_H
#define HASH_OPERATIONS_H

#include <cstddef>

// Hash computation using x86 SSE2 for parallel processing
unsigned long long compute_hash(const char* data, size_t len);

// Benchmark function
void benchmark_hashing();

#endif // HASH_OPERATIONS_H
