#pragma once

#include <flow/Flow.hpp>
#include <flow/NodeRED.hpp>
#include <memory>
#include <string>
#include <functional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace pt::flow {

    /**
     * NodeREDNode: Custom Node-RED node wrapper for Flow objects
     *
     * This class bridges C++ Flow objects to Node-RED's node interface,
     * enabling seamless integration between pipeline-tools and Node-RED.
     *
     * Usage:
     *   auto source = std::make_shared<ConstantSource>(42);
     *   NodeREDNode node(source, "source_node_1", "source");
     *   node.get_metadata();  // Node-RED compatible metadata
     */
    class NodeREDNode {
    public:
        /**
         * Default constructor (required for map storage)
         */
        NodeREDNode() : flow_(nullptr) {
        }

        /**
         * Create a NodeREDNode wrapper around a Flow object
         * @param flow The C++ Flow to wrap
         * @param node_id Unique identifier for this Node-RED node
         * @param node_type Type category ("source", "function", "sink", "aggregator")
         */
        NodeREDNode(std::shared_ptr<Flow> flow,
                   const std::string& node_id,
                   const std::string& node_type)
            : flow_(flow), node_id_(node_id), node_type_(node_type) {

            // Create and register Node-RED metadata
            if (flow_) {
                noderd::NodeREDMetadata metadata;
                metadata.node_id = node_id;
                metadata.node_type = node_type;
                metadata.label = node_id;  // Default label is the node ID

                flow_->enable_noderd_integration(metadata);
            }
        }

        /**
         * Set the display label for this node in Node-RED editor
         * @param label Human-readable label
         */
        void set_label(const std::string& label) {
            if (!flow_) return;
            auto metadata = flow_->get_noderd_metadata();
            metadata.label = label;
            flow_->enable_noderd_integration(metadata);
        }

        /**
         * Add a configuration property visible in Node-RED UI
         * @param key Property name
         * @param value Property value (any JSON-serializable type)
         */
        void add_property(const std::string& key, const json& value) {
            if (!flow_) return;
            auto metadata = flow_->get_noderd_metadata();
            metadata.properties[key] = value;
            flow_->enable_noderd_integration(metadata);
        }

        /**
         * Get the underlying Flow object
         * @return Shared pointer to the wrapped Flow
         */
        std::shared_ptr<Flow> get_flow() const {
            return flow_;
        }

        /**
         * Get the Node-RED metadata for this node
         * @return NodeREDMetadata structure
         */
        noderd::NodeREDMetadata get_metadata() const {
            if (!flow_) {
                return noderd::NodeREDMetadata();
            }
            return flow_->get_noderd_metadata();
        }

        /**
         * Execute this node with input from Node-RED message envelope
         * @param envelope JSON message envelope from upstream node
         */
        void execute_from_message(const json& envelope) {
            if (!flow_) return;
            flow_->execute_from_noderd_message(envelope);
        }

        /**
         * Execute this node and get output as Node-RED message envelope
         * @param input Optional input value
         * @return JSON message envelope with type and value
         */
        json execute_and_get_output(std::any input = std::any()) {
            if (!flow_) {
                json error;
                error["type"] = "error";
                error["value"] = "No flow attached";
                return error;
            }
            flow_->execute(std::move(input), 0);
            return flow_->get_last_output_as_noderd();
        }

        /**
         * Get Node-RED node ID
         * @return The unique node ID
         */
        std::string get_node_id() const {
            return node_id_;
        }

        /**
         * Get Node-RED node type
         * @return The node type ("source", "function", "sink", "aggregator")
         */
        std::string get_node_type() const {
            return node_type_;
        }

        /**
         * Serialize this node to Node-RED JSON representation
         * @return JSON object with node configuration
         */
        json to_json() const {
            return get_metadata().to_json();
        }

    private:
        std::shared_ptr<Flow> flow_;
        std::string node_id_;
        std::string node_type_;
    };

    /**
     * Builder for creating NodeREDNode with fluent API
     *
     * Usage:
     *   auto node = NodeREDNodeBuilder(source, "src1")
     *       .with_label("Input Source")
     *       .with_property("value", 42)
     *       .build();
     */
    class NodeREDNodeBuilder {
    public:
        NodeREDNodeBuilder(std::shared_ptr<Flow> flow,
                          const std::string& node_id)
            : node_id_(node_id), flow_(flow) {
        }

        /**
         * Set the node type
         * @param node_type Type category ("source", "function", "sink", "aggregator")
         * @return Builder for method chaining
         */
        NodeREDNodeBuilder& with_type(const std::string& node_type) {
            node_type_ = node_type;
            return *this;
        }

        /**
         * Set the display label
         * @param label Human-readable label
         * @return Builder for method chaining
         */
        NodeREDNodeBuilder& with_label(const std::string& label) {
            label_ = label;
            return *this;
        }

        /**
         * Add a configuration property
         * @param key Property name
         * @param value Property value
         * @return Builder for method chaining
         */
        NodeREDNodeBuilder& with_property(const std::string& key, const json& value) {
            properties_[key] = value;
            return *this;
        }

        /**
         * Build the NodeREDNode
         * @return Constructed NodeREDNode instance
         */
        NodeREDNode build() {
            if (node_type_.empty()) {
                throw std::runtime_error("Node type must be set before building");
            }

            NodeREDNode node(flow_, node_id_, node_type_);

            if (!label_.empty()) {
                node.set_label(label_);
            }

            for (const auto& [key, value] : properties_) {
                node.add_property(key, value);
            }

            return node;
        }

    private:
        std::string node_id_;
        std::string node_type_;
        std::string label_;
        std::map<std::string, json> properties_;
        std::shared_ptr<Flow> flow_;
    };

} // namespace pt::flow
