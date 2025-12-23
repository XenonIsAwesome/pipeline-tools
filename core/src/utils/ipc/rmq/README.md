\namespace pt::ipc::rmq

# PipelineTools's RabbitMQ Integration

The `pt::ipc::rmq` namespace provides utilities and PipelineTools Flow blocks for interacting with RabbitMQ. It simplifies the process of consuming and publishing messages using a high-level, type-safe C++ interface.

## Core Components

### RMQOptions
A configuration structure used to define connection details and exchange/queue parameters.

```cpp
struct RMQOptions {
    std::string uri;         // e.g., "amqp://localhost"
    std::string username;
    std::string password;

    std::string exchange_name;
    std::string exchange_type = AmqpClient::Channel::EXCHANGE_TYPE_DIRECT;
    // ... additional exchange and queue flags (durable, auto_delete, etc.)
};
```

### Low-level Utilities
These classes provide direct interaction with RabbitMQ:
- **Consumer<T>**: Consumes messages of type `T` from a configured queue.
- **Publisher<T>**: Publishes messages of type `T` to a configured exchange and routing key.

### Flow Framework Blocks
The RabbitMQ integration is seamlessly integrated into the `pt::flow` framework:
- **RMQSource<T>**: A `pt::flow::Source` that produces data by consuming from RabbitMQ.
- **RMQSink<T>**: A `pt::flow::Sink` that consumes data by publishing it to RabbitMQ.

> [!NOTE]
> These blocks reside in the `pt::modules::io::ipc::rmq` namespace.

## Serialization Concepts
The RabbitMQ utilities use C++20 concepts to ensure that types handled by the library can be properly serialized and deserialized. A type `T` must satisfy the `RMQSerialized` concept, which means it must be one of:
- `std::string`
- `nlohmann::json`
- A type that supports `T(input)` and `t.serialize()` where `input` and the return type are either `std::string` or `nlohmann::json`.

## Usage Examples

### Using Publisher and Consumer Directly

```cpp
#include <utils/ipc/rmq/Publisher.h>
#include <utils/ipc/rmq/Consumer.h>

using namespace pt::ipc::rmq;

// Configure options
RMQOptions options;
options.uri = "amqp://localhost";
options.queue_name = "my_queue";

// Publish a message
Publisher<std::string> publisher(options);
publisher.publish("Hello RabbitMQ!");

// Consume a message
Consumer<std::string> consumer(options);
auto msg = consumer.consume(std::chrono::milliseconds(500));
if (msg) {
    std::cout << "Received: " << *msg << std::endl;
}
```

### Using RabbitMQ in a Pipeline

```cpp
#include <flow/Pipeline.hpp>
#include <modules/io/ipc/rmq/RMQSource.h>
#include <modules/io/ipc/rmq/RMQSink.h>

using namespace pt::flow;
using namespace pt::modules::io::ipc::rmq;

Pipeline p;

RMQOptions rmq_opts = { .uri = "amqp://localhost", .queue_name = "test_queue" };

// Create a source that reads from RabbitMQ
auto source = p.add(std::make_shared<RMQSource<std::string>>(rmq_opts));

// Create a sink that writes back to RabbitMQ (different routing key)
auto sink = p.add(std::make_shared<RMQSink<std::string>>(rmq_opts, "output_key"));

p.execute();
```
