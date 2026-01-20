<%
def format_name(name: str):
    mapping = {
        ' ': '_',
        '-': '_',
    }

    for k, v in mapping.items():
        name = name.replace(k, v)

    return name
%>
#include <server/routes.hpp>

namespace swg = pt::server::swagger;

namespace ${mission_namespace} {
[[maybe_unused]] static pt::server::RegisterEndpoint
    _register_${format_name(mission_name).lower()}([](httplib::Server& svr) {
        // TODO(${username}): Add routes
    });
}  // namespace ${mission_namespace}