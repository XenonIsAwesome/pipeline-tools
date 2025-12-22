#include "MQEntity.h"

#include <dawg-log/logger.hpp>

pt::ipc::rmq::MQEntity::MQEntity(const RMQOptions& options, const std::string& tag) :
    options(options), logger(tag) {
    try {
        auto open_opts = AmqpClient::Channel::OpenOpts::FromUri(options.uri);
        open_opts.auth =
            AmqpClient::Channel::OpenOpts::BasicAuth(options.username, options.password);

        channel = AmqpClient::Channel::Open(open_opts);
    } catch (std::exception& e) {
        logger.throw_error<std::runtime_error>(LOG_SRC,
                                               "Error in channel initialization: {}",
                                               e.what());
    }
}
