#pragma once

#include <string>

namespace pt::utils {
    /**
     * portable-ish demangle helper (does nothing by default)
     * @param name The name of the type
     * @return The (hopefully) demangled name
     */
    std::string demangle(const char *name);
}
