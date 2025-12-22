#pragma once

#include <flow/blocks/Aggregator.hpp>
#include <modules/math/concepts.h>

namespace pt::modules {

template<typename In, typename Out>
    requires Addable<In, In, Out>
class SumAggregator : public flow::Aggregator<In, Out> {
public:
    std::optional<int> process(std::vector<int> inputs) override {
        int sum = 0;
        for (auto v : inputs) {
            sum += v;
        }
        return sum;
    }
};

}  // namespace pt::modules
