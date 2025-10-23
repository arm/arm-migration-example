#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H

#include <vector>
#include <cstddef>

// Matrix class with x86 SSE2 optimizations
class Matrix {
private:
    std::vector<std::vector<double>> data;
    size_t rows;
    size_t cols;

public:
    Matrix(size_t r, size_t c);
    void randomize();
    Matrix multiply(const Matrix& other) const;
    double sum() const;

    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }
};

// Benchmark function
void benchmark_matrix_ops();

#endif // MATRIX_OPERATIONS_H
