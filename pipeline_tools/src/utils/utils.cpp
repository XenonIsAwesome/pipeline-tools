#include <utils/utils.h>
#include <cxxabi.h>
#include <sstream>
#include <stdexcept>

std::string pt::utils::demangle(const char *name) {
#if defined(__GNUG__)
    int status = 0;
    char *demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    std::string out = (status == 0 && demangled) ? demangled : name;
    std::free(demangled);
    return out;
#else
    return name; // fallback: raw typeid name
#endif
}

std::vector<int> pt::utils::parse_numeric_range(const std::string& range){
    std::vector<int> result;
    std::stringstream ss(range);
    std::string token;

    while (std::getline(ss, token, ',')) {
        // Trim whitespace
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);

        if (token.empty()) continue;

        auto dash = token.find('-');
        if (dash != std::string::npos) {
            // Range case: "a-b"
            int start = std::stoi(token.substr(0, dash));
            int end   = std::stoi(token.substr(dash + 1));

            if (end < start) {
                throw std::invalid_argument("Invalid range: " + token);
            }

            for (int i = start; i <= end; ++i) {
                result.push_back(i);
            }
        } else {
            // Single number
            result.push_back(std::stoi(token));
        }
    }

    return result;
}
