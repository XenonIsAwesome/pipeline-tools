#pragma once

#[[#include]]# <flow/blocks/${Block_type}.hpp>

#[[#include]]# <optional>

namespace pt::modules {
    // TODO(${USER}): add docs
    class ${NAME}: public flow::blocks::${Block_type}<${Input_type}, ${Output_type}> {
    public:
        // TODO(${USER}): add docs if needed
        ${NAME}(): ${Block_type}("${NAME}") {}
        
        // TODO(${USER}): add docs if needed
        std::optional<${Output_type}> process(const ${Input_type}&) override;
    };
} // namespace pt::modules
