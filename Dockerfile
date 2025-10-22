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

# Build the application without ARM-specific optimizations
# Explicitly avoiding flags like -march=native, -mcpu, or ARM NEON intrinsics
RUN g++ -O2 -o benchmark main.cpp -std=c++11

# Create a startup script
COPY start.sh .
RUN chmod +x start.sh

# Run the application
CMD ["./start.sh"]