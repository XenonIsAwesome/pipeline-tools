#pragma once

#include <stdexcept>

namespace pt::flow::exceptions {
class unknown_flow_object : public std::runtime_error {
public:
    explicit unknown_flow_object(const std::type_info& flow_obj) : runtime_error(build_msg(flow_obj)) {}

private:
    static std::string build_msg(const std::type_info& flow_obj);
};
}  // namespace pt::flow::exceptions
