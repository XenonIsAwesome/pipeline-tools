#include <benchmark/benchmark.h>
#include "TestBlocks.hpp"
#include <memory>

static void BM_Aggregator_Synchronized(benchmark::State& state) {
    TestAggregator agg(pt::flow::AggregationPolicy::Synchronized);
    for (auto _ : state) {
        agg.process_any(5, 0);
    }
}
BENCHMARK(BM_Aggregator_Synchronized);

static void BM_Aggregator_Continuous(benchmark::State& state) {
    TestAggregator agg(pt::flow::AggregationPolicy::Continuous);
    for (auto _ : state) {
        agg.process_any(5, 0);
    }
}
BENCHMARK(BM_Aggregator_Continuous);
