#include <flow/exceptions/unknown_flow_object.h>
#include <utils/misc_utils.h>

std::string
pt::flow::exceptions::unknown_flow_object::build_msg(const std::type_info& flow_obj) {
    return "unknown_flow_object: " + utils::demangle(flow_obj.name());
}
