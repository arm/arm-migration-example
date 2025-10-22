FROM ubuntu:22.04

# Install build dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy the C++ source code
COPY main.cpp .

# Build the application with architecture-specific optimizations
# Detect architecture and apply appropriate flags
RUN if [ "$(uname -m)" = "aarch64" ]; then \
        g++ -O3 -march=armv8-a+simd+crypto+crc \
            -mcpu=native -mtune=native \
            -ffast-math -funroll-loops \
            -fprefetch-loop-arrays \
            -o benchmark main.cpp -std=c++11; \
    else \
        g++ -O3 -march=native -mtune=native \
            -ffast-math -funroll-loops \
            -fprefetch-loop-arrays \
            -o benchmark main.cpp -std=c++11; \
    fi

# Create a startup script
COPY start.sh .
RUN chmod +x start.sh

# Run the application
CMD ["./start.sh"]