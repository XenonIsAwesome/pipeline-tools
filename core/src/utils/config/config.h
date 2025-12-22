#pragma once

#include "env.h"

namespace pt::config {
/**
 * Gets a config value based on a key
 * @tparam OutT Type of the output
 * @param key Key of the config we want to get
 * @return The value of the config we requested
 */
template<typename OutT = std::string>
std::optional<OutT> get(const std::string& key) {
    return get_env<OutT>(key);
}

/**
 * Gets a config value based on a key (or a default)
 * @tparam OutT Type of the output
 * @param key Key of the config we want to get
 * @param default_val Default value in-case theres no value
 * @return The value of the config we requested, or the default.
 */
template<typename OutT = std::string>
OutT get(const std::string& key, OutT default_val) {
    return get_env(key, default_val);
}


}  // namespace pt::config