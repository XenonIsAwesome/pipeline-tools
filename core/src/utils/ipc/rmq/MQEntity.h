#pragma once

#include <dawg-log/tagged_logger.hpp>
#include <SimpleAmqpClient/Channel.h>
#include <string>

namespace pt::ipc::rmq {

struct RMQOptions {
    std::string uri;

    std::string exchange_name;
    std::string exchange_type = AmqpClient::Channel::EXCHANGE_TYPE_DIRECT;
    bool exchange_passive = false;
    bool exchange_durable = false;
    bool exchange_auto_delete = false;

    std::string queue_name;
    bool queue_passive = false;
    bool queue_durable = false;
    bool queue_exclusive = false;
    bool queue_auto_delete = false;
};

class MQEntity {
public:
    MQEntity(const RMQOptions& options, const std::string& tag);

private:
    RMQOptions options;

protected:
    AmqpClient::Channel::ptr_t channel;
    DawgLog::TaggedLogger logger;
};
}  // namespace pt::ipc::rmq