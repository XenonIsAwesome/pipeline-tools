#include "string_utils.h"
#include <algorithm>

std::string pt::utils::stolower(const std::string& str) {
    std::string result(str.size(), '\0');
    std::ranges::transform(str, result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string pt::utils::stoupper(const std::string& str) {
    std::string result(str.size(), '\0');
    std::ranges::transform(str, result.begin(), [](const unsigned char c) {
        return std::toupper(c);
    });
    return result;
}
