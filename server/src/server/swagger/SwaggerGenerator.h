#pragma once

#include "EndpointMetadata.h"

#include <httplib.h>
#include <nlohmann/json_fwd.hpp>
#include <server/utils/web_utils.h>
#include <string>
#include <utils/singleton/Singleton.h>

/**
 * A helper macro to both handle the request with a given handler,
 * and add the EndpointMetadata object to the swagger JSON.
 *
 * @param SERVER The server object, passed on to HANDLE_REQUEST
 * @param METHOD The http method of this endpoint, passed on to HANDLE_REQUEST
 * @param PATH The path of this endpoint, passed on to HANDLE_REQUEST
 * @param ENDPOINT_DATA EndpointMetadata object to add to the swagger
 * @param HANDLER The handler function, passed on to HANDLE_REQUEST
 */
#define DECLARE_ENDPOINT(SERVER, METHOD, PATH, ENDPOINT_DATA, HANDLER) \
    HANDLE_REQUEST(SERVER, METHOD, PATH, HANDLER)                      \
    pt::server::swagger::SwaggerGenerator::add_endpoint(PATH, #METHOD, ENDPOINT_DATA);

/**
 * A helper macro to declare a JSON schema.
 * @param NAME The name of the definition of the JSON schema.
 *             Will be used inside the swagger documents like this `#/definitions/NAME`
 * @param SCHEMA The nlohmann::json declaration of the schema.
 *
 * @note Schemas are currently not implemented.
 */
#define DECLARE_SCHEMA(NAME, SCHEMA) \
    pt::server::swagger::SwaggerGenerate::add_endpoint(#NAME, SCHEMA);

namespace pt::server::swagger {
/**
 * A singleton class that collects all the existing endpoints metadata
 * and generates a swagger JSON file
 */
class SwaggerGenerator : public pt::utils::Singleton<SwaggerGenerator> {
public:
    /**
     * Adds an endpoint to the collection of metadata objects.
     * @param path The path of the endpoint
     * @param method The http method of the endpoint
     * @param endpoint The EndpointMetadata object we want to add
     */
    static void
    add_endpoint(const std::string& path, const std::string& method, EndpointMetadata&& endpoint);

    /**
     * Adds a schema to be used later in the parameters and responses metadata
     *
     * @param name The name of the schema
     * @param schema The nlohmann::json declaration of the schema.
     *
     * @note Schemas are currently not implemented
     */
    [[maybe_unused]] static void add_schema(const std::string& name, const nlohmann::json& schema);

    /**
     * Generates a swagger JSON with the given title and version of the API
     * @param title The title of the API
     * @param version The version of the API
     */
    static void
    generate(const std::string& title, const std::string& version, httplib::Server& svr);

private:
    /// The collection of the metadata of all endpoints.
    std::vector<std::tuple<std::string, std::string, EndpointMetadata>> endpoints;

    /**
     * The map of schema name/ref to schema decleration.
     * @note Schemas are currently not implemented
     */
    std::map<std::string, std::string> schemas;
};
}  // namespace pt::server::swagger
