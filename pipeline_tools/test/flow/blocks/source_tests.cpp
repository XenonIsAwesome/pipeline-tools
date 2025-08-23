#include <gtest/gtest.h>
#include <modules/NumberSource.h>
#include <flow/blocks/Source.hpp>

#include "TestBlocks.hpp"
#include "flow/blocks/Pipeline.hpp"

TEST(SourceTests, ProcessAnyInvokesProcess) {
    pt::modules::NumberSource src(123);

    // process_any should call process() and wrap result
    auto out_any = src.process_any({}, 0);
    ASSERT_TRUE(out_any.has_value());
    EXPECT_EQ(123, std::any_cast<int>(out_any));
}

TEST(SourceTests, ProcessAnyEmptyOptional) {
    // A source that always returns std::nullopt
    class NullSource : public pt::flow::Source<int> {
    public:
        NullSource(): Source("NullSource", pt::flow::ProductionPolicy::SingleOutput) {}
        std::optional<int> process() override { return std::nullopt; }
    };

    NullSource src;
    auto out_any = src.process_any({}, 0);

    // Expect empty any
    EXPECT_FALSE(out_any.has_value());
}

TEST(SourceTests, SourceWorksInPipeline) {
    pt::flow::Pipeline p("p");
    p.add(std::make_shared<pt::modules::NumberSource>(77));
    auto sink = p.add(std::make_shared<TestSink>());

    p.execute();
    ASSERT_EQ(1u, sink->collected.size());
    EXPECT_EQ(77, sink->collected[0]);
}