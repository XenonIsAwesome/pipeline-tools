#pragma once

#include <stdexcept>
#include <string>
#include <typeinfo>

namespace pt::utils::exceptions {
    class unknown_flow_object_type : public std::runtime_error {
    public:
        explicit unknown_flow_object_type(const std::type_info &type)
            : std::runtime_error(build_message(type)) {
        }

    private:
        static std::string build_message(const std::type_info &type);
    };
} // namespace pt::flow
