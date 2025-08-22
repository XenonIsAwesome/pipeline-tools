#pragma once

#include <flow/blocks/Module.hpp>

#include <optional>

namespace pt::modules {
    // TODO(xenon): add docs
    class AddModule : public flow::Module<int, int> {
    public:
        // TODO(xenon): add docs if needed
        explicit AddModule(const int addition, flow::ProductionPolicy policy = flow::ProductionPolicy::SingleOutputManyConsumers):
            Module("AddModule", policy), addition(addition) {}
        
        // TODO(xenon): add docs if needed
        int process(const int&) override;

    private:
        int addition;
    };
} // namespace pt::modules
