#pragma once

#include <nlohmann/json.hpp>
#include <flow/Flow.hpp>
#include <flow/NodeREDNode.hpp>
#include <string>
#include <map>
#include <memory>
#include <functional>

using json = nlohmann::json;

namespace pt::utils {

    /**
     * HTTPServerRegistry: Manages HTTP endpoints for Flow execution
     *
     * This class provides a registry pattern for exposing C++ Flow objects
     * as HTTP endpoints, enabling Node-RED to call pipelines via REST API.
     *
     * Usage:
     *   HTTPServerRegistry registry;
     *   registry.register_node("add_node", std::make_shared<AdditionModule>());
     *   auto response = registry.execute("add_node", {{"type": "int32", "value": 10}});
     *
     * For actual HTTP server integration (e.g., with cpp-httplib or Crow),
     * use the execute() method as the handler for your endpoints.
     */
    class HTTPServerRegistry {
    public:
        /**
         * Register a Flow/Node for HTTP execution
         * @param endpoint_name Unique name for the HTTP endpoint
         * @param flow The Flow object to register
         */
        void register_node(const std::string& endpoint_name,
                          std::shared_ptr<pt::flow::Flow> flow) {
            flows_[endpoint_name] = flow;
        }

        /**
         * Register a NodeREDNode for HTTP execution
         * @param endpoint_name Unique name for the HTTP endpoint
         * @param node The NodeREDNode to register
         */
        void register_noderd_node(const std::string& endpoint_name,
                                 const pt::flow::NodeREDNode& node) {
            flows_[endpoint_name] = node.get_flow();
            nodes_[endpoint_name] = node;
        }

        /**
         * Execute a registered Flow with JSON input
         * @param endpoint_name Name of the endpoint to execute
         * @param input_envelope JSON message envelope with type and value
         * @return JSON response with output or error message
         *
         * Input format:
         *   {"type": "int32", "value": 42}
         *
         * Output format (success):
         *   {"status": "success", "output": {"type": "int32", "value": 84}}
         *
         * Output format (error):
         *   {"status": "error", "message": "Unknown endpoint"}
         */
        json execute(const std::string& endpoint_name, const json& input_envelope) {
            try {
                if (flows_.find(endpoint_name) == flows_.end()) {
                    return json{
                        {"status", "error"},
                        {"message", "Endpoint not found: " + endpoint_name}
                    };
                }

                auto flow = flows_[endpoint_name];

                // Deserialize input from envelope
                try {
                    pt::flow::noderd::MessageEnvelope env =
                        pt::flow::noderd::MessageEnvelope::from_json(input_envelope);
                    std::any input = pt::flow::noderd::deserialize_to_any(env);

                    // Execute the flow
                    flow->execute(std::move(input), 0);

                    // Get output as envelope
                    json output = flow->get_last_output_as_noderd();

                    json response;
                    response["status"] = "success";
                    response["output"] = output;
                    return response;

                } catch (const std::exception& e) {
                    return json{
                        {"status", "error"},
                        {"message", std::string("Deserialization error: ") + e.what()}
                    };
                }

            } catch (const std::exception& e) {
                return json{
                    {"status", "error"},
                    {"message", std::string("Execution error: ") + e.what()}
                };
            }
        }

        /**
         * Get all registered endpoints
         * @return Vector of endpoint names
         */
        std::vector<std::string> get_endpoints() const {
            std::vector<std::string> endpoints;
            for (const auto& [name, _] : flows_) {
                endpoints.push_back(name);
            }
            return endpoints;
        }

        /**
         * Get Node-RED metadata for all registered nodes
         * @return JSON array of node metadata for Node-RED editor
         */
        json get_all_nodes_metadata() const {
            json nodes_array = json::array();

            for (const auto& [endpoint_name, _] : flows_) {
                auto flow = flows_.at(endpoint_name);
                if (flow->is_noderd_enabled()) {
                    nodes_array.push_back(flow->get_noderd_metadata().to_json());
                }
            }

            return nodes_array;
        }

        /**
         * Check if an endpoint exists
         * @param endpoint_name Name to check
         * @return true if endpoint is registered
         */
        bool has_endpoint(const std::string& endpoint_name) const {
            return flows_.find(endpoint_name) != flows_.end();
        }

        /**
         * Get the number of registered endpoints
         * @return Count of endpoints
         */
        size_t endpoint_count() const {
            return flows_.size();
        }

    private:
        std::map<std::string, std::shared_ptr<pt::flow::Flow>> flows_;
        std::map<std::string, pt::flow::NodeREDNode> nodes_;
    };

    /**
     * NodeREDFlowRegistry: Manages collections of related Flows as Node-RED flows
     *
     * This class represents a single Node-RED flow (tab) containing multiple nodes.
     * It manages the graph topology and enables serialization to Node-RED format.
     *
     * Usage:
     *   NodeREDFlowRegistry flow("my_flow");
     *   flow.add_node("source", source_node);
     *   flow.add_node("module", module_node);
     *   flow.connect("source", "module");
     *   auto json = flow.serialize();
     */
    class NodeREDFlowRegistry {
    public:
        /**
         * Create a new Node-RED flow container
         * @param flow_id Unique identifier for this flow (tab)
         */
        explicit NodeREDFlowRegistry(const std::string& flow_id)
            : flow_id_(flow_id) {
        }

        /**
         * Add a node to this flow
         * @param node_name Reference name within this flow
         * @param node The NodeREDNode to add
         */
        void add_node(const std::string& node_name, const pt::flow::NodeREDNode& node) {
            nodes_[node_name] = node;
            node_mapping_[node.get_node_id()] = node_name;
        }

        /**
         * Connect two nodes in this flow
         * @param from_name Source node name
         * @param to_name Destination node name
         */
        void connect(const std::string& from_name, const std::string& to_name) {
            if (nodes_.find(from_name) == nodes_.end()) {
                throw std::runtime_error("Source node not found: " + from_name);
            }
            if (nodes_.find(to_name) == nodes_.end()) {
                throw std::runtime_error("Destination node not found: " + to_name);
            }

            auto from_flow = nodes_[from_name].get_flow();
            auto to_flow = nodes_[to_name].get_flow();

            from_flow->add_next(to_flow);
            connections_.push_back({from_name, to_name});
        }

        /**
         * Get all nodes in this flow
         * @return Map of node name to NodeREDNode
         */
        const std::map<std::string, pt::flow::NodeREDNode>& get_nodes() const {
            return nodes_;
        }

        /**
         * Get all connections in this flow
         * @return Vector of (from, to) node name pairs
         */
        const std::vector<std::pair<std::string, std::string>>& get_connections() const {
            return connections_;
        }

        /**
         * Serialize this flow to Node-RED JSON format
         * @return JSON object representing the flow
         */
        json serialize() const {
            json flow;
            flow["id"] = flow_id_;

            json nodes_array = json::array();
            for (const auto& [_, node] : nodes_) {
                nodes_array.push_back(node.to_json());
            }

            json wires_array = json::array();
            for (const auto& [from_name, to_name] : connections_) {
                json wire;
                wire["source"] = nodes_.at(from_name).get_node_id();
                wire["target"] = nodes_.at(to_name).get_node_id();
                wires_array.push_back(wire);
            }

            flow["nodes"] = nodes_array;
            flow["wires"] = wires_array;
            return flow;
        }

        /**
         * Get the flow ID
         * @return The unique identifier for this flow
         */
        std::string get_flow_id() const {
            return flow_id_;
        }

    private:
        std::string flow_id_;
        std::map<std::string, pt::flow::NodeREDNode> nodes_;
        std::map<std::string, std::string> node_mapping_;  // node_id -> node_name
        std::vector<std::pair<std::string, std::string>> connections_;
    };

} // namespace pt::utils
