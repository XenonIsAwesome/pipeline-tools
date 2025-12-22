#pragma once

#include "convert.h"

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>

namespace pt::config {
template<typename OutT>
std::optional<OutT> get_env(const std::string& key) {
    const char* value = getenv(key.c_str());
    if (value == nullptr) {
        return std::nullopt;
    }

    return convert<OutT>(value);
}

template<typename OutT>
OutT get_env(const std::string& key, OutT default_val) {
    auto value = get_env<OutT>(key);
    if (value.has_value()) {
        return value.value();
    }
    return default_val;
}
}  // namespace pt::config