#include "routes.hpp"
#include <utils/logging/logger.h>

void pt::server::EndpointRegistry::start(const std::string& bind_addr, uint16_t port) {
    auto reg = getInstance();

    // Register all endpoints
    register_all();

    // Generate swagger.json
    swagger::SwaggerGenerator::generate("web playground", "1.0.0", reg->svr);

    // Listen on requested bind address and port
    reg->logger.info(LOG_SRC, "Started listening on {}:{}", bind_addr, port);
    auto status = reg->svr.listen(bind_addr, port);

    if (status == false) {
        reg->logger.error(LOG_SRC, "Couldn't bind to port {}", port);
    } else {
        reg->logger.info(LOG_SRC, "Closing server.");
    }
}

void pt::server::EndpointRegistry::register_all() {
    auto reg = getInstance();

    reg->logger.debug(LOG_SRC, "Registering {} handlers", reg->registry.size());
    for (auto& f : reg->registry) {
        f(reg->svr);
    }
}

void pt::server::EndpointRegistry::register_endpoint(const EndpointRegisterHandler& handler) {
    auto reg = getInstance();
    reg->registry.push_back(handler);
}

httplib::Server& pt::server::EndpointRegistry::get_server() {
    auto reg = getInstance();
    return reg->svr;
}

void pt::server::EndpointRegistry::log_request(const httplib::Request& req,
                                                     httplib::Response& res) {
    auto logger = getInstance()->logger;

    auto path = req.path;
    if (!req.params.empty()) {
        path += "?" + httplib::detail::params_to_query_str(req.params);
    }
    logger.info(LOG_SRC,
                "{}:{} - {} {} {} -> {}",
                req.remote_addr,
                req.remote_port,
                req.method,
                path,
                req.version,
                res.status);
}
