#include "SwaggerGenerator.h"

#include <fstream>
#include <httplib.h>
#include <map>
#include <nlohmann/json.hpp>
#include <server/utils/web_utils.h>
#include <utils/string_utils.h>
#include <dawg-log/logger.hpp>

namespace swg = pt::server::swagger;

void swg::SwaggerGenerator::generate(const std::string& title,
                                     const std::string& version,
                                     httplib::Server& svr) {
    auto gen = getInstance();

    nlohmann::json swagger;
    swagger["openapi"] = "3.0.0";
    swagger["info"]["title"] = title;
    swagger["info"]["version"] = version;
    swagger["paths"] = nlohmann::json::object();

    for (auto& e_data : gen->endpoints) {
        std::string path = std::get<0>(e_data);
        std::string method = std::get<1>(e_data);

        method = pt::utils::stolower(method);

        EndpointMetadata e = std::get<2>(e_data);

        nlohmann::json op;
        op["summary"] = e.summary;

        nlohmann::json params = nlohmann::json::array();
        for (auto& p : e.parameters) {
            nlohmann::json param;
            param["name"] = p.name;
            param["in"] = parameterLocationMap[p.in];
            param["required"] = p.required;
            param["schema"]["type"] = p.type;

            params.push_back(param);
        }
        op["parameters"] = params;

        for (auto& [code, res] : e.responses) {
            op["responses"][std::to_string(code)]["description"] = res.description;
        }

        swagger["paths"][path][method] = op;
    }

    std::ofstream file(SWAGGER_JSON_PATH);
    file << swagger.dump(4);

    HANDLE_REQUEST(svr, Get, "/swagger.json", [](const httplib::Request&, httplib::Response& res) {
        std::ifstream file(SWAGGER_JSON_PATH);
        std::stringstream content;
        content << file.rdbuf();

        res.set_content(content.str(), "application/json");
    });
}

void swg::SwaggerGenerator::add_endpoint(const std::string& path,
                                         const std::string& method,
                                         EndpointMetadata&& endpoint) {
    auto gen = getInstance();
    gen->endpoints.emplace_back(path, method, std::move(endpoint));
}

void pt::server::swagger::SwaggerGenerator::add_schema(const std::string& name,
                                                             const nlohmann::json& schema) {
    auto gen = getInstance();
    if (gen->schemas.contains(name)) {
        throw std::runtime_error("Schema " + name + " already exists");
    }
    gen->schemas[name] = schema;
}
