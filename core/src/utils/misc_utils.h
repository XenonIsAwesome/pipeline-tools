#pragma once

#include <cstdint>
#include <fstream>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace pt::utils {

/**
 * Parses numeric ranges (e.g. 1-5,9-18) to a vector of numbers
 * @param input Input numeric range as a string
 * @return The parsed range as a vector of ints
 */
std::vector<int> parse_numeric_ranges(const std::string& input);

/**
 * Demangles the cpp object name, used with std::type_info objects.
 *
 * @note Due to restrictions, on some systems / compilers the name might still be mangled
 *
 * @param name Original name
 * @return Demangled name
 */
std::string demangle(const std::string& name);

};  // namespace pt::utils
