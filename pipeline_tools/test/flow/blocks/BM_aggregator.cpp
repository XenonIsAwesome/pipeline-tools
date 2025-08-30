#include <memory>
#include <benchmark/benchmark.h>
#include "../flow_test_utils.hpp"
#include <modules/math/SumAggregator.hpp>

static void BM_SumAggregator_ProcessAny(benchmark::State &state) {
    pt::modules::SumAggregator<int, int> agg{0};
    for (auto _: state) {
        agg.process_any(5, 0);
    }
}
BENCHMARK(BM_SumAggregator_ProcessAny);
