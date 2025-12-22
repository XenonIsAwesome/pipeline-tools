#include "../flow_test_utils.hpp"

#include <cmath>
#include <flow/Pipeline.hpp>
#include <gtest/gtest.h>
#include <modules/io/ConstantSource.hpp>
#include <modules/math/AdditionModule.hpp>

TEST(ModuleTests, ProcessAnyInvokesProcess) {
    pt::modules::AdditionModule<int, int, int> mod(420);

    /// `process_any` should unwrap -> call process -> wrap back into nstd::any
    auto out_any = mod.process_any(69, 0);

    ASSERT_TRUE(out_any.has_value());
    EXPECT_EQ(69 + 420, nstd::any_cast<int>(out_any));
}

TEST(ModuleTests, ProcessAnyHandlesEmptyOptional) {
    /// A module that always returns std::nullopt
    class NullModule : public pt::flow::Module<int, int> {
    public:
        std::optional<int> process(int) override {
            return std::nullopt;
        }
    };

    NullModule mod;

    auto out_any = mod.process_any(69, 0);

    /// Expect 'empty' any
    EXPECT_TRUE(out_any.has_value());
    EXPECT_EQ(out_any.type(), typeid(std::nullopt_t));
}

TEST(ModuleTests, ModuleWorksInsidePipeline) {
    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::ConstantSource<int>>(69));
    p.add(std::make_shared<pt::modules::AdditionModule<int, int, int>>(420));
    auto sink = p.add(std::make_shared<MockSink<int>>());

    p.execute();

    ASSERT_EQ(1, sink->collected.size());
    EXPECT_EQ(420 + 69, sink->collected[0]);
}

TEST(ModuleTests, ModuleChainMultiple) {
    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::ConstantSource<float>>(4.20));
    p.add(std::make_shared<pt::modules::AdditionModule<float, float, float>>(6.9));
    p.add(std::make_shared<pt::modules::AdditionModule<float, float, float>>(0.69));
    auto sink = p.add(std::make_shared<MockSink<float>>());

    p.execute();

    ASSERT_EQ(1, sink->collected.size());
    EXPECT_EQ(std::ceil(4.20 + 6.9 + 0.69), std::ceil(sink->collected[0]));
}
