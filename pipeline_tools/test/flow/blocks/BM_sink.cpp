#include <benchmark/benchmark.h>
#include "TestBlocks.hpp"

static void BM_Sink_ProcessAny(benchmark::State& state) {
    TestSink sink;
    for (auto _ : state) {
        sink.process_any(123, 0);
    }
}
BENCHMARK(BM_Sink_ProcessAny);