#pragma once

#include <nlohmann/json.hpp>

namespace pt::ipc::rmq::concepts {
template<typename T, typename D>
concept Serialized = requires(T t, D input) {
    {T(input)} -> std::same_as<T>;
    { t.serialize() } -> std::same_as<D>;
};

template<typename T>
concept JSONSerialized = Serialized<T, nlohmann::json>;

template<typename T>
concept StringSerialized = Serialized<T, std::string>;

template<typename T>
concept RMQSerialized = JSONSerialized<T> || StringSerialized<T> ||
                        std::is_same_v<T, std::string> || std::is_same_v<T, nlohmann::json>;
}  // namespace pt::ipc::rmq::concepts