#include <flow/Pipeline.hpp>
#include <flow/blocks/Source.hpp>
#include <gtest/gtest.h>
#include <modules/io/ConstantSource.hpp>
#include "../FlowTestObjects.hpp"

TEST(SourceTests, ProcessAnyInvokesProcess) {
    pt::modules::ConstantSource src(123);

    // process_any should call process() and wrap result
    auto out_any = src.process_any({}, 0);
    ASSERT_TRUE(out_any.has_value());
    EXPECT_EQ(123, std::any_cast<int>(out_any));
}

TEST(SourceTests, ProcessAnyEmptyOptional) {
    // A source that always returns std::nullopt
    NullSource src;
    auto out_any = src.process_any({}, 0);

    // Expect empty any
    EXPECT_TRUE(out_any.has_value());
    EXPECT_EQ(out_any.type(), typeid(std::nullopt_t));
}

TEST(SourceTests, SourceWorksInPipeline) {
    pt::flow::Pipeline p;
    p.add(std::make_shared<pt::modules::ConstantSource<int>>(77));
    auto sink = p.add(std::make_shared<MockSink>());

    p.execute();
    ASSERT_EQ(1, sink->collected.size());
    EXPECT_EQ(77, sink->collected[0]);
}