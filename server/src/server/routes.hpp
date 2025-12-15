#pragma once

#include <dawg-log/logger.hpp>
#include <functional>
#include <httplib.h>
#include <server/swagger/SwaggerGenerator.h>
#include <utils/singleton/Singleton.h>
#include <vector>


namespace pt::server {
/**
 * A registry of all the endpoints in the server
 */
class EndpointRegistry : public pt::utils::Singleton<EndpointRegistry> {
public:
    EndpointRegistry() = default;

    using EndpointRegisterHandler = std::function<void(httplib::Server& svr)>;

    /**
     * Registers all the endpoints, generates the swagger and starts the server.
     *
     * @param bind_addr The address to listen on
     * @param port The port to listen on
     */
    static void start(const std::string& bind_addr, uint16_t port);

    /**
     * Registers all the endpoints using the collected `EndpointRegisterHandler`
     */
    static void register_all();

    /**
     * Adds a handler to the registered endpoints collection
     * @param handler Handler function to register the endpoint.
     */
    static void register_endpoint(const EndpointRegisterHandler& handler);

    /**
     * @return The instance of the httplib server.
     */
    static httplib::Server& get_server();

    /**
     * Logs a given request the same way FastAPI does.
     * (Or as close as possible)
     *
     * @param req The request object
     * @param res The response object (to show the http status code)
     */
    static void log_request(const httplib::Request& req, httplib::Response& res);


private:
    httplib::Server svr;
    std::vector<EndpointRegisterHandler> registry;
    DawgLog::TaggedLogger logger {"Server"};
};

/**
 * Helper struct to register an endpoint.
 */
struct RegisterEndpoint {
    explicit RegisterEndpoint(const std::function<void(httplib::Server&)>& f) {
        EndpointRegistry::register_endpoint(f);
    }
};
}  // namespace pt::server
