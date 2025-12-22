#pragma once
#include <flow/blocks/Module.hpp>
#include <modules/math/concepts.h>

namespace pt::modules {
/**
 * Adds a certain amount to the input.
 * @tparam In The input type to add to.
 * @tparam Add The type of the value we add.
 * @tparam Out The output type.
 */
template<typename In, typename Add, typename Out>
    requires Addable<In, Add, Out>
class AdditionModule : public flow::Module<In, Out> {
public:
    explicit AdditionModule(Add add) :
        flow::Module<In, Out>(flow::ProductionPolicy::Fanout), addition(add) {}

    std::optional<Out> process(In input) override {
        return input + addition;
    }

private:
    Add addition;
};
}  // namespace pt::modules