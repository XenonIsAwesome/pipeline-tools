#pragma once
#include <optional>
#include <stdexcept>
#include <string>

namespace pt::config {
template<typename T>
T convert(const std::string& value) {
    if constexpr (std::is_same_v<T, bool>) {
        return value == "true";
    } else if constexpr (std::convertible_to<T, std::string>) {
        return value;
    } else if constexpr (std::is_floating_point_v<T>) {
        return std::stod(value);
    } else if constexpr (std::is_signed_v<T>) {
        return std::stoll(value);
    } else if constexpr (std::is_unsigned_v<T>) {
        return std::stoull(value);
    } else {
        throw std::runtime_error("Unsupported type " + std::string(typeid(T).name()));
    }
}
}