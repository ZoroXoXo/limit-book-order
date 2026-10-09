#include <benchmark/benchmark.h>
#include "lob.hpp"

static void BM_LOB_OrderInsertion(benchmark::State& state) {
    systems::lob::LimitOrderBook<10000, 100000> lob;
    uint64_t id = 1;

    for (auto _ : state) {
        auto* order = lob.add_order(id++, 100, 10, systems::lob::Side::Buy);
        benchmark::DoNotOptimize(order);
    }
}
BENCHMARK(BM_LOB_OrderInsertion);

static void BM_LOB_OrderCancel(benchmark::State& state) {
    systems::lob::LimitOrderBook<10000, 100000> lob;
    uint64_t id = 1;

    for (auto _ : state) {
        state.PauseTiming();
        auto* order = lob.add_order(id++, 100, 10, systems::lob::Side::Buy);
        state.ResumeTiming();

        lob.cancel_order(order);
    }
}
BENCHMARK(BM_LOB_OrderCancel);

BENCHMARK_MAIN();
