#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <dawg-log/logger.hpp>

namespace pt::config {

template<typename OutT>
OutT convert(const char* value) {
    OutT converted_value;
    if constexpr (std::is_same_v<OutT, std::string>) {
        converted_value = std::string(value);
    } else if constexpr (std::is_floating_point_v<OutT>) {
        converted_value = static_cast<OutT>(std::stod(value));
    } else if constexpr (std::is_unsigned_v<OutT>) {
        converted_value = static_cast<OutT>(std::stoull(value));
    } else if constexpr (std::is_signed_v<OutT>) {
        converted_value = static_cast<OutT>(std::stoll(value));
    } else {
        DawgLog::throw_error<std::runtime_error>(LOG_SRC,
            "pt::config doesnt support the type: {}", typeid(OutT).name());
    }

    return converted_value;
}
}  // namespace pt::config