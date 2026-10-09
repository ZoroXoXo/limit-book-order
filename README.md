# Limit Book Order Engine

An ultra-low latency, deterministic C++20 Limit Order Book engine designed for high-frequency trading application workloads. Built with zero-allocation memory pooling and cache-conscious data structures to achieve sub-nanosecond insertion and execution characteristics.

---

## Technical Highlights

- **C++20 Implementation:** Utilizes modern C++ concepts, strict zero-copy primitives, and noexcept bounds.
- **$O(1)$ Fixed-Block Memory Allocator:** Eliminates system call overhead (`malloc`/`new`) on the critical path using a custom static block memory pool.
- **Cache-Aligned Order Nodes:** `Order` structs are aligned to 64-byte boundaries (`alignas(64)`) to eliminate false sharing and match CPU L1 cache lines.
- **Intrusive Doubly Linked Lists:** Fast price-level insertion and cancellation without indirect pointer chasing or dynamic re-allocations.
- **Compiler Optimizations:** Compiled with `-O3 -march=native` and built with Ninja/CMake.

---

## Architecture & Benchmark Design
+-------------------------------------------------------+
|                 LimitOrderBook<10k, 1M>               |
+-------------------------------------------------------+
|  bids_[MaxPriceTicks]  |  asks_[MaxPriceTicks]        |
|  [ Price Level 100 ]   |  [ Price Level 101 ]         |
|   ├── Order 1 <-> 2    |   ├── Order 3 <-> 4          |
+-------------------------------------------------------+
|  FixedBlockAllocator<sizeof(Order), 64, MaxOrders>    |
+-------------------------------------------------------+
---

## Build and Benchmark Instructions

### Prerequisites

- GCC 12+ or Clang 14+ (C++20 support)
- CMake 3.20+
- Ninja build tool
- Google Benchmark library

### Building

```bash
# Configure release build
cmake -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build benchmark executable
ninja -C build-release bench_lob

# Run Google Benchmark
./build-release/bench_lob
