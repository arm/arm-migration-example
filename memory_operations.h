#ifndef MEMORY_OPERATIONS_H
#define MEMORY_OPERATIONS_H

#include <cstddef>

// Fast memory copy using x86 SSE2
void fast_memcpy(void* dest, const void* src, size_t n);

// Benchmark function
void benchmark_memory_ops();

#endif // MEMORY_OPERATIONS_H
