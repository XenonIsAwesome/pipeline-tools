#include <flow/Pipeline.hpp>
#include <flow/blocks/Sink.hpp>
#include <gtest/gtest.h>
#include <modules/io/ConstantSource.hpp>
#include "../flow_test_utils.hpp"

TEST(SinkTests, ProcessAnyInvokesProcess) {
    MockSink sink;
    sink.process_any(5, 0);
    sink.process_any(10, 0);

    EXPECT_EQ((std::vector{5,10}), sink.collected);
}

TEST(SinkTests, SinkInPipelineCollectsValues) {
    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::ConstantSource<int> >(42));
    auto sink = p.add(std::make_shared<MockSink>());

    p.execute();
    ASSERT_EQ(1, sink->collected.size());
    EXPECT_EQ(42, sink->collected[0]);
}

TEST(SinkTests, MultipleInputsCollected) {
    MockSink sink;
    sink.process_any(1, 0);
    sink.process_any(2, 0);
    sink.process_any(3, 0);

    EXPECT_EQ((std::vector{1,2,3}), sink.collected);
}

TEST(SinkTests, PipelineWithMultipleSources) {
    pt::flow::Pipeline p;

    auto src1 = p.add(std::make_shared<pt::modules::ConstantSource<int> >(7));
    auto src2 = p.add(std::make_shared<pt::modules::ConstantSource<int> >(8));

    auto sink = p.add(std::make_shared<MockSink>());
    pt::flow::Pipeline::connect(src2, sink);

    // execute both sources
    p.execute();

    ASSERT_EQ(2, sink->collected.size());
    EXPECT_EQ(7, sink->collected[0]);
    EXPECT_EQ(8, sink->collected[1]);
}
