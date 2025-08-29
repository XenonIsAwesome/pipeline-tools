#pragma once

#include <flow/blocks/Aggregator.hpp>
#include <modules/math/concepts.h>

namespace pt::modules {
    template<typename In, typename Out>
        requires concepts::Addable<In, In, Out>
    class SumAggregator : public flow::Aggregator<In, Out> {
    public:
        SumAggregator(Out default_value): flow::Aggregator<In, Out>() {
        }

        std::optional<Out> process(std::vector<In> inputs) {
            Out sum = default_value;
            for (In input: inputs) {
                sum += input;
            }

            return std::move(sum);
        }

    private:
        Out default_value;
    };
}
