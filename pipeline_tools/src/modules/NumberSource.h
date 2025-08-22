#pragma once

#include <flow/blocks/Source.hpp>

#include <optional>

namespace pt::modules {
    // TODO(xenon): add docs
    class NumberSource final: public flow::Source<int> {
    public:
        // TODO(xenon): add docs if needed
        explicit NumberSource(const int number, flow::ProductionPolicy policy = flow::ProductionPolicy::SingleOutputManyConsumers):
            Source("NumberSource", policy), num(number) {}
        
        // TODO(xenon): add docs if needed
        int process() override;

    private:
        int num;
    };
} // namespace pt::modules
