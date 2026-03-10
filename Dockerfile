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

# Build the application with optimizations
# Detects architecture at build time: uses NEON on AArch64, native opts on x86-64
RUN ARCH=$(uname -m) && \
    if [ "$ARCH" = "aarch64" ]; then \
        MARCH_FLAG="-march=armv8-a"; \
    else \
        MARCH_FLAG="-march=native"; \
    fi && \
    g++ -O3 $MARCH_FLAG -o benchmark \
        main.cpp \
        matrix_operations.cpp \
        hash_operations.cpp \
        string_search.cpp \
        memory_operations.cpp \
        polynomial_eval.cpp \
        -std=c++14

# Create a startup script
COPY start.sh .
RUN chmod +x start.sh

# Run the application
CMD ["./start.sh"]