#include <benchmark/benchmark.h>
#include <memory>
#include "lob.hpp"

static void BM_LOB_OrderInsertion(benchmark::State& state) {
    auto lob = std::make_unique<systems::lob::LimitOrderBook<10000, 1000000>>();
    uint64_t order_id = 1;

    for (auto _ : state) {
        auto* order = lob->add_order(order_id++, 100, 10, systems::lob::Side::Buy);
        benchmark::DoNotOptimize(order);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_LOB_OrderInsertion);

static void BM_LOB_InsertionAndCancel(benchmark::State& state) {
    auto lob = std::make_unique<systems::lob::LimitOrderBook<10000, 1000000>>();
    uint64_t order_id = 1;

    for (auto _ : state) {
        auto* order = lob->add_order(order_id++, 100, 10, systems::lob::Side::Buy);
        lob->cancel_order(order);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_LOB_InsertionAndCancel);

BENCHMARK_MAIN();
