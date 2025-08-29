#include <flow/Pipeline.hpp>
#include <gtest/gtest.h>
#include <modules/io/ConstantSource.hpp>
#include <modules/math/AdditionModule.hpp>

#include "../flow_test_utils.hpp"

TEST(PipelineTests, AutoConnectSingleChain) {
    pt::flow::Pipeline p;
    p.add(std::make_shared<pt::modules::ConstantSource<int> >(1));
    p.add(std::make_shared<pt::modules::AdditionModule<int, int, int> >(2));
    auto sink = p.add(std::make_shared<MockSink>());
    p.execute();

    EXPECT_EQ(std::vector{3}, sink->collected);
}

TEST(PipelineTests, ManualConnectBranching) {
    pt::flow::Pipeline p;
    auto src = p.add(std::make_shared<pt::modules::ConstantSource<int> >(10));
    auto mod1 = p.add(std::make_shared<pt::modules::AdditionModule<int, int, int> >(1));
    auto mod2 = std::make_shared<pt::modules::AdditionModule<int, int, int> >(5);
    auto sink1 = p.add(std::make_shared<MockSink>());
    auto sink2 = std::make_shared<MockSink>();

    pt::flow::connect(src, mod2);
    pt::flow::connect(mod2, sink2);

    p.execute();

    ASSERT_EQ(1, sink1->collected.size());
    ASSERT_EQ(1, sink2->collected.size());
    EXPECT_EQ(11, sink1->collected[0]);
    EXPECT_EQ(15, sink2->collected[0]);
}

TEST(PipelineTests, EmptyPipelineDoesNothing) {
    pt::flow::Pipeline p;
    EXPECT_NO_THROW(p.execute());
}
