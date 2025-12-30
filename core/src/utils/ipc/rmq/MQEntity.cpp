#include "MQEntity.h"

#include <dawg-log/logger.hpp>

pt::ipc::rmq::MQEntity::MQEntity(const RMQOptions& options, const std::string& tag) :
    options(options), logger(tag) {
    try {
        channel = AmqpClient::Channel::CreateFromUri(options.uri);
    } catch (std::exception& e) {
        logger.throw_error<std::runtime_error>(LOG_SRC,
                                               "Error in channel initialization: {}",
                                               e.what());
    }
}
