#include <gtest/gtest.h>
#include <modules/PrintSink.hpp>
#include <flow/blocks/Sink.hpp>

#include "TestBlocks.h"
#include "flow/blocks/Pipeline.hpp"
#include "modules/NumberSource.h"

TEST(SinkTests, ProcessAnyInvokesProcess) {
    TestSink sink;
    sink.process_any(5, 0);
    sink.process_any(10, 0);

    EXPECT_EQ((std::vector<int>{5,10}), sink.collected);
}

TEST(SinkTests, SinkInPipelineCollectsValues) {
    pt::flow::Pipeline p("p");

    p.add(std::make_shared<pt::modules::NumberSource>(42));
    auto sink = p.add(std::make_shared<TestSink>());

    p.execute();
    ASSERT_EQ(1u, sink->collected.size());
    EXPECT_EQ(42, sink->collected[0]);
}

TEST(SinkTests, MultipleInputsCollected) {
    TestSink sink;
    sink.process_any(1, 0);
    sink.process_any(2, 0);
    sink.process_any(3, 0);

    EXPECT_EQ((std::vector<int>{1,2,3}), sink.collected);
}

TEST(SinkTests, PipelineWithMultipleSources) {
    pt::flow::Pipeline p("p");

    auto src1 = p.add(std::make_shared<pt::modules::NumberSource>(7));
    auto src2 = std::make_shared<pt::modules::NumberSource>(8);

    auto sink = p.add(std::make_shared<TestSink>());
    pt::flow::Pipeline::connect(src2, sink);

    // execute both sources
    p.execute();
    src2->execute();

    ASSERT_EQ(2u, sink->collected.size());
    EXPECT_EQ(7, sink->collected[0]);
    EXPECT_EQ(8, sink->collected[1]);
}
