#include "../flow_test_utils.hpp"

#include <benchmark/benchmark.h>

static void BM_Sink_ProcessAny(benchmark::State& state) {
    MockSink<int> sink;
    for (auto _ : state) {
        benchmark::DoNotOptimize(sink.process_any(420, 0));
    }
}
BENCHMARK(BM_Sink_ProcessAny);