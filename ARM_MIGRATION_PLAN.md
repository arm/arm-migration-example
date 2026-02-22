# ARM Migration Plan - arm-migration-example

This document outlines the plan for migrating the `arm-migration-example` repository from x86-64 SSE2 to ARM64 NEON.

## 1. Codebase Analysis

The codebase consists of several C++ files that use SSE2 intrinsics for performance optimization. Each file follows a pattern of checking for `__x86_64__` and using `#if USE_X86_SIMD`.

### Affected Files:
- `matrix_operations.cpp`: Matrix multiplication using `__m128d`.
- `hash_operations.cpp`: DJB2-like hash using `__m128i`.
- `memory_operations.cpp`: Fast memory copy using `__m128i`.
- `polynomial_eval.cpp`: Polynomial evaluation using `__m128d`.
- `string_search.cpp`: String search using `__m128i` and `_mm_movemask_epi8`.

## 2. Migration Strategy

### Header and Guard Updates
In each file, I will update the architecture guards:
```cpp
#if defined(__x86_64__)
#include <immintrin.h>
#define USE_X86_SIMD 1
#elif defined(__aarch64__)
#include <arm_neon.h>
#define USE_ARM_NEON 1
#else
#define USE_SCALAR 1
#endif
```

### SIMD Replacements

| x86 SSE2 | ARM NEON Equivalent |
|---|---|
| `__m128d` | `float64x2_t` |
| `__m128i` | `uint8x16_t` or `int32x4_t` |
| `_mm_setzero_pd()` | `vdupq_n_f64(0.0)` |
| `_mm_loadu_pd()` | `vld1q_f64()` |
| `_mm_storeu_pd()` | `vst1q_f64()` |
| `_mm_add_pd()` | `vaddq_f64()` |
| `_mm_mul_pd()` | `vmulq_f64()` |
| `_mm_set_pd(b, a)` | `(float64x2_t){a, b}` |
| `_mm_set1_pd(a)` | `vdupq_n_f64(a)` |
| `_mm_loadu_si128()` | `vld1q_u8()` |
| `_mm_storeu_si128()` | `vst1q_u8()` |
| `_mm_set1_epi8()` | `vdupq_n_u8()` |
| `_mm_cmpeq_epi8()` | `vceqq_u8()` |
| `_mm_movemask_epi8()` | Use `vgetq_lane_u64` or horizontal max |

## 3. Dockerfile Updates
- Ensure `ubuntu:22.04` is used (multi-arch).
- Update compiler flags if necessary (though NEON is default on ARM64).

## 4. Validation Plan
- Provision a Tau T2A ARM VM.
- Build the application using the updated Dockerfile.
- Run the benchmarks and verify correctness and performance.
