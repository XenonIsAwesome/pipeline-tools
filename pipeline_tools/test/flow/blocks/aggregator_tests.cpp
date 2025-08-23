#include <gtest/gtest.h>
#include <flow/blocks/Aggregator.hpp>
#include <modules/NumberSource.h>
#include <flow/blocks/Pipeline.hpp>

#include "flow/blocks/Sink.hpp"

class SumAgg : public pt::flow::Aggregator<int> {
public:
    explicit SumAgg(pt::flow::AggregationPolicy pol)
        : Aggregator<int>("SumAgg", pol) {}
    std::optional<int> process(const std::vector<int>& inputs) override {
        int sum = 0;
        for (auto v : inputs) sum += v;
        return sum;
    }
};

class CollectSink : public pt::flow::Sink<int> {
public:
    CollectSink() : Sink<int>("CollectSink") {}
    void process(const int& input) override { collected.push_back(input); }
    std::vector<int> collected;
};

TEST(AggregatorTests, NoAggregationPassThrough) {
    pt::flow::Pipeline p("p");
    auto src = p.add(std::make_shared<pt::modules::NumberSource>(5));
    auto agg = p.add(std::make_shared<SumAgg>(pt::flow::AggregationPolicy::NoAggregation));
    auto sink = p.add(std::make_shared<CollectSink>());
    p.execute();

    ASSERT_EQ(1u, sink->collected.size());
    EXPECT_EQ(5, sink->collected[0]);
}

TEST(AggregatorTests, SynchronizedAggregation) {
    pt::flow::Pipeline p("p");
    auto src1 = p.add(std::make_shared<pt::modules::NumberSource>(2));
    auto src2 = std::make_shared<pt::modules::NumberSource>(3);
    auto agg = p.add(std::make_shared<SumAgg>(pt::flow::AggregationPolicy::Synchronized));
    pt::flow::Pipeline::connect(src2, agg);

    auto sink = p.add(std::make_shared<CollectSink>());
    p.execute();
    src2->execute();

    ASSERT_EQ(1u, sink->collected.size());
    EXPECT_EQ(5, sink->collected[0]);
}

TEST(AggregatorTests, ContinuousAggregation) {
    pt::flow::Pipeline p("p");
    auto src1 = p.add(std::make_shared<pt::modules::NumberSource>(1));
    auto src2 = std::make_shared<pt::modules::NumberSource>(2);
    auto agg = p.add(std::make_shared<SumAgg>(pt::flow::AggregationPolicy::Continuous));
    pt::flow::Pipeline::connect(src2, agg);

    auto sink = p.add(std::make_shared<CollectSink>());
    p.execute();
    src2->execute();

    ASSERT_GE(sink->collected.size(), 1u);
    EXPECT_TRUE(std::find(sink->collected.begin(), sink->collected.end(), 3) != sink->collected.end());
}
