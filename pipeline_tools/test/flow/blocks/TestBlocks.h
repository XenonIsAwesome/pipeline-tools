#include <flow/blocks/Aggregator.hpp>
#include <flow/blocks/Sink.hpp>

class TestSink : public pt::flow::Sink<int> {
public:
    TestSink() : Sink<int>("TestSink") {}
    void process(const int& input) override { collected.push_back(input); }
    std::vector<int> collected;
};

class TestAggregator : public pt::flow::Aggregator<int> {
public:
    explicit TestAggregator(pt::flow::AggregationPolicy pol)
        : Aggregator<int>("TestAggregator", pol) {}
    std::optional<int> process(const std::vector<int>& inputs) override {
        int sum = 0;
        for (auto v : inputs) sum += v;
        return sum;
    }
};