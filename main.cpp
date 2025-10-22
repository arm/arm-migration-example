/*
 * Compute-Intensive Benchmark Application
 * Designed for performance testing on different architectures
 *
 * This code is intentionally NOT optimized for ARM architecture
 * to demonstrate opportunities for ARM-specific optimizations.
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <cstring>
#include <random>

// Matrix operations without ARM NEON optimizations
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

    // Standard matrix multiplication - not using SIMD
    Matrix multiply(const Matrix& other) const {
        if (cols != other.rows) {
            throw std::runtime_error("Invalid matrix dimensions for multiplication");
        }

        Matrix result(rows, other.cols);

        // Basic triple nested loop - no vectorization hints
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < other.cols; j++) {
                double sum = 0.0;
                for (size_t k = 0; k < cols; k++) {
                    sum += data[i][k] * other.data[k][j];
                }
                result.data[i][j] = sum;
            }
        }

        return result;
    }

    double sum() const {
        double total = 0.0;
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                total += data[i][j];
            }
        }
        return total;
    }
};

// Compute-intensive cryptographic hash (simplified)
// Using scalar operations instead of ARM crypto extensions
unsigned long long compute_hash(const char* data, size_t len) {
    unsigned long long hash = 5381;

    // Character by character processing - no vector instructions
    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + data[i];
    }

    return hash;
}

// String search without optimizations
int naive_string_search(const std::string& text, const std::string& pattern) {
    int count = 0;
    size_t text_len = text.length();
    size_t pattern_len = pattern.length();

    // Naive string matching algorithm
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

    return count;
}

// Memory copy without hardware acceleration
void slow_memcpy(void* dest, const void* src, size_t n) {
    char* d = static_cast<char*>(dest);
    const char* s = static_cast<const char*>(src);

    // Byte-by-byte copy instead of using optimized memcpy
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
}

// Floating point operations without FMA (Fused Multiply-Add)
double polynomial_eval(double x, const std::vector<double>& coeffs) {
    double result = 0.0;
    double power = 1.0;

    // Evaluate polynomial without FMA instructions
    for (size_t i = 0; i < coeffs.size(); i++) {
        result = result + coeffs[i] * power;
        power = power * x;
    }

    return result;
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
    std::cout << "  Compute Benchmark Suite" << std::endl;
    std::cout << "  (Non-ARM Optimized Version)" << std::endl;
    std::cout << "========================================" << std::endl;

    // Run all benchmarks
    benchmark_matrix_ops();
    benchmark_hashing();
    benchmark_string_ops();
    benchmark_memory_ops();
    benchmark_polynomial();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  All benchmarks completed!" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
