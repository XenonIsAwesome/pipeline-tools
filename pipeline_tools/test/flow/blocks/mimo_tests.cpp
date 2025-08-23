#include <gtest/gtest.h>
#include <modules/NumberSource.h>
#include <modules/AddModule.h>
#include <modules/PrintSink.hpp>
#include <flow/blocks/Module.hpp>
#include <flow/blocks/Sink.hpp>
#include <flow/blocks/Source.hpp>
#include <flow/blocks/Aggregator.hpp>
#include <flow/blocks/Pipeline.hpp>
#include "TestBlocks.h"

// --- Tests ---

TEST(MiMoTests, SingleInputSingleProducer) {
    pt::flow::Pipeline p("p");
    p.add(std::make_shared<pt::modules::NumberSource>(1));
    p.add(std::make_shared<pt::modules::AddModule>(1));
    auto sink = p.add(std::make_shared<TestSink>());

    p.execute();
    EXPECT_EQ((std::vector<int>{2}), sink->collected);
}

TEST(MiMoTests, SingleInputManyProducers) {
    pt::flow::Pipeline p("p");

    p.add(std::make_shared<pt::modules::NumberSource>(1));
    auto src2 = p.add(std::make_shared<pt::modules::NumberSource>(4));
    auto mod = p.add(std::make_shared<pt::modules::AddModule>(10));
    auto sink = p.add(std::make_shared<TestSink>());
    pt::flow::Pipeline::connect(src2, mod); // manually connect into same module chain

    p.execute();

    EXPECT_EQ((std::vector<int>{11,14}), sink->collected);
}

TEST(MiMoTests, ManyInputsSingleProducer) {
    class MultiSource : public pt::flow::Source<std::vector<int>> {
    public:
        MultiSource(): Source("MultiSource", pt::flow::ProductionPolicy::SingleOutput) {}
        std::optional<std::vector<int>> process() override {
            return std::vector<int>{1,2,3};
        }
    };

    class CollectSink : public pt::flow::Sink<std::vector<int>> {
    public:
        CollectSink() : Sink<std::vector<int>>("CollectSink") {}
        void process(const std::vector<int>& input) override { collected = input; }
        std::vector<int> collected;
    };

    pt::flow::Pipeline p("p");

    p.add(std::make_shared<MultiSource>());
    auto sink = p.add(std::make_shared<CollectSink>());

    p.execute();
    EXPECT_EQ((std::vector<int>{1,2,3}), sink->collected);
}

TEST(MiMoTests, ManyInputsManyProducersAggregator) {
    pt::flow::Pipeline p("p");
    auto src1 = p.add(std::make_shared<pt::modules::NumberSource>(1));
    auto src2 = p.add(std::make_shared<pt::modules::NumberSource>(2));
    auto agg = p.add(std::make_shared<TestAggregator>(pt::flow::AggregationPolicy::Synchronized));
    pt::flow::Pipeline::connect(src2, agg);

    auto sink = p.add(std::make_shared<TestSink>());

    p.execute();
    ASSERT_EQ(1u, sink->collected.size());
    EXPECT_EQ(3, sink->collected[0]);
}

TEST(MiMoTests, SingleOutputManyConsumers) {
    pt::flow::Pipeline p("p");
    auto src = p.add(std::make_shared<pt::modules::NumberSource>(5));
    auto mod = p.add(std::make_shared<pt::modules::AddModule>(1));

    auto sink1 = p.add(std::make_shared<TestSink>());
    auto sink2 = std::make_shared<TestSink>();
    auto sink3 = std::make_shared<TestSink>();
    pt::flow::Pipeline::connect(mod, sink2);
    pt::flow::Pipeline::connect(mod, sink3);

    p.execute();

    EXPECT_EQ(1u, sink1->collected.size());
    EXPECT_EQ(1u, sink2->collected.size());
    EXPECT_EQ(1u, sink3->collected.size());
    EXPECT_EQ(6, sink1->collected[0]);
    EXPECT_EQ(6, sink2->collected[0]);
    EXPECT_EQ(6, sink3->collected[0]);
}

TEST(MiMoTests, ManyOutputsSingleConsumer) {
    class MultiOutModule : public pt::flow::Module<int,std::vector<int>> {
    public:
        MultiOutModule(): Module("MultiOutModule", pt::flow::ProductionPolicy::SingleOutput) {}
        std::optional<std::vector<int>> process(const int& input) override {
            return std::vector<int>{input, input+1, input+2};
        }
    };

    class CollectSink : public pt::flow::Sink<std::vector<int>> {
    public:
        CollectSink(): Sink<std::vector<int>>("CollectSink") {}
        void process(const std::vector<int>& input) override { collected = input; }
        std::vector<int> collected;
    };

    pt::flow::Pipeline p("p");
    p.add(std::make_shared<pt::modules::NumberSource>(10));
    p.add(std::make_shared<MultiOutModule>());
    auto sink = p.add(std::make_shared<CollectSink>());

    p.execute();
    EXPECT_EQ((std::vector<int>{10,11,12}), sink->collected);
}

TEST(MiMoTests, ManyOutputsManyConsumers) {
    class FanoutModule : public pt::flow::Module<int,std::vector<std::any>> {
    public:
        FanoutModule(): Module("FanoutModule", pt::flow::ProductionPolicy::ManyOutputs) {}
        std::optional<std::vector<std::any>> process(const int& input) override {
            return std::vector<std::any>{input, input+1};
        }
    };

    pt::flow::Pipeline p("p");
    p.add(std::make_shared<pt::modules::NumberSource>(7));
    auto mod = p.add(std::make_shared<FanoutModule>());

    auto sink1 = p.add(std::make_shared<TestSink>());
    auto sink2 = std::make_shared<TestSink>();
    pt::flow::Pipeline::connect(mod, sink2);

    p.execute();

    EXPECT_EQ(7, sink1->collected[0]);
    EXPECT_EQ(8, sink2->collected[0]);
}
