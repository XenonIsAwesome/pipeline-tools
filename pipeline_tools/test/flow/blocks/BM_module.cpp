#include <memory>
#include <benchmark/benchmark.h>
#include <flow/Pipeline.hpp>
#include <modules/io/ConstantSource.hpp>
#include <modules/math/AdditionModule.hpp>
#include "../flow_test_utils.hpp"

static void BM_AdditionModule_ProcessAny(benchmark::State &state) {
    pt::modules::AdditionModule<int, int, int> mod(10);
    for (auto _: state) {
        benchmark::DoNotOptimize(mod.process_any(5, 0));
    }
}

BENCHMARK(BM_AdditionModule_ProcessAny);

static void BM_AdditionModule_Produce(benchmark::State &state) {
    pt::flow::Pipeline p;

    auto producer = p.add(std::make_shared<pt::modules::AdditionModule<int, int, int> >(1));

    int num_consumers = static_cast<int>(state.range(0));
    for (int i = 0; i < num_consumers; ++i) {
        auto consumer = std::make_shared<MockSink>();
        p.add(consumer);
    }

    for (auto _: state) {
        producer->execute(5);
    }
}

BENCHMARK(BM_AdditionModule_Produce)->RangeMultiplier(5)->Range(1, 125);