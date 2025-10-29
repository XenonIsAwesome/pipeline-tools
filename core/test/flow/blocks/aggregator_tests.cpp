#include <pt/core/flow/Pipeline.hpp>
#include <pt/core/flow/blocks/Aggregator.hpp>
#include <gtest/gtest.h>
#include <pt/core/modules/io/ConstantSource.hpp>
#include "../flow_test_utils.hpp"
#include <pt/core/modules/math/SumAggregator.hpp>


TEST(AggregatorTests, Aggregation) {
    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::ConstantSource<int> >(2));
    p.add(std::make_shared<pt::modules::ConstantSource<int> >(3));
    p.add(std::make_shared<pt::modules::SumAggregator<int, int> >(0));
    auto sink = p.add(std::make_shared<MockSink>());

    p.execute();

    ASSERT_EQ(1, sink->collected.size());
    EXPECT_EQ(5, sink->collected[0]);
}
