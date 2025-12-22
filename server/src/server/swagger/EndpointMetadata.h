#pragma once

#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>

namespace pt::server::swagger {
/**
 * Location of the parameter
 */
enum class ParameterLocation {
    URL_QUERY, /**< Query parameter, i.e. POST /api/something?param=value&param2=value */
    PATH, /**< Path parameter, i.e. /api/something/value1/value2/ */
    BODY /**< Body parameter for form submission. */
};

/**
 * Convertion between the enum and the actual string that is going to
 * be written down in the JSON.
 */
static std::map<ParameterLocation, const char*> parameterLocationMap = {
    {ParameterLocation::URL_QUERY, "query"},
    {ParameterLocation::PATH, "path"},
    {ParameterLocation::BODY, "body"}};

/**
 * Data needed to declare a parameter in the JSON
 *
 * @note Schemas are currently not implemented
 */
struct Parameter {
    std::string_view name;
    ParameterLocation in;
    std::string_view type;
    bool required;
};

/**
 * Data needed to declare a response in the JSON
 *
 * @note Schemas are currently not implemented
 */
struct Response {
    std::string_view description;
};

/**
 * Metadata included in an endpoint declaration
 */
class EndpointMetadata {
public:
    /**
     * @param summary A short description about this endpoint
     * @param parameters A vector of parameter metadata
     * @param responses A map of http status codes, to response metadata
     */
    EndpointMetadata(const std::string& summary = "",
                     const std::vector<Parameter>& parameters = {},
                     const std::map<size_t, Response>& responses = {}) :
        summary(summary), parameters(parameters), responses(responses) {}

    /**
     * Copy ctor
     */
    EndpointMetadata(EndpointMetadata& other) = default;

    /**
     * Move semantic ctor
     */
    EndpointMetadata(EndpointMetadata&& other) = default;

    /**
     * Creates and adds a parameter to the metadata
     * @param name The name of the parameter
     * @param in The location of the parameter
     * @param type The variable type of the parameter
     * @param required Is the parameter required
     * @return Another EndpointMetadata object, allowing chaining of method calls.
     */
    EndpointMetadata add_parameter(std::string_view name,
                                   ParameterLocation in,
                                   std::string_view type,
                                   bool required);

    /**
     * Adds an existing parameter to the metadata
     * @param parameter The existing parameter data
     * @return Another EndpointMetadata object, allowing chaining of method calls.
     */
    EndpointMetadata add_parameter(Parameter&& parameter);

    /**
     * Creates and adds a response to the metadata.
     * @param status_code The status code of the response
     * @param description A short description about the response
     * @return Another EndpointMetadata object, allowing chaining of method calls.
     */
    EndpointMetadata add_response(size_t status_code, std::string_view description);

    /**
     * Adds an existing response to the metadata
     * @param status_code The status code of the response
     * @param res The existing response data
     * @return Another EndpointMetadata object, allowing chaining of method calls.
     */
    EndpointMetadata add_response(size_t status_code, Response&& res);

    /// A short description of this endpoint
    std::string summary;

    /// A vector to hold the parameters of this endpoints
    std::vector<Parameter> parameters;

    /// A map of the http status codes to the responses of this endpoints
    std::map<size_t, Response> responses;
};
}  // namespace pt::server::swagger
