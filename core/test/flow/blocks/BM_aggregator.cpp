#include "../flow_test_utils.hpp"

#include <modules/math/SumAggregator.hpp>
#include <benchmark/benchmark.h>

static void BM_Aggregator(benchmark::State& state) {
    pt::modules::SumAggregator<int, int> agg;
    for (auto _ : state) {
        benchmark::DoNotOptimize(agg.process_any(69, 0));
    }
}
BENCHMARK(BM_Aggregator);