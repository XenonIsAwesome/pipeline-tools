#include "misc_utils.h"

#include <cxxabi.h>
#include <dawg-log/logger.hpp>
#include <sstream>

std::string pt::utils::demangle(const std::string& name) {
    std::string demangled_name;
#if defined(__GNUC__)
    int status = 0;
    char* demangled = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status);
    demangled_name = (status == 0 && demangled) ? demangled : name;
    std::free(demangled);
#else
    demangled_name = name;  // backup, returns original
#endif
    return demangled_name;
}

std::vector<int> pt::utils::parse_numeric_ranges(const std::string& input) {
    std::vector<int> numerics;
    std::stringstream ss(input);
    std::string token;

    while (std::getline(ss, token, ',')) {
        size_t dash_pos = token.find('-');
        if (dash_pos == std::string::npos) {
            numerics.push_back(std::stoll(token));
        } else {
            int start = std::stoll(token.substr(0, dash_pos));
            int end = std::stoll(token.substr(dash_pos + 1));
            for (int i = start; i <= end; ++i) {
                numerics.push_back(i);
            }
        }
    }

    return numerics;
}