#pragma once

#include <stdexcept>

namespace pt::flow::exceptions {
class unordered_sources : public std::runtime_error {
public:
    unordered_sources() : runtime_error(build_msg()) {}

private:
    static std::string build_msg();
};
}  // namespace pt::flow::exceptions
