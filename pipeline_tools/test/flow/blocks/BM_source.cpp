#include "benchmark/benchmark.h"
#include "modules/NumberSource.h"

static void BM_NumberSource_ProcessAny(benchmark::State& state) {
    pt::modules::NumberSource src(42);
    for (auto _ : state) {
        benchmark::DoNotOptimize(src.process_any({}, 0));
    }
}
BENCHMARK(BM_NumberSource_ProcessAny);