#pragma once

#include <string>
#include <vector>

namespace pt::utils {
    /**
     * portable-ish demangle helper (does nothing by default)
     * @param name The name of the type
     * @return The (hopefully) demangled name
     */
    std::string demangle(const char *name);

    /**
     * TODO: add docs
     * @param range
     * @return
     */
    std::vector<int> parse_numeric_range(const std::string& range);
}
