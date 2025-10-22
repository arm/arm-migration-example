/*
 * ARM-Optimized Compute-Intensive Benchmark Application
 * Optimized for ARM architecture with NEON SIMD, crypto extensions,
 * and memory optimizations for maximum performance.
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <cstring>
#include <random>

// ARM-specific optimizations
#ifdef __aarch64__
#include <arm_neon.h>
#include <arm_acle.h>
#endif

// ARM NEON optimized Matrix operations
class Matrix {
private:
    std::vector<std::vector<double>> data;
    size_t rows;
    size_t cols;

public:
    Matrix(size_t r, size_t c) : rows(r), cols(c) {
        data.resize(rows, std::vector<double>(cols, 0.0));
    }

    void randomize() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 10.0);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                data[i][j] = dis(gen);
            }
        }
    }

    // ARM NEON optimized matrix multiplication
    Matrix multiply(const Matrix& other) const {
        if (cols != other.rows) {
            throw std::runtime_error("Invalid matrix dimensions for multiplication");
        }

        Matrix result(rows, other.cols);

#ifdef __aarch64__
        // ARM NEON SIMD optimized implementation
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < other.cols; j++) {
                float64x2_t sum_vec = vdupq_n_f64(0.0);
                size_t k = 0;
                
                // Process 2 elements at a time with NEON
                for (; k + 1 < cols; k += 2) {
                    // Prefetch next cache lines for better memory access
                    __builtin_prefetch(&data[i][k + 16], 0, 1);
                    __builtin_prefetch(&other.data[k + 16][j], 0, 1);
                    
                    float64x2_t a_vec = vld1q_f64(&data[i][k]);
                    float64x2_t b_vec = {other.data[k][j], other.data[k+1][j]};
                    
                    // Use fused multiply-add for better precision and performance
                    sum_vec = vfmaq_f64(sum_vec, a_vec, b_vec);
                }
                
                // Sum the NEON vector elements
                double sum = vgetq_lane_f64(sum_vec, 0) + vgetq_lane_f64(sum_vec, 1);
                
                // Handle remaining elements
                for (; k < cols; k++) {
                    sum += data[i][k] * other.data[k][j];
                }
                
                result.data[i][j] = sum;
            }
        }
#else
        // Fallback for non-ARM architectures
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < other.cols; j++) {
                double sum = 0.0;
                for (size_t k = 0; k < cols; k++) {
                    sum += data[i][k] * other.data[k][j];
                }
                result.data[i][j] = sum;
            }
        }
#endif

        return result;
    }

    double sum() const {
        double total = 0.0;
        
#ifdef __aarch64__
        // ARM NEON optimized summation
        float64x2_t sum_vec = vdupq_n_f64(0.0);
        
        for (size_t i = 0; i < rows; i++) {
            size_t j = 0;
            // Process 2 elements at a time
            for (; j + 1 < cols; j += 2) {
                __builtin_prefetch(&data[i][j + 8], 0, 1);
                float64x2_t data_vec = vld1q_f64(&data[i][j]);
                sum_vec = vaddq_f64(sum_vec, data_vec);
            }
            // Handle remaining elements
            for (; j < cols; j++) {
                total += data[i][j];
            }
        }
        
        total += vgetq_lane_f64(sum_vec, 0) + vgetq_lane_f64(sum_vec, 1);
#else
        // Fallback for non-ARM architectures
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                total += data[i][j];
            }
        }
#endif
        
        return total;
    }
};

// ARM crypto extensions optimized hash function
unsigned long long compute_hash(const char* data, size_t len) {
#ifdef __aarch64__
    // Use ARM CRC32 instruction for faster hashing
    uint32_t crc = 0;
    size_t i = 0;
    
    // Process 8 bytes at a time using CRC32X
    for (; i + 7 < len; i += 8) {
        uint64_t chunk;
        memcpy(&chunk, &data[i], 8);
        crc = __crc32d(crc, chunk);
    }
    
    // Process 4 bytes at a time using CRC32W
    for (; i + 3 < len; i += 4) {
        uint32_t chunk;
        memcpy(&chunk, &data[i], 4);
        crc = __crc32w(crc, chunk);
    }
    
    // Process remaining bytes
    for (; i < len; i++) {
        crc = __crc32b(crc, data[i]);
    }
    
    // Combine with DJB2 hash for better distribution
    unsigned long long hash = 5381;
    hash = ((hash << 5) + hash) + crc;
    
    return hash;
#else
    // Fallback for non-ARM architectures
    unsigned long long hash = 5381;
    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + data[i];
    }
    return hash;
#endif
}

// ARM NEON optimized string search
int naive_string_search(const std::string& text, const std::string& pattern) {
    int count = 0;
    size_t text_len = text.length();
    size_t pattern_len = pattern.length();

#ifdef __aarch64__
    // ARM NEON optimized string matching for longer patterns
    if (pattern_len >= 8) {
        uint8x16_t pattern_vec = vdupq_n_u8(pattern[0]);
        
        for (size_t i = 0; i <= text_len - pattern_len; i += 16) {
            // Prefetch next cache line
            __builtin_prefetch(&text[i + 64], 0, 1);
            
            // Load 16 bytes of text
            size_t remaining = std::min(static_cast<size_t>(16), text_len - i);
            if (remaining < 16) break;
            
            uint8x16_t text_vec = vld1q_u8(reinterpret_cast<const uint8_t*>(&text[i]));
            uint8x16_t cmp_result = vceqq_u8(text_vec, pattern_vec);
            
            // Check each lane for matches
            uint64x2_t result64 = vreinterpretq_u64_u8(cmp_result);
            if (vgetq_lane_u64(result64, 0) || vgetq_lane_u64(result64, 1)) {
                // Potential match found, verify with detailed comparison
                for (int j = 0; j < 16 && i + j <= text_len - pattern_len; j++) {
                    if (text[i + j] == pattern[0]) {
                        bool match = true;
                        for (size_t k = 1; k < pattern_len; k++) {
                            if (text[i + j + k] != pattern[k]) {
                                match = false;
                                break;
                            }
                        }
                        if (match) count++;
                    }
                }
            }
        }
    } else {
        // Use standard algorithm for short patterns
        for (size_t i = 0; i <= text_len - pattern_len; i++) {
            bool match = true;
            for (size_t j = 0; j < pattern_len; j++) {
                if (text[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) count++;
        }
    }
#else
    // Fallback for non-ARM architectures
    for (size_t i = 0; i <= text_len - pattern_len; i++) {
        bool match = true;
        for (size_t j = 0; j < pattern_len; j++) {
            if (text[i + j] != pattern[j]) {
                match = false;
                break;
            }
        }
        if (match) count++;
    }
#endif

    return count;
}

// ARM optimized memory copy with prefetching
void slow_memcpy(void* dest, const void* src, size_t n) {
    char* d = static_cast<char*>(dest);
    const char* s = static_cast<const char*>(src);

#ifdef __aarch64__
    // ARM NEON optimized memory copy
    size_t i = 0;
    
    // Process 16 bytes at a time with NEON
    for (; i + 15 < n; i += 16) {
        // Prefetch next cache lines for better memory throughput
        __builtin_prefetch(s + i + 64, 0, 1);
        __builtin_prefetch(d + i + 64, 1, 1);
        
        uint8x16_t data_vec = vld1q_u8(reinterpret_cast<const uint8_t*>(s + i));
        vst1q_u8(reinterpret_cast<uint8_t*>(d + i), data_vec);
    }
    
    // Process 8 bytes at a time
    for (; i + 7 < n; i += 8) {
        uint64x1_t data_vec = vld1_u64(reinterpret_cast<const uint64_t*>(s + i));
        vst1_u64(reinterpret_cast<uint64_t*>(d + i), data_vec);
    }
    
    // Handle remaining bytes
    for (; i < n; i++) {
        d[i] = s[i];
    }
#else
    // Fallback for non-ARM architectures
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
#endif
}

// ARM FMA optimized polynomial evaluation
double polynomial_eval(double x, const std::vector<double>& coeffs) {
#ifdef __aarch64__
    // Use ARM FMA (Fused Multiply-Add) instructions for better precision and performance
    double result = 0.0;
    double power = 1.0;

    for (size_t i = 0; i < coeffs.size(); i++) {
        // FMA: result = result + (coeffs[i] * power)
        result = __builtin_fma(coeffs[i], power, result);
        power *= x;
    }

    return result;
#else
    // Fallback for non-ARM architectures
    double result = 0.0;
    double power = 1.0;

    for (size_t i = 0; i < coeffs.size(); i++) {
        result = result + coeffs[i] * power;
        power = power * x;
    }

    return result;
#endif
}

// Benchmark matrix multiplication
void benchmark_matrix_ops() {
    std::cout << "\n=== Matrix Multiplication Benchmark ===" << std::endl;

    const size_t size = 200;
    Matrix a(size, size);
    Matrix b(size, size);

    a.randomize();
    b.randomize();

    auto start = std::chrono::high_resolution_clock::now();
    Matrix c = a.multiply(b);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Matrix size: " << size << "x" << size << std::endl;
    std::cout << "Time: " << duration.count() << " ms" << std::endl;
    std::cout << "Result sum: " << c.sum() << std::endl;
}

// Benchmark hashing
void benchmark_hashing() {
    std::cout << "\n=== Hashing Benchmark ===" << std::endl;

    const size_t data_size = 10 * 1024 * 1024; // 10 MB
    std::vector<char> data(data_size);

    // Fill with random data
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

// Benchmark string operations
void benchmark_string_ops() {
    std::cout << "\n=== String Search Benchmark ===" << std::endl;

    // Create a large text
    std::string text;
    for (int i = 0; i < 100000; i++) {
        text += "The quick brown fox jumps over the lazy dog. ";
    }

    std::string pattern = "fox";

    auto start = std::chrono::high_resolution_clock::now();
    int count = naive_string_search(text, pattern);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Text size: " << text.length() << " characters" << std::endl;
    std::cout << "Pattern: \"" << pattern << "\"" << std::endl;
    std::cout << "Occurrences found: " << count << std::endl;
    std::cout << "Time: " << duration.count() << " ms" << std::endl;
}

// Benchmark memory operations
void benchmark_memory_ops() {
    std::cout << "\n=== Memory Operations Benchmark ===" << std::endl;

    const size_t size = 50 * 1024 * 1024; // 50 MB
    std::vector<char> src(size, 'A');
    std::vector<char> dest(size);

    auto start = std::chrono::high_resolution_clock::now();
    slow_memcpy(dest.data(), src.data(), size);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Memory size: " << size / 1024 / 1024 << " MB" << std::endl;
    std::cout << "Time: " << duration.count() << " ms" << std::endl;
    std::cout << "Throughput: " << (size / 1024.0 / 1024.0) / (duration.count() / 1000.0)
              << " MB/s" << std::endl;
}

// Benchmark polynomial evaluation
void benchmark_polynomial() {
    std::cout << "\n=== Polynomial Evaluation Benchmark ===" << std::endl;

    std::vector<double> coeffs = {1.0, 2.5, -3.2, 4.8, -1.5, 2.0, -0.5};
    const int iterations = 10000000;

    auto start = std::chrono::high_resolution_clock::now();
    double sum = 0.0;
    for (int i = 0; i < iterations; i++) {
        sum += polynomial_eval(1.5 + i * 0.0001, coeffs);
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Iterations: " << iterations << std::endl;
    std::cout << "Time: " << duration.count() << " ms" << std::endl;
    std::cout << "Result sum: " << sum << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  ARM-Optimized Compute Benchmark Suite" << std::endl;
#ifdef __aarch64__
    std::cout << "  (ARM NEON + Crypto Extensions Enabled)" << std::endl;
#else
    std::cout << "  (Generic Version - ARM optimizations disabled)" << std::endl;
#endif
    std::cout << "========================================" << std::endl;

    // Run all benchmarks
    benchmark_matrix_ops();
    benchmark_hashing();
    benchmark_string_ops();
    benchmark_memory_ops();
    benchmark_polynomial();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  All benchmarks completed!" << std::endl;
#ifdef __aarch64__
    std::cout << "  ARM optimizations: NEON SIMD, CRC32, FMA, Prefetching" << std::endl;
#endif
    std::cout << "========================================" << std::endl;

    return 0;
}
