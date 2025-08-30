#pragma once

#include <flow/blocks/Source.hpp>

namespace pt::modules {
    template<typename Out>
    class ConstantSource final : public flow::Source<Out> {
    public:
        ConstantSource(Out value): flow::Source<Out>(), value(value) {
        }

        std::optional<Out> process() {
            return value;
        }

    private:
        Out value;
    };
}
