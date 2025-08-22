#pragma once

#include <flow/blocks/Aggregator.hpp>

template<typename T>
concept Summable = requires(T t1, T t2)
{
    t1 += t2;
};

template<Summable T>
class SumAggregator : public pt::flow::Aggregator<int> {
public:
    SumAggregator(pt::flow::AggregationPolicy policy)
        : Aggregator("SumAggregator", policy) {}

    std::optional<T> process(const std::vector<T>& inputs) override {
        T sum;
        for (auto v: inputs)
            sum += v;
        return sum;
    }
};
