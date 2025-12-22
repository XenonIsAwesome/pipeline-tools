#include <server/routes.hpp>

namespace swg = pt::server::swagger;

namespace example {
[[maybe_unused]] static pt::server::RegisterEndpoint
    _register_example([](httplib::Server& svr) {
        DECLARE_ENDPOINT(
            svr,
            Get,
            "/api/missions/example",
            swg::EndpointMetadata("Example Mission Status").add_response(httplib::OK_200, "OK"),
            [](const httplib::Request& req, httplib::Response& res) {
                res.status = httplib::OK_200;
                res.set_content("true", "application/json");
            })
        DECLARE_ENDPOINT(
            svr,
            Post,
            "/api/missions/example",
            swg::EndpointMetadata("Start Example Mission").add_response(httplib::OK_200, "OK"),
            [](const httplib::Request& req, httplib::Response& res) {
                res.status = httplib::OK_200;
                res.set_content("true", "application/json");
            })
    });
}  // namespace example