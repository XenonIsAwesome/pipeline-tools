#include "../flow_test_utils.hpp"

#include <flow/Pipeline.hpp>
#include <gtest/gtest.h>
#include <modules/io/ConstantSource.hpp>
#include <modules/math/SumAggregator.hpp>

TEST(AggregatorTests, Aggregation) {
    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::ConstantSource<int>>(420));
    p.add(std::make_shared<pt::modules::ConstantSource<int>>(69));
    p.add(std::make_shared<pt::modules::SumAggregator<int, int>>());
    auto sink = p.add(std::make_shared<MockSink<int>>());

    p.execute();

    ASSERT_EQ(1, sink->collected.size());
    EXPECT_EQ(420 + 69, sink->collected[0]);
}