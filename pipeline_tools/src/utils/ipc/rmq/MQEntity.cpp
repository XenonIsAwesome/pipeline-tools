#include <iostream>
#include "MQEntity.h"
#include <config/get_env.h>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

static constexpr char default_rmq_uri[] = "amqp://guest:guest@127.0.0.1";

pt::ipc::rmq::MQEntity::MQEntity(const RMQOptions& options): options(options){
    auto uri = config::get_env<std::string>("RABBITMQ_URI", default_rmq_uri);
    auto opts = AmqpClient::Channel::OpenOpts::FromUri(uri);

    try {
        channel = AmqpClient::Channel::Open(opts);
    }
    catch (const std::exception& e) {
        std::stringstream ss;
        ss << "[Error] During channel initialization: " << e.what();

        std::cout << ss.str() << std::endl;
        throw std::runtime_error(ss.str());
    }
}
