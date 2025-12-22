#pragma once
#include <flow/blocks/Source.hpp>

namespace pt::modules {
/**
 * Source that outputs a constant value each iteration
 * @tparam T Type of the constant value
 */
template<typename T>
class ConstantSource : public flow::Source<T> {
public:
    explicit ConstantSource(T constant) : flow::Source<T>(), constant(constant) {}

    std::optional<T> process() override {
        return constant;
    }

private:
    T constant;
};
}  // namespace pt::modules