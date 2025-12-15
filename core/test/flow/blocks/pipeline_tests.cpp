#include "../flow_test_utils.hpp"

#include <flow/exceptions/unknown_flow_object.h>
#include <flow/exceptions/unordered_sources.h>
#include <flow/Pipeline.hpp>
#include <gtest/gtest.h>
#include <modules/io/ConstantSource.hpp>
#include <modules/math/AdditionModule.hpp>

TEST(PipelineTests, AutoConnectSingleChain) {
    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::ConstantSource<int>>(1));
    p.add(std::make_shared<pt::modules::AdditionModule<int, int, int>>(2));
    auto sink = p.add(std::make_shared<MockSink<int>>());

    p.execute();

    EXPECT_EQ(std::vector{3}, sink->collected);
}

TEST(PipelineTests, ManualConnectBranching) {
    pt::flow::Pipeline p;

    // Auto-wired
    auto src = p.add(std::make_shared<pt::modules::ConstantSource<int>>(10));
    p.add(std::make_shared<pt::modules::AdditionModule<int, int, int>>(1));
    auto sink1 = p.add(std::make_shared<MockSink<int>>());
    auto sink2 = std::make_shared<MockSink<int>>();

    // Manually connected
    auto out_of_pipeline_mod = std::make_shared<pt::modules::AdditionModule<int, int, int>>(5);
    pt::flow::connect(src, out_of_pipeline_mod);
    pt::flow::connect(out_of_pipeline_mod, sink2);

    p.execute();

    ASSERT_EQ(1, sink1->collected.size());
    ASSERT_EQ(1, sink2->collected.size());
    EXPECT_EQ(11, sink1->collected[0]);
    EXPECT_EQ(15, sink2->collected[0]);
}

TEST(PipelineTests, ThrowsUnknownFlowType) {
    class MockFlow: public pt::flow::Flow {
    protected:
        nstd::any process_any(nstd::any, size_t producer_id) override {
            return {};
        }
    };

    pt::flow::Pipeline p;

    bool raises_err = false;
    try {
        p.add(std::make_shared<MockFlow>());
    } catch (const pt::flow::exceptions::unknown_flow_object& e) {
        raises_err = true;
        std::cout << e.what() << std::endl;
    }

    EXPECT_TRUE(raises_err);
}

TEST(PipelineTests, ThrowsCustomBadAnyCast) {
    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::ConstantSource<char>>('a'));
    p.add(std::make_shared<pt::modules::AdditionModule<int, int, int>>(1));

    bool raises_err = false;
    try {
        p.execute();
    } catch (pt::exceptions::bad_any_cast_with_info& e) {
        raises_err = true;
        std::cout << e.what() << std::endl;
    }

    EXPECT_TRUE(raises_err);
}

TEST(PipelineTests, ThrowsUnorderedSources) {
    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::AdditionModule<int, int, int>>(1));

    bool raises_err = false;
    try {
        p.add(std::make_shared<pt::modules::ConstantSource<char>>('a'));
    } catch (pt::flow::exceptions::unordered_sources& e) {
        raises_err = true;
        std::cout << e.what() << std::endl;
    }

    EXPECT_TRUE(raises_err);
}
