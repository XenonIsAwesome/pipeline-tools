#pragma once

#include <flow/blocks/Module.hpp>

#include <optional>

namespace pt::modules {
    // TODO: add docs
    class AddModule : public flow::Module<int, int> {
    public:
        // TODO: add docs if needed
        explicit AddModule(const int addition):
            Module("AddModule", flow::ProductionPolicy::SingleOutput), addition(addition) {}
        
        // TODO: add docs if needed
        std::optional<int> process(const int&) override;

    private:
        int addition;
    };
} // namespace pt::modules
