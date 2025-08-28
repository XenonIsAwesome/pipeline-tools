#include <benchmark/benchmark.h>
#include <modules/io/ConstantSource.hpp>

static void BM_NumberSource_ProcessAny(benchmark::State& state) {
    pt::modules::ConstantSource src(42);
    for (auto _ : state) {
        benchmark::DoNotOptimize(src.process_any({}, 0));
    }
}
BENCHMARK(BM_NumberSource_ProcessAny);