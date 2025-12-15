#pragma once

#include <httplib.h>
#include <server/routes.hpp>

namespace pt::server::utils {};

/**
 * A helper macro to handle a request,
 * wraps around the handler and adds headers.
 * Also logs the request using `pt::server::utils::log_request`
 *
 * @param SERVER The server object
 * @param METHOD The http method of this endpoint
 * @param PATH The path of this endpoint
 * @param HANDLER The handler function
 */
#define HANDLE_REQUEST(SERVER, METHOD, PATH, HANDLER)                                  \
    SERVER.METHOD(PATH, [](const httplib::Request& req, httplib::Response& res) {      \
        /* Adding headers */                                                           \
        res.set_header("Access-Control-Allow-Origin", "*");                            \
        res.set_header("Access-Control-Allow-Methods", req.method);                    \
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization"); \
                                                                                       \
        /* Calling the handler */                                                      \
        res.status = httplib::OK_200;                                                  \
        HANDLER(req, res);                                                             \
                                                                                       \
        /* Logging the request */                                                      \
        pt::server::EndpointRegistry::log_request(req, res);                     \
    });
