#include <flow/Pipeline.hpp>
#include <flow/blocks/Aggregator.hpp>
#include <gtest/gtest.h>
#include <modules/io/ConstantSource.hpp>
#include "../FlowTestObjects.hpp"
#include "modules/math/SumAggregator.hpp"


TEST(AggregatorTests, Aggregation) {
    pt::flow::Pipeline p;
    auto src1 = p.add(std::make_shared<pt::modules::ConstantSource<int>>(2));
    auto src2 = p.add(std::make_shared<pt::modules::ConstantSource<int>>(3));
    auto agg = p.add(std::make_shared<pt::modules::SumAggregator<int, int>>(0));
    pt::flow::Pipeline::connect(src2, agg);

    auto sink = p.add(std::make_shared<MockSink>());
    p.execute();
    src2->execute();

    ASSERT_EQ(1u, sink->collected.size());
    EXPECT_EQ(5, sink->collected[0]);
}
