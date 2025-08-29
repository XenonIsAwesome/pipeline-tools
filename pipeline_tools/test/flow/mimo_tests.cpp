#include <gtest/gtest.h>
#include <flow/blocks/Module.hpp>
#include <flow/blocks/Sink.hpp>
#include <flow/blocks/Source.hpp>
#include <flow/blocks/Aggregator.hpp>
#include "flow_test_utils.hpp"
#include <flow/Pipeline.hpp>
#include <modules/io/ConstantSource.hpp>
#include <modules/math/AdditionModule.hpp>
#include <modules/math/SumAggregator.hpp>

// --- Tests ---

TEST(MiMoTests, SingleInputSingleProducer) {
    pt::flow::Pipeline p;
    p.add(std::make_shared<pt::modules::ConstantSource<int> >(1));
    p.add(std::make_shared<pt::modules::AdditionModule<int, int, int> >(1));
    auto sink = p.add(std::make_shared<MockSink>());

    p.execute();
    EXPECT_EQ((std::vector{2}), sink->collected);
}

TEST(MiMoTests, SingleInputManyProducers) {
    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::ConstantSource<int> >(1));
    auto src2 = p.add(std::make_shared<pt::modules::ConstantSource<int> >(4));
    auto mod = p.add(std::make_shared<pt::modules::AdditionModule<int, int, int> >(10));
    auto sink = p.add(std::make_shared<MockSink>());
    pt::flow::Pipeline::connect(src2, mod); // manually connect into same module chain

    p.execute();

    EXPECT_EQ((std::vector<int>{11,14}), sink->collected);
}

TEST(MiMoTests, ManyInputsSingleProducer) {
    class MultiSource : public pt::flow::Source<std::vector<int> > {
    public:
        std::optional<std::vector<int> > process() override {
            return std::vector{1, 2, 3};
        }
    };

    pt::flow::Pipeline p;

    p.add(std::make_shared<MultiSource>());
    auto sink = p.add(std::make_shared<CollectSink>());

    p.execute();
    EXPECT_EQ((std::vector{1,2,3}), sink->collected);
}

TEST(MiMoTests, ManyInputsManyProducersAggregator) {
    pt::flow::Pipeline p;
    auto src1 = p.add(std::make_shared<pt::modules::ConstantSource<int> >(1));
    auto src2 = p.add(std::make_shared<pt::modules::ConstantSource<int> >(2));
    auto agg = p.add(std::make_shared<pt::modules::SumAggregator<int, int> >(0));
    pt::flow::Pipeline::connect(src2, agg);

    auto sink = p.add(std::make_shared<MockSink>());

    p.execute();
    ASSERT_EQ(1, sink->collected.size());
    EXPECT_EQ(3, sink->collected[0]);
}

TEST(MiMoTests, SingleOutputManyConsumers) {
    pt::flow::Pipeline p;
    auto src = p.add(std::make_shared<pt::modules::ConstantSource<int> >(5));
    auto mod = p.add(std::make_shared<pt::modules::AdditionModule<int, int, int> >(1));

    auto sink1 = p.add(std::make_shared<MockSink>());
    auto sink2 = std::make_shared<MockSink>();
    auto sink3 = std::make_shared<MockSink>();
    pt::flow::Pipeline::connect(mod, sink2);
    pt::flow::Pipeline::connect(mod, sink3);

    p.execute();

    EXPECT_EQ(1, sink1->collected.size());
    EXPECT_EQ(1, sink2->collected.size());
    EXPECT_EQ(1, sink3->collected.size());
    EXPECT_EQ(6, sink1->collected[0]);
    EXPECT_EQ(6, sink2->collected[0]);
    EXPECT_EQ(6, sink3->collected[0]);
}

TEST(MiMoTests, ManyOutputsSingleConsumer) {
    class MultiOutModule : public pt::flow::Module<int, std::vector<int> > {
    public:
        std::optional<std::vector<int> > process(int input) override {
            return std::vector{input, input + 1, input + 2};
        }
    };

    pt::flow::Pipeline p;
    p.add(std::make_shared<pt::modules::ConstantSource<int> >(10));
    p.add(std::make_shared<MultiOutModule>());
    auto sink = p.add(std::make_shared<CollectSink>());

    p.execute();
    EXPECT_EQ((std::vector{10,11,12}), sink->collected);
}

TEST(MiMoTests, ManyOutputsManyConsumers) {
    class RoundRobinModule : public pt::flow::Module<int, std::vector<int> > {
    public:
        RoundRobinModule(): Module(pt::flow::ProductionPolicy::RoundRobin) {
        }

        std::optional<std::vector<int> > process(int input) override {
            return std::vector{input, input + 1};
        }
    };

    pt::flow::Pipeline p;
    p.add(std::make_shared<pt::modules::ConstantSource<int> >(7));
    auto mod = p.add(std::make_shared<RoundRobinModule>());
    auto sink1 = p.add(std::make_shared<MockSink>());
    auto sink2 = std::make_shared<MockSink>();
    pt::flow::Pipeline::connect(mod, sink2);

    p.execute();

    ASSERT_EQ(1, sink1->collected.size());
    ASSERT_EQ(1, sink2->collected.size());
    EXPECT_EQ(7, sink1->collected[0]);
    EXPECT_EQ(8, sink2->collected[0]);
}
