#include <gtest/gtest.h>
#include <modules/math/AdditionModule.hpp>
#include <modules/io/ConstantSource.hpp>
#include <flow/blocks/Module.hpp>
#include "../FlowTestObjects.hpp"
#include "flow/Pipeline.hpp"

TEST(ModuleTests, ProcessAnyInvokesProcess) {
    pt::modules::AdditionModule<int, int, int> mod(10);

    // `process_any` should unwrap -> call process -> wrap back into std::any
    auto out_any = mod.process_any(5, 0);
    ASSERT_TRUE(out_any.has_value());
    EXPECT_EQ(15, std::any_cast<int>(out_any));
}

TEST(ModuleTests, ProcessAnyHandlesEmptyOptional) {
    // A Module that always returns std::nullopt
    NullModule mod;
    auto out_any = mod.process_any(123, 0);

    // Expect "empty" any
    EXPECT_TRUE(out_any.has_value());
    EXPECT_EQ(out_any.type(), typeid(std::nullopt_t));
}

TEST(ModuleTests, ModuleWorksInsidePipeline) {
    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::ConstantSource<int> >(3));
    p.add(std::make_shared<pt::modules::AdditionModule<int, int, int> >(4));
    auto sink = p.add(std::make_shared<MockSink>());

    p.execute();

    ASSERT_EQ(1, sink->collected.size());
    EXPECT_EQ(7, sink->collected[0]);
}

TEST(ModuleTests, ModuleChainingMultiple) {
    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::ConstantSource<int> >(1));
    p.add(std::make_shared<pt::modules::AdditionModule<int, int, int> >(2));
    p.add(std::make_shared<pt::modules::AdditionModule<int, int, int> >(3));
    auto sink = p.add(std::make_shared<MockSink>());

    p.execute();

    ASSERT_EQ(1, sink->collected.size());
    EXPECT_EQ(6, sink->collected[0]); // 1 + 2 + 3
}
