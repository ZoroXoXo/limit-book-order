cmake_minimum_required(VERSION 3.20)
project(limit_order_book CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Low-latency compiler flags
add_compile_options(-O3 -march=native -Wall -Wextra -Wpedantic)

# Include path for standalone headers
include_directories(include)

# Google Benchmark Dependency
find_package(benchmark REQUIRED)

# Executable target for LOB microbenchmarks
add_executable(bench_lob benchmarks/bench_lob.cpp)
target_link_libraries(bench_lob PRIVATE benchmark::benchmark)
