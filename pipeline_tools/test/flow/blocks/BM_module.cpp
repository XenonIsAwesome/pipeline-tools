#include <benchmark/benchmark.h>
#include <modules/AddModule.h>
#include <memory>

static void BM_AddModule_ProcessAny(benchmark::State& state) {
    pt::modules::AddModule mod(10);
    for (auto _ : state) {
        benchmark::DoNotOptimize(mod.process_any(5, 0));
    }
}
BENCHMARK(BM_AddModule_ProcessAny);
