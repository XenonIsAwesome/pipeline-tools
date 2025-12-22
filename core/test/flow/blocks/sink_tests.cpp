#include <modules/io/ConstantSource.hpp>
#include "../flow_test_utils.hpp"
#include <flow/Pipeline.hpp>

#include <gtest/gtest.h>

TEST(SinkTests, SinkInPipelineCollectsValues) {
    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::ConstantSource<int>>(420));
    auto sink = p.add(std::make_shared<MockSink<int>>());

    p.execute();
    ASSERT_EQ(1, sink->collected.size());
    EXPECT_EQ(420, sink->collected[0]);
}

TEST(SinkTests, MultipleInputsCollected) {
    MockSink<int> sink;

    sink.process_any(6, 0);
    sink.process_any(9, 0);

    EXPECT_EQ((std::vector<int>{6, 9}), sink.collected);
}

TEST(SinkTests, PipelineWithMultipleSources) {
    pt::flow::Pipeline p{};

    p.add(std::make_shared<pt::modules::ConstantSource<int>>(6));
    p.add(std::make_shared<pt::modules::ConstantSource<int>>(9));
    auto sink = p.add(std::make_shared<MockSink<int>>());

    p.execute();

    ASSERT_EQ(2, sink->collected.size());
    EXPECT_EQ(6, sink->collected[0]);
    EXPECT_EQ(9, sink->collected[1]);
}

TEST(SinkTests, ProcessMoveConstructibleOnlyType) {
    auto sink = MockSink<std::unique_ptr<int>>();

    auto p = std::make_unique<int>(69);
    sink.process(std::move(p));
    ASSERT_EQ(nullptr, p);

    sink.process(std::make_unique<int>(69));
    ASSERT_EQ(2, sink.collected.size());
    EXPECT_EQ(*sink.collected[1], 69);
}