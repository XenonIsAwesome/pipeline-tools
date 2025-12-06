#pragma once
#include <optional>
#include <string>
#include "convert.h"

namespace pt::config {
template<typename T>
std::optional<T> get_env(const std::string &key) {
    char* value = getenv(key.c_str());
    if (value == nullptr) {
        return std::nullopt;
    }

    return convert<T>(value);
}

template<typename T>
T get_env(const std::string &key, T default_value) {
    auto value = get_env<T>(key);
    if (!value.has_value()) {
        return default_value;
    }
    return value.value();
}
}