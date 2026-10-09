#include <benchmark/benchmark.h>
#include "lob.hpp"
#include <vector>

static void BM_LOB_OrderInsertion(benchmark::State& state) {
    systems::lob::LimitOrderBook<10000, 1000000> lob;
    uint64_t id = 1;

    for (auto _ : state) {
        auto* order = lob.add_order(id++, 100, 10, systems::lob::Side::Buy);
        benchmark::DoNotOptimize(order);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_LOB_OrderInsertion);

static void BM_LOB_OrderInsertionAndCancel(benchmark::State& state) {
    systems::lob::LimitOrderBook<10000, 100000> lob;
    uint64_t id = 1;

    for (auto _ : state) {
        auto* order = lob.add_order(id++, 100, 10, systems::lob::Side::Buy);
        benchmark::DoNotOptimize(order);
        lob.cancel_order(order);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_LOB_OrderInsertionAndCancel);

BENCHMARK_MAIN();
