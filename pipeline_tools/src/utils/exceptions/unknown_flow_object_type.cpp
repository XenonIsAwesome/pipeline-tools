#include <utils/exceptions/unknown_flow_object_type.h>
#include <utils/utils.h>

std::string pt::utils::exceptions::unknown_flow_object_type::build_message(const std::type_info &type) {
    return "Unknown flow object type: " + demangle(type.name());
}
