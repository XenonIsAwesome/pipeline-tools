#include "flow/Mission.hpp"
#include "modules/math/AdditionModule.hpp"
#include "modules/io/PrintSink.hpp"
#include "modules/io/ConstantSource.hpp"

#include <server/routes.hpp>
#include <utils/config/config.h>
#include <utils/logging/logger.h>

class TestFixMission: public pt::flow::Mission {
    void build() override {
        this->add(std::tuple(
            std::make_shared<pt::modules::ConstantSource<int>>(5)
        ), {}, {});

        this->add({}, std::tuple(
            std::make_shared<pt::modules::AdditionModule<int, int, int>>(1)
        ), {});

        this->add({}, {}, std::tuple(
            std::make_shared<pt::modules::PrintSink<int>>()
        ));
    }
};

namespace swg = pt::server::swagger;

int main() {
    pt::utils::logging::initialize_dawglogger();

    TestFixMission m;
    m.start();

    while (true) {}

    // auto registry = pt::server::EndpointRegistry::getInstance();

    // [[maybe_unused]] static pt::server::RegisterEndpoint _register_main(
    //     [](httplib::Server& svr) {
    //         DECLARE_ENDPOINT(
    //             svr,
    //             Get,
    //             "/",
    //             swg::EndpointMetadata("System Status").add_response(httplib::OK_200, "OK"),
    //             [](const httplib::Request& req, httplib::Response& res) {
    //                 res.status = httplib::OK_200;
    //                 res.set_content("true", "application/json");
    //             })
    //     });
    //
    // const auto host = pt::config::get("PT_API_HOST", "0.0.0.0");
    // const auto port = pt::config::get("PT_API_HOST", 8081);
    //
    // registry->start(host, port);

    return 0;
}