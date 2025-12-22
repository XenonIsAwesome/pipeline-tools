#include "flow_test_utils.hpp"

#include <flow/Pipeline.hpp>
#include <gtest/gtest.h>
#include <modules/io/ConstantSource.hpp>
#include <modules/math/AdditionModule.hpp>
#include <modules/math/SumAggregator.hpp>

TEST(MiMoTests, SingleInputSingleProducer_SingleOutputSingleConsumer) {
    pt::flow::Pipeline p{};

    p.add(std::make_shared<pt::modules::ConstantSource<int>>(420));
    p.add(std::make_shared<pt::modules::AdditionModule<int, float, float>>(0.69));
    auto sink = p.add(std::make_shared<MockSink<float>>());

    p.execute();
    EXPECT_EQ((std::vector<float>{420.69}), sink->collected);
}

TEST(MiMoTests, SingleInputManyProducers_SIMPTest) {
    pt::flow::Pipeline p{};

    p.add(std::make_shared<pt::modules::ConstantSource<int>>(420));
    auto src2 = p.add(std::make_shared<pt::modules::ConstantSource<int>>(69));
    auto mod = p.add(std::make_shared<pt::modules::AdditionModule<int, float, float>>(0.69));
    auto sink = p.add(std::make_shared<MockSink<float>>());

    p.execute();

    EXPECT_EQ((std::vector<float>{420.69, 69.69}), sink->collected);
}

TEST(MiMoTests, ManyInputsSingleProducer) {
    class MultiSource : public pt::flow::Source<std::vector<int>> {
    public:
        std::optional<std::vector<int>> process() override {
            return std::vector{1, 2, 3};
        }
    };

    class CollectSink : public pt::flow::Sink<std::vector<int>> {
    public:
        void process(std::vector<int> input) override {
            collected = input;
        }
        std::vector<int> collected;
    };


    pt::flow::Pipeline p{};

    p.add(std::make_shared<MultiSource>());
    auto sink = p.add(std::make_shared<CollectSink>());

    p.execute();

    EXPECT_EQ((std::vector{1, 2, 3}), sink->collected);
}

TEST(MiMoTests, ManyInputsManyProducers) {
    pt::flow::Pipeline p{};

    p.add(std::make_shared<pt::modules::ConstantSource<int>>(420));
    auto src2 = p.add(std::make_shared<pt::modules::ConstantSource<int>>(69));
    auto agg = p.add(std::make_shared<pt::modules::SumAggregator<int, int>>());
    auto sink = p.add(std::make_shared<MockSink<int>>());

    p.execute();

    ASSERT_EQ(1, sink->collected.size());
    EXPECT_EQ(420 + 69, sink->collected[0]);
}

TEST(MiMoTests, SingleOutputManyConsumers) {
    pt::flow::Pipeline p{};

    p.add(std::make_shared<pt::modules::ConstantSource<int>>(420));
    auto mod = p.add(std::make_shared<pt::modules::AdditionModule<int, int, int>>(69));

    auto sink1 = p.add(std::make_shared<MockSink<int>>());
    auto sink2 = std::make_shared<MockSink<int>>();
    connect(mod, sink2);

    auto sink3 = std::make_shared<MockSink<int>>();
    connect(mod, sink3);

    p.execute();

    ASSERT_EQ(1, sink1->collected.size());
    ASSERT_EQ(1, sink2->collected.size());
    ASSERT_EQ(1, sink3->collected.size());

    EXPECT_EQ(420 + 69, sink1->collected[0]);
    EXPECT_EQ(420 + 69, sink2->collected[0]);
    EXPECT_EQ(420 + 69, sink3->collected[0]);
}

TEST(MiMoTests, ManyOutputsSingleConsumer) {
    class MultiOutModule : public pt::flow::Module<int, std::vector<int>> {
    public:
        std::optional<std::vector<int>> process(int input) override {
            return std::vector{input, input + 1, input + 2};
        }
    };

    class CollectSink : public pt::flow::Sink<std::vector<int>> {
    public:
        void process(std::vector<int> input) override {
            collected = input;
        }
        std::vector<int> collected;
    };

    pt::flow::Pipeline p{};
    p.add(std::make_shared<pt::modules::ConstantSource<int>>(69));
    p.add(std::make_shared<MultiOutModule>());
    auto sink = p.add(std::make_shared<CollectSink>());

    p.execute();

    EXPECT_EQ((std::vector<int>{69, 70, 71}), sink->collected);
}

TEST(MiMoTests, ManyOutputsManyConsumers) {
    class FanoutModule : public pt::flow::Module<int, std::vector<nstd::any>> {
    public:
        FanoutModule() : Module(pt::flow::ProductionPolicy::RoundRobin) {}

        std::optional<std::vector<nstd::any>> process(int input) override {
            return std::vector<nstd::any>{input, input + 1};
        }
    };

    pt::flow::Pipeline p{};
    p.add(std::make_shared<pt::modules::ConstantSource<int>>(69));
    auto mod = p.add(std::make_shared<FanoutModule>());
    auto sink1 = p.add(std::make_shared<MockSink<int>>());
    auto sink2 = std::make_shared<MockSink<int>>();
    connect(mod, sink2);

    p.execute();

    ASSERT_EQ(1, sink1->collected.size());
    EXPECT_EQ(69, sink1->collected[0]);

    ASSERT_EQ(1, sink2->collected.size());
    EXPECT_EQ(70, sink2->collected[0]);
}

class MiMoDataTests: public ::testing::Test {
public:
    MiMoDataTests() {}

protected:
    void SetUp() override {
        Tracer::reset();
    }

    void TearDown() override {
        Tracer::reset();
    }
};

TEST_F(MiMoDataTests, FanoutCopiesToAllButMovesToLast) {
    pt::flow::Pipeline p;

    p.add(std::make_shared<TracerSource>());
    auto sink1 = p.add(std::make_shared<TracerSink>());
    auto sink2 = p.add(std::make_shared<TracerSink>());

    p.execute();

    // Both sinks should see value 42
    ASSERT_EQ(1, sink1->collected.size());
    EXPECT_EQ(42, sink1->collected[0]);
    ASSERT_EQ(1, sink2->collected.size());
    EXPECT_EQ(42, sink2->collected[0]);

    // Expect: ctor (in process) + copy (to sink1) + move (to sink2) + dtor's
    EXPECT_NE(std::find(Tracer::events.begin(), Tracer::events.end(), "copy"), Tracer::events.end());
    EXPECT_NE(std::find(Tracer::events.begin(), Tracer::events.end(), "move"), Tracer::events.end());
}


TEST_F(MiMoDataTests, RoundRobinMovesWithoutCopies) {
    class RoundRobinSource: public pt::flow::Source<std::vector<Tracer>> {
    public:
        RoundRobinSource(): Source(pt::flow::ProductionPolicy::RoundRobin) {}

        std::optional<std::vector<Tracer>> process() override {
            std::vector<Tracer> v;
            v.reserve(2); // avoid reallocs
            v.emplace_back(1);
            v.emplace_back(2);

            return v;
        }
    };

    pt::flow::Pipeline p;

    p.add(std::make_shared<RoundRobinSource>());
    auto sink1 = p.add(std::make_shared<TracerSink>());
    auto sink2 = p.add(std::make_shared<TracerSink>());

    p.execute();

    ASSERT_EQ(1, sink1->collected.size());
    ASSERT_EQ(1, sink2->collected.size());

    // Should only see moves from vector into sinks
    EXPECT_EQ(std::count(Tracer::events.begin(), Tracer::events.end(), "copy"), 0);
    EXPECT_GT(std::count(Tracer::events.begin(), Tracer::events.end(), "move"), 0);
}


TEST_F(MiMoDataTests, DtorCalledAfterPipelineScope) {
    {
        pt::flow::Pipeline p;

        p.add(std::make_shared<TracerSource>());
        p.add(std::make_shared<TracerSink>());

        p.execute();
    }

    // After pipeline destruction, at least one dtor should be called.
    EXPECT_NE(std::find(Tracer::events.begin(), Tracer::events.end(), "dtor"), Tracer::events.end());
}

