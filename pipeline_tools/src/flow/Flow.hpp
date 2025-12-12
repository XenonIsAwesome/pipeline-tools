#pragma once

#include <any>
#include <iostream>
#include <memory>
#include <optional>
#include <set>
#include <utility>
#include <vector>
#include <flow/NodeRED.hpp>


namespace pt::flow {
    enum class ProductionPolicy {
        RoundRobin, /**< Give each output to each consumer: <br>`consumers[i]->consume(outputs[i])` */
        Fanout /**< Copy output to all consumers but move to the last */
    };

    class Flow {
    public:
        virtual ~Flow() = default;

        using input_type = void;
        using output_type = void;

        explicit Flow(const ProductionPolicy policy = ProductionPolicy::Fanout)
            : policy(policy), noderd_enabled(false) {
        }

        virtual std::any process_any(std::any in, size_t producer_id) = 0;

        // ============================================================
        // Node-RED Integration Methods
        // ============================================================

        /**
         * Enable Node-RED integration for this Flow
         * @param metadata Node-RED metadata (ID, type, label, etc.)
         */
        void enable_noderd_integration(const noderd::NodeREDMetadata& metadata) {
            noderd_enabled = true;
            noderd_metadata = metadata;
        }

        /**
         * Get Node-RED metadata for this Flow
         * @return Metadata for Node-RED serialization
         */
        const noderd::NodeREDMetadata& get_noderd_metadata() const {
            return noderd_metadata;
        }

        /**
         * Check if this Flow is integrated with Node-RED
         * @return true if Node-RED integration is enabled
         */
        bool is_noderd_enabled() const {
            return noderd_enabled;
        }

        /**
         * Serialize this Flow and all downstream Flows to Node-RED JSON format
         * Enables visual editing in Node-RED editor
         * @return JSON object containing nodes and wires
         */
        json serialize_to_noderd() const {
            json flow;
            json nodes = json::array();
            json wires = json::array();

            // Collect all flows in graph using DFS with raw pointers
            std::set<const Flow*> visited;
            std::vector<const Flow*> to_visit = {this};
            std::map<const Flow*, size_t> flow_to_index;

            size_t node_index = 0;
            while (!to_visit.empty()) {
                const Flow* current = to_visit.back();
                to_visit.pop_back();

                if (visited.count(current)) {
                    continue;
                }

                visited.insert(current);
                flow_to_index[current] = node_index++;

                if (current->noderd_enabled) {
                    nodes.push_back(current->noderd_metadata.to_json());
                }

                // Add downstream flows
                for (const auto& [next, _] : current->next_nodes) {
                    if (!visited.count(next.get())) {
                        to_visit.push_back(next.get());
                    }
                }
            }

            // Create wires from topology
            for (const Flow* flow : visited) {
                if (flow_to_index.count(flow) && flow->noderd_enabled) {
                    for (const auto& [next, _] : flow->next_nodes) {
                        if (visited.count(next.get()) && next->noderd_enabled) {
                            json wire;
                            wire["source"] = flow->noderd_metadata.node_id;
                            wire["target"] = next->noderd_metadata.node_id;
                            wires.push_back(wire);
                        }
                    }
                }
            }

            flow["nodes"] = nodes;
            flow["wires"] = wires;
            return flow;
        }

        /**
         * Execute with Node-RED message envelope input
         * Automatically deserializes typed message back to C++ std::any
         * @param envelope JSON message envelope from Node-RED
         */
        void execute_from_noderd_message(const json& envelope) {
            noderd::MessageEnvelope msg = noderd::MessageEnvelope::from_json(envelope);
            std::any input = noderd::deserialize_to_any(msg);
            execute(std::move(input), 0);
        }

        /**
         * Get the last output message in Node-RED envelope format
         * @return JSON message envelope of the last produced output
         */
        json get_last_output_as_noderd() const {
            return last_output_envelope.to_json();
        }

        void add_next(const std::shared_ptr<Flow> &next) {
            size_t id = next->register_producer(next);
            next_nodes.emplace_back(next, id);
        }

        void execute(std::any in = {}, size_t producer_id = 0) {
            auto output = process_any(std::move(in), producer_id);
            if (output.type() != typeid(std::nullopt_t) && output.has_value())
                produce(std::move(output));
        }

    protected:
        virtual void produce(std::any output) {
            // Capture for Node-RED serialization
            if (noderd_enabled) {
                last_output_envelope = noderd::MessageEnvelope::from_any(
                    output, noderd_metadata.node_id);
            }

            if (policy == ProductionPolicy::RoundRobin) [[likely]] {
                round_robin(std::move(output));
            } else if (policy == ProductionPolicy::Fanout) [[likely]] {
                for (size_t i = 0; i < next_nodes.size() - 1; ++i) {
                    auto &[next, producer_id] = next_nodes[i];
                    next->execute(output, producer_id);
                }
                auto &[last, producer_id] = next_nodes.at(next_nodes.size() - 1);
                last->execute(std::move(output), producer_id);
            } else {
                std::cerr << "Unknown production policy" << std::endl;
            }
        }

        /**
         * Consumer override point (e.g. Aggregator)
         * @return The ID of the producer
         */
        virtual size_t register_producer(std::shared_ptr<Flow>) { return 0; }

        /**
         * Consumer override point (e.g. Producer)
         * @param output The output to produce in a round-robin way
         */
        virtual void round_robin(std::any output) {
            throw std::runtime_error("Flow objects with a RoundRobin policy must return a vector");
        }

        // (next node, producer_id for *that* consumer)
        std::vector<std::pair<std::shared_ptr<Flow>, size_t> > next_nodes;

    private:
        ProductionPolicy policy;

        // Node-RED integration members
        bool noderd_enabled;
        noderd::NodeREDMetadata noderd_metadata;
        noderd::MessageEnvelope last_output_envelope;
    };

    /**
     * Manual connect (for branching/fanout/etc.)
     * @param from The flow object to wire from
     * @param to The flow object to wire to
     */
    static void connect(const std::shared_ptr<Flow> &from,
                        const std::shared_ptr<Flow> &to) {
        from->add_next(to);
    }
} // namespace pt::flow
