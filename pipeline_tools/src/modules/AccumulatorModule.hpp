#pragma once
#include <vector>
#include <array>
#include <optional>

#include "flow/blocks/Module.hpp"

namespace pt::modules {
    // TODO(xenon): add docs
    template<typename T>
    class AccumulatorModule: public flow::Module<T, std::vector<T>> {
    public:
        // TODO(xenon): add docs if needed
        AccumulatorModule(size_t expected_inputs):
            flow::Module<T, std::vector<T>>("AccumulatorModule"), expected_inputs(expected_inputs) {}
        
        // TODO(xenon): add docs if needed
        std::optional<std::vector<T>> process(const T& input) override;
    private:
        std::vector<T> accumulator;
        size_t expected_inputs;
    };

    template<typename T>
    std::optional<std::vector<T>> AccumulatorModule<T>::process(const T& input) {
        accumulator.emplace_back(std::move(input));
        if (accumulator.size() == expected_inputs) {
            auto retval = std::move(accumulator);
            accumulator = {};
            accumulator.reserve(expected_inputs);
            return retval;
        }
        return std::nullopt;
    }

} // namespace pt::modules
