#pragma once

#include <flow/blocks/Module.hpp>

#include "concepts.h"


namespace pt::modules {
    template<typename In, typename Add, typename Out>
    requires concepts::Addable<In, Add, Out>
    class AdditionModule final: public pt::flow::Module<In, Out> {
    public:
        AdditionModule(Add addition):
            pt::flow::Module<In, Out>(), addition(addition) {}

        std::optional<Out> process(In input) override {
            return input + addition;
        }
    private:
        Add addition;
    };
}
