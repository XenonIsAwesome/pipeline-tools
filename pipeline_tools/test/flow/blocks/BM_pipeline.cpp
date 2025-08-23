#include <benchmark/benchmark.h>
#include <flow/blocks/Pipeline.hpp>
#include <modules/AddModule.h>
#include <modules/NumberSource.h>
#include "TestBlocks.hpp"

static void BM_Pipeline_ExecuteChain(benchmark::State& state) {
    for (auto _ : state) {
        pt::flow::Pipeline p("p");
        p.add(std::make_shared<pt::modules::NumberSource>(1));
        p.add(std::make_shared<pt::modules::AddModule>(2));
        p.add(std::make_shared<TestSink>());
        p.execute();
    }
}
BENCHMARK(BM_Pipeline_ExecuteChain);
