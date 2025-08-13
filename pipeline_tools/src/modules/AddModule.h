#pragma once

#include <flow/blocks/Module.hpp>

#include <optional>

namespace pt::modules {
    // TODO(xenon): add docs
    class AddModule final : public flow::blocks::Module<int, int> {
    public:
        // TODO(xenon): add docs if needed
        explicit AddModule(const int addition):
            Module("AddModule"), addition(addition) {}
        
        // TODO(xenon): add docs if needed
        std::optional<int> process(const int&) override;

    private:
        int addition;
    };
} // namespace pt::modules
