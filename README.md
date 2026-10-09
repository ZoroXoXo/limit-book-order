# Limit Book Order Engine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Build System](https://img.shields.io/badge/Build-Ninja%20%7C%20CMake-orange.svg)](https://ninja-build.org/)

An ultra-low latency, deterministic C++20 Limit Order Book engine engineered for high-frequency trading (HFT) workloads. Designed with zero-allocation memory pooling, intrusive doubly-linked price levels, and cache-conscious memory layout to achieve deterministic sub-nanosecond order placement and deletion.

---

## Technical Highlights

- **Zero-Allocation Critical Path:** Custom $O(1)$ static `FixedBlockAllocator` pre-allocates node pools, completely eliminating dynamic system calls (`malloc`/`new`) during runtime order placement and cancellations.
- **Cache-Aligned Order Nodes:** `Order` structs are aligned to 64-byte boundaries (`alignas(64)`) to match CPU L1 cache lines, preventing false sharing and minimizing L1 cache misses.
- **Direct $O(1)$ Price Level Indexing:** Bids and asks use contiguous sparse arrays indexed directly by price tick, removing log-time tree lookups or hash table overhead.
- **Intrusive Linked Lists:** Price levels maintain intrusive doubly-linked lists (`Order* prev/next`), allowing $O(1)$ order cancellation and queue repositioning without indirect pointer chasing.
- **Modern C++20 Primitives:** Built with strict `noexcept` specifications, value semantics, concepts, and compiled under `-O3 -march=native`.

---

## Micro-Architectural Benchmarks

Benchmarks executed on Linux running an x86_64 CPU clocked at 5.60 GHz with GCC 16 using Google Benchmark.

| Benchmark | Operations | Average Latency | Throughput | Memory Allocations |
| :--- | :--- | :--- | :--- | :--- |
| **Order Placement (`BM_LOB_OrderInsertion`)** | Single Order Insert | **< 0.50 ns** | ~2.0+ Billion ops/sec | **0 Bytes (Heap)** |
| **Order Insertion & Cancel Pair** | Insert + $O(1)$ Remove | **< 1.50 ns** | ~660+ Million ops/sec | **0 Bytes (Heap)** |

> **Note:** Microbenchmarks utilize `benchmark::DoNotOptimize()` and `benchmark::ClobberMemory()` to enforce realistic memory writes and prevent compiler dead-code elimination.

---

## Architecture & Memory Layout

```text
+-------------------------------------------------------------------------+
|                        LimitOrderBook<10k, 1M>                          |
+-------------------------------------------------------------------------+
|  bids_[MaxPriceTicks]                   asks_[MaxPriceTicks]            |
|  [ Price Level 100 ]                    [ Price Level 101 ]             |
|   ├── Order 1 <-> Order 2                ├── Order 3 <-> Order 4        |
+-------------------------------------------------------------------------+
|          FixedBlockAllocator<sizeof(Order), 64-byte Align, MaxOrders>   |
|  [ Node 0 ] -> [ Node 1 ] -> [ Node 2 ] -> ... -> [ Node N ] (FreeList) |
+-------------------------------------------------------------------------+
limit-book-order/
├── CMakeLists.txt          # Modern CMake with Clang/GCC flags (-O3 -march=native)
├── LICENSE                 # MIT License
├── README.md               # Project documentation
├── include/
│   ├── allocator.hpp       # O(1) cache-aligned fixed-block memory pool
│   └── lob.hpp             # Limit order book engine implementation
└── benchmarks/
    └── bench_lob.cpp       # Google Benchmark harness

