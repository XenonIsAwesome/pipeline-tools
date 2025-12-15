#include <modules/io/ConstantSource.hpp>

#include <benchmark/benchmark.h>

static void BM_Source_ProcessAny(benchmark::State& state) {
    pt::modules::ConstantSource src(42);
    for (auto _ : state) {
        benchmark::DoNotOptimize(src.process_any({}, 0));
    }
}
BENCHMARK(BM_Source_ProcessAny);