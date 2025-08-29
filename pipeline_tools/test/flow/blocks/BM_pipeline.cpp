#include <benchmark/benchmark.h>
#include <flow/Pipeline.hpp>
#include <modules/io/ConstantSource.hpp>
#include <modules/math/AdditionModule.hpp>
#include "../FlowTestObjects.hpp"

static void BM_Pipeline_ExecuteChain(benchmark::State &state) {
    for (auto _: state) {
        pt::flow::Pipeline p;
        p.add(std::make_shared<pt::modules::ConstantSource<int> >(1));
        p.add(std::make_shared<pt::modules::AdditionModule<int, int, int> >(2));
        p.add(std::make_shared<MockSink>());
        p.execute();
    }
}

BENCHMARK(BM_Pipeline_ExecuteChain);
