#include "EndpointMetadata.h"

namespace swg = pt::server::swagger;

swg::EndpointMetadata swg::EndpointMetadata::add_parameter(Parameter&& parameter) {
    this->parameters.emplace_back(parameter);
    return {summary, parameters, responses};
}

swg::EndpointMetadata swg::EndpointMetadata::add_response(size_t status_code, Response&& res) {
    this->responses[status_code] = res;
    return {summary, parameters, responses};
}

swg::EndpointMetadata swg::EndpointMetadata::add_parameter(std::string_view name,
                                                           ParameterLocation in,
                                                           std::string_view type,
                                                           bool required) {
    return add_parameter({name, in, type, required});
}

swg::EndpointMetadata swg::EndpointMetadata::add_response(size_t status_code,
                                                          std::string_view description) {
    return add_response(status_code, Response{description});
}
