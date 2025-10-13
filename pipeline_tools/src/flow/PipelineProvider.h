#pragma once

#include <flow/Pipeline.hpp>

namespace pt::flow {
    class InputPipelineProvider {
    public:
        virtual ~InputPipelineProvider() = default;

        virtual Pipeline build_input() = 0;
    };

    class OutputPipelineProvider {
    public:
        virtual ~OutputPipelineProvider() = default;

        virtual Pipeline build_output() = 0;
    };

    namespace concepts {
        template<typename T>
        concept PipelineProvider = std::derived_from<T, InputPipelineProvider> ||
            std::derived_from<T, OutputPipelineProvider> || std::is_same_v<T, void>;
    }
}