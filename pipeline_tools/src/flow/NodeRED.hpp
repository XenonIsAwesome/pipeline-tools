#pragma once

#include <any>
#include <chrono>
#include <map>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace pt::flow::noderd {

    /**
     * Type identifiers for safe serialization across Node-RED boundary
     * Maps C++ std::any to JSON-compatible type names
     */
    enum class TypeIdentifier {
        Int32,
        Int64,
        Float,
        Double,
        Bool,
        String,
        Vector,
        Unknown
    };

    /**
     * Metadata for serializing Flow objects to Node-RED format
     * Each Flow can optionally provide metadata for Node-RED integration
     */
    struct NodeREDMetadata {
        std::string node_id;                        ///< Unique Node-RED node ID
        std::string node_type;                      ///< "source", "function", "sink", "aggregator"
        std::string label;                          ///< Display name in Node-RED editor
        std::map<std::string, json> properties;     ///< Custom node configuration

        /**
         * Convert metadata to Node-RED JSON representation
         * @return JSON object with node configuration
         */
        json to_json() const {
            json j;
            j["id"] = node_id;
            j["type"] = node_type;
            j["label"] = label;
            for (const auto& [key, value] : properties) {
                j[key] = value;
            }
            return j;
        }
    };

    /**
     * Detect C++ type from std::any and return identifier
     * @param data std::any value to inspect
     * @return TypeIdentifier corresponding to the data type
     */
    inline TypeIdentifier detect_type(const std::any& data) {
        if (data.type() == typeid(int)) {
            return TypeIdentifier::Int32;
        } else if (data.type() == typeid(long) || data.type() == typeid(long long)) {
            return TypeIdentifier::Int64;
        } else if (data.type() == typeid(float)) {
            return TypeIdentifier::Float;
        } else if (data.type() == typeid(double)) {
            return TypeIdentifier::Double;
        } else if (data.type() == typeid(bool)) {
            return TypeIdentifier::Bool;
        } else if (data.type() == typeid(std::string)) {
            return TypeIdentifier::String;
        } else if (data.type() == typeid(std::vector<int>)) {
            return TypeIdentifier::Vector;
        }
        return TypeIdentifier::Unknown;
    }

    /**
     * Get string representation of TypeIdentifier
     * @param type TypeIdentifier to convert
     * @return String name of type
     */
    inline std::string type_to_string(TypeIdentifier type) {
        switch (type) {
            case TypeIdentifier::Int32:
                return "int32";
            case TypeIdentifier::Int64:
                return "int64";
            case TypeIdentifier::Float:
                return "float";
            case TypeIdentifier::Double:
                return "double";
            case TypeIdentifier::Bool:
                return "bool";
            case TypeIdentifier::String:
                return "string";
            case TypeIdentifier::Vector:
                return "vector";
            case TypeIdentifier::Unknown:
                return "unknown";
        }
        return "unknown";
    }

    /**
     * Get TypeIdentifier from string representation
     * @param type_str String name of type
     * @return TypeIdentifier corresponding to the string
     */
    inline TypeIdentifier string_to_type(const std::string& type_str) {
        if (type_str == "int32") {
            return TypeIdentifier::Int32;
        } else if (type_str == "int64") {
            return TypeIdentifier::Int64;
        } else if (type_str == "float") {
            return TypeIdentifier::Float;
        } else if (type_str == "double") {
            return TypeIdentifier::Double;
        } else if (type_str == "bool") {
            return TypeIdentifier::Bool;
        } else if (type_str == "string") {
            return TypeIdentifier::String;
        } else if (type_str == "vector") {
            return TypeIdentifier::Vector;
        }
        return TypeIdentifier::Unknown;
    }

    /**
     * Message envelope for safe type serialization across Node-RED
     * Preserves type information through JSON serialization
     */
    struct MessageEnvelope {
        std::string type_name;      ///< Type identifier ("int32", "float", "string", etc.)
        json value;                 ///< Serialized value
        int64_t timestamp_ms;       ///< When message was created (milliseconds since epoch)
        std::string source_node_id; ///< Which Node-RED node sent this message

        /**
         * Create MessageEnvelope from std::any with automatic type detection
         * @param data std::any value to serialize
         * @param source_id Optional source node ID
         * @return MessageEnvelope with type information and serialized value
         */
        static MessageEnvelope from_any(const std::any& data,
                                       const std::string& source_id = "") {
            MessageEnvelope env;
            env.source_node_id = source_id;
            env.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

            TypeIdentifier type = detect_type(data);
            env.type_name = type_to_string(type);

            // Serialize based on detected type
            try {
                switch (type) {
                    case TypeIdentifier::Int32:
                        env.value = std::any_cast<int>(data);
                        break;
                    case TypeIdentifier::Int64:
                        env.value = std::any_cast<long long>(data);
                        break;
                    case TypeIdentifier::Float:
                        env.value = std::any_cast<float>(data);
                        break;
                    case TypeIdentifier::Double:
                        env.value = std::any_cast<double>(data);
                        break;
                    case TypeIdentifier::Bool:
                        env.value = std::any_cast<bool>(data);
                        break;
                    case TypeIdentifier::String:
                        env.value = std::any_cast<std::string>(data);
                        break;
                    case TypeIdentifier::Vector: {
                        auto vec = std::any_cast<std::vector<int>>(data);
                        env.value = json::array();
                        for (int v : vec) {
                            env.value.push_back(v);
                        }
                        break;
                    }
                    case TypeIdentifier::Unknown:
                        env.type_name = "unknown";
                        env.value = "unsupported type";
                        break;
                }
            } catch (const std::bad_any_cast& e) {
                env.type_name = "error";
                env.value = std::string("Serialization error: ") + e.what();
            }

            return env;
        }

        /**
         * Convert MessageEnvelope to JSON representation
         * @return JSON object with type, value, timestamp, and source
         */
        json to_json() const {
            json j;
            j["type"] = type_name;
            j["value"] = value;
            j["timestamp"] = timestamp_ms;
            j["source"] = source_node_id;
            return j;
        }

        /**
         * Create MessageEnvelope from JSON
         * @param j JSON object with type and value fields
         * @return MessageEnvelope deserialized from JSON
         */
        static MessageEnvelope from_json(const json& j) {
            MessageEnvelope env;
            env.type_name = j.value("type", "unknown");
            env.value = j.value("value", json{});
            env.timestamp_ms = j.value("timestamp", static_cast<int64_t>(0));
            env.source_node_id = j.value("source", "");
            return env;
        }
    };

    /**
     * Deserialize MessageEnvelope back to std::any
     * @param envelope MessageEnvelope to deserialize
     * @return std::any value reconstructed from envelope
     * @throws std::runtime_error if type is unknown
     */
    inline std::any deserialize_to_any(const MessageEnvelope& envelope) {
        TypeIdentifier type = string_to_type(envelope.type_name);

        try {
            switch (type) {
                case TypeIdentifier::Int32:
                    return std::any(envelope.value.get<int>());
                case TypeIdentifier::Int64:
                    return std::any(envelope.value.get<long long>());
                case TypeIdentifier::Float:
                    return std::any(envelope.value.get<float>());
                case TypeIdentifier::Double:
                    return std::any(envelope.value.get<double>());
                case TypeIdentifier::Bool:
                    return std::any(envelope.value.get<bool>());
                case TypeIdentifier::String:
                    return std::any(envelope.value.get<std::string>());
                case TypeIdentifier::Vector: {
                    std::vector<int> vec = envelope.value.get<std::vector<int>>();
                    return std::any(vec);
                }
                case TypeIdentifier::Unknown:
                    throw std::runtime_error("Cannot deserialize unknown type: " +
                                            envelope.type_name);
            }
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Deserialization error: ") + e.what());
        }

        return std::any();
    }

} // namespace pt::flow::noderd
