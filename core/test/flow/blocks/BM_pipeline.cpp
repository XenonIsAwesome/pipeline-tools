#include <benchmark/benchmark.h>
#include <pt/core/flow/Pipeline.hpp>
#include <pt/core/modules/io/ConstantSource.hpp>
#include <pt/core/modules/math/AdditionModule.hpp>
#include "../flow_test_utils.hpp"

static void BM_Pipeline_ExecuteChain(benchmark::State &state) {
    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::ConstantSource<int> >(1));
    p.add(std::make_shared<pt::modules::AdditionModule<int, int, int> >(2));
    p.add(std::make_shared<MockSink>());

    for (auto _: state) {
        p.execute();
    }
}
BENCHMARK(BM_Pipeline_ExecuteChain);
