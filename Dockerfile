FROM ubuntu:22.04

# Install build dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy all header files
COPY *.h ./

# Copy all C++ source files
COPY *.cpp ./

# Build the application with optimizations.
# The code detects the target architecture at compile time:
#   x86-64  -> SSE2 SIMD path  (#ifdef __x86_64__)
#   ARM64   -> NEON SIMD path  (#ifdef __aarch64__)
# ubuntu:22.04 is a multi-arch manifest image (amd64 + arm64), so this
# Dockerfile builds correctly on both platforms without any changes.
RUN g++ -O2 -o benchmark \
    main.cpp \
    matrix_operations.cpp \
    hash_operations.cpp \
    string_search.cpp \
    memory_operations.cpp \
    polynomial_eval.cpp \
    -std=c++11

# Create a startup script
COPY start.sh .
RUN chmod +x start.sh

# Run the application
CMD ["./start.sh"]