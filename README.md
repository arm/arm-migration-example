# Compute Benchmark Suite

A high-performance compute benchmark application optimized for both x86-64 and ARM64 architectures with SIMD instructions.

## Overview

This benchmark suite tests various compute-intensive operations including:
- Matrix multiplication (200x200 matrices)
- Cryptographic hashing (10MB data processing)
- String pattern matching (4.5M character text search)
- Memory operations (50MB copy operations)
- Polynomial evaluation (10M iterations)

The code is optimized using architecture-specific SIMD intrinsics for maximum performance on Intel/AMD (x86-64) and ARM processors.

## Building with Docker

Build the Docker image:

```bash
docker build -t benchmark-suite .
```

For multi-architecture builds:

```bash
docker buildx build --platform linux/amd64,linux/arm64 -t benchmark-suite .
```

## Running the Benchmark

Run the benchmark suite:

```bash
docker run --rm benchmark-suite
```

This will execute all benchmark tests and display timing results for each operation.

## Architecture Notes

- **Optimized for**: x86-64 with SSE2 support and ARM64 with NEON support
- **x86-64 SIMD**: Uses SSE2 intrinsics (`__m128d`, `__m128i`) for vectorized operations
- **ARM64 SIMD**: Uses NEON intrinsics (`float64x2_t`, `uint8x16_t`) for vectorized operations
- **Fallback**: Includes scalar fallback implementation for platforms without SIMD support

## Output Example

```
========================================
  Compute Benchmark Suite
  ARM64 with NEON Optimizations
========================================

=== Matrix Multiplication Benchmark ===
Matrix size: 200x200
Time: 8 ms
Result sum: 2.00393e+08

=== Hashing Benchmark ===
Data size: 10240 KB
Time: 9 ms
Hash: 0xbfd8e92e2fb01505

...
```

## Project Structure

The benchmark suite is organized into separate modules:

- `main.cpp` - Main entry point and benchmark orchestration
- `matrix_operations.{h,cpp}` - Matrix multiplication with SSE2/NEON optimizations
- `hash_operations.{h,cpp}` - Cryptographic hashing with SIMD acceleration
- `string_search.{h,cpp}` - String pattern matching using SSE2/NEON
- `memory_operations.{h,cpp}` - Fast memory copy operations
- `polynomial_eval.{h,cpp}` - Vectorized polynomial evaluation

Each module uses C++11 standard library and architecture-specific SIMD intrinsics where applicable.

## Supported Architectures

- **x86-64**: Intel and AMD processors with SSE2 support (standard on all 64-bit x86 CPUs)
- **ARM64**: ARM processors with NEON support (ARMv8-A and later)
- **Generic**: Scalar fallback for other architectures