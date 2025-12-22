#pragma once

#include <string>

namespace pt::utils {
/**
 * Converts a string to upper-case
 * @param str The input string
 * @return The same string upper-case
 */
std::string stoupper(const std::string& str);

/**
 * Converts a string to lower-case
 * @param str The input string
 * @return The same string but lower-case
 */
std::string stolower(const std::string& str);
}  // namespace pt::server::utils