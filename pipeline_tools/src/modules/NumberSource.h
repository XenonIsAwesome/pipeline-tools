#pragma once

#include <flow/blocks/Source.hpp>

#include <optional>

namespace pt::modules {
    // TODO: add docs
    class NumberSource final: public flow::Source<int> {
    public:
        // TODO: add docs if needed
        explicit NumberSource(const int number):
            Source("NumberSource", flow::ProductionPolicy::SingleOutput), num(number) {}
        
        // TODO: add docs if needed
        std::optional<int> process() override;

    private:
        int num;
    };
} // namespace pt::modules
