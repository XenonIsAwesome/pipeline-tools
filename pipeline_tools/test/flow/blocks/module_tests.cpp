#include <gtest/gtest.h>
#include <modules/AddModule.h>
#include <modules/NumberSource.h>
#include <flow/blocks/Module.hpp>
#include <flow/blocks/Pipeline.hpp>
#include "TestBlocks.h"

TEST(ModuleTests, ProcessAnyInvokesProcess) {
    pt::modules::AddModule mod(10);

    // `process_any` should unwrap -> call process -> wrap back into std::any
    auto out_any = mod.process_any(5, 0);
    ASSERT_TRUE(out_any.has_value());
    EXPECT_EQ(15, std::any_cast<int>(out_any));
}

TEST(ModuleTests, ProcessAnyHandlesEmptyOptional) {
    // A Module that always returns std::nullopt
    class NullModule : public pt::flow::Module<int,int> {
    public:
        NullModule(): Module("NullModule", pt::flow::ProductionPolicy::SingleOutput) {}
        std::optional<int> process(const int&) override { return std::nullopt; }
    };

    NullModule mod;
    auto out_any = mod.process_any(123, 0);

    // Expect "empty" any
    EXPECT_TRUE(out_any.has_value());
    EXPECT_EQ(out_any.type(), typeid(std::nullopt_t));
}

TEST(ModuleTests, ModuleWorksInsidePipeline) {
    pt::flow::Pipeline p("p");

    p.add(std::make_shared<pt::modules::NumberSource>(3));
    p.add(std::make_shared<pt::modules::AddModule>(4));
    auto sink = p.add(std::make_shared<TestSink>());

    p.execute();

    ASSERT_EQ(1u, sink->collected.size());
    EXPECT_EQ(7, sink->collected[0]);
}

TEST(ModuleTests, ModuleChainingMultiple) {
    pt::flow::Pipeline p("p");

    p.add(std::make_shared<pt::modules::NumberSource>(1));
    p.add(std::make_shared<pt::modules::AddModule>(2));
    p.add(std::make_shared<pt::modules::AddModule>(3));
    auto sink = p.add(std::make_shared<TestSink>());

    p.execute();

    ASSERT_EQ(1u, sink->collected.size());
    EXPECT_EQ(6, sink->collected[0]); // 1 + 2 + 3
}