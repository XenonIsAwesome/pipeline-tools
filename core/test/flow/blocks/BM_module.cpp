#include "../flow_test_utils.hpp"

#include <benchmark/benchmark.h>
#include <flow/Pipeline.hpp>
#include <modules/math/AdditionModule.hpp>

static void BM_Module_ProcessAny(benchmark::State& state) {
    pt::modules::AdditionModule<int, float, float> mod(0.69);
    for (auto _ : state) {
        benchmark::DoNotOptimize(mod.process_any(420, 0));
    }
}
BENCHMARK(BM_Module_ProcessAny);

static void BM_Module_Produce(benchmark::State& state) {
    pt::flow::Pipeline p;

    auto producer = p.add(std::make_shared<pt::modules::AdditionModule<int, int, int>>(1));

    int num_consumers = static_cast<int>(state.range(0));
    for (size_t i = 0; i < num_consumers; i++) {
        p.add(std::make_shared<MockSink<int>>());
    }

    for (auto _ : state) {
        producer->execute(5);
    }
}
BENCHMARK(BM_Module_Produce)->RangeMultiplier(5)->Range(1, 125);