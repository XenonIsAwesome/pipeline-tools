\namespace pt::flow

# PipelineTools's Flow Framework

The `pt::flow` namespace contains the core components of the PipelineTools dataflow framework. It allows for building complex, asynchronous, and multi-threaded data processing pipelines.

## Core Concepts

The framework is based on a graph of `Flow` objects that consume and produce data.

### Flow
The base class for all processing units. Elements are connected like a linked list, but a single `Flow` can have multiple consumers. Data is passed between elements using `nstd::any`.

### Blocks
To simplify development, several specialized "blocks" are provided:
- **Source**: A starting point of a pipeline that produces data without input.
- **Module**: A processing unit that takes one input and produces one output.
- **Aggregator**: A unit that collects inputs from multiple producers and processes them once all have arrived.
- **Sink**: An endpoint of a pipeline that consumes data without producing output.

### Typed Interface
While the internal engine uses type-erasure, the framework provides typed base classes (`Source<Out>`, `Module<In, Out>`, `Aggregator<In, Out>`, `Sink<In>`) to ensure type safety at the user level.

## Production Policies
The way a `Flow` object distributes its output to multiple consumers is defined by its `ProductionPolicy`:
- **Fanout** (Default): Every output is sent to every connected consumer. The framework optimizes this by copying data for all but the last consumer, to which the data is moved.
- **RoundRobin**: Outputs are distributed among consumers one by one (load balancing). For `Producer<std::vector<T>>`, elements of the vector are distributed.

## Pipeline
The `Pipeline` class simplifies the wiring of flow objects. It auto-connects elements based on the order they are added:
1. All **Sources** are connected to the first **Node** (Module or Aggregator).
2. **Nodes** are connected linearly.
3. The last **Node** is connected to all **Sinks**.

```cpp
Pipeline p;
auto src = p.add(std::make_shared<MySource>());
auto mod = p.add(std::make_shared<MyModule>());
auto snk = p.add(std::make_shared<MySink>());

p.execute();
```

## Data Flow Behaviors (MiMo)
The framework supports various Many-input Many-output (MiMo) configurations:

- **Single Input - Single Producer**: Standard 1:1 connection where one producer feeds one consumer.
- **Single Input - Many Producers**: Multiple producers (e.g., two different sources) feeding into a single node that processes each input as it arrives.
- **Many Inputs - Single Producer**: A producer provides a collection of data (e.g., `std::vector`) that is consumed as a single unit by a consumer.
- **Many Inputs - Many Producers**: Multiple producers feed into an `Aggregator`, which waits until it has received an input from every registered producer before triggering its process logic.
- **Single Output - Single Consumer**: A module produces a single result that is sent to one consumer.
- **Single Output - Many Consumers**: A module produces a single result that is broadcast to multiple consumers via a **Fanout** policy.
- **Many Outputs - Single Consumer**: A module produces a collection of results that is handed over to a single consumer.
- **Many Outputs - Many Consumers**: A module produces a collection of results that are distributed across multiple consumers via a **RoundRobin** policy.

## Mission
A `Mission` manages multiple `Pipeline` objects and their execution environment. It handles:
- **Auto-connection**: Automatically creates queues between pipelines if their types match.
- **Threading**: Manages `Worker` threads to run pipelines asynchronously.
- **Execution Control**: Provides `start()` and `stop()` methods to control the entire dataflow.

Users should inherit from `Mission` and implement the `build()` method to define their dataflow structure.
