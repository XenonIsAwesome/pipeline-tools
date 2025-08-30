#include <flow/Pipeline.hpp>
#include <gtest/gtest.h>
#include <modules/io/ConstantSource.hpp>
#include <modules/math/AdditionModule.hpp>
#include <utils/exceptions/bad_any_cast_with_info.hpp>
#include "../../flow/flow_test_utils.hpp"

TEST(CustomExceptionsTests, BadAnyCastWithInfo) {
    bool did_throw = false;

    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::ConstantSource<char> >('a'));
    p.add(std::make_shared<pt::modules::AdditionModule<int, int, int> >(10));
    p.add(std::make_shared<MockSink>());

    try {
        p.execute();
    } catch (const pt::utils::exceptions::bad_any_cast_with_info &e) {
        did_throw = true;
        std::cout << e.what() << std::endl;
    }

    EXPECT_TRUE(did_throw);
}


TEST(CustomExceptionsTests, UnknownFlowInPipeline) {
    class MockFlow : public pt::flow::Flow {
        std::any process_any(std::any in, size_t producer_id) override {
            return {};
        }
    };

    bool did_throw = false;
    try {
        pt::flow::Pipeline p;
        p.add(std::make_shared<MockFlow>());
    } catch (const pt::utils::exceptions::unknown_flow_object_type &e) {
        did_throw = true;
        std::cout << e.what() << std::endl;
    }

    EXPECT_TRUE(did_throw);
}
