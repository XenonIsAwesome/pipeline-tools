#pragma once

#include <concepts>
#include <string>
#include <nlohmann/json.hpp>

namespace pt::ipc::rmq::concepts {
template<typename T, typename D>
concept Deserializer = requires(T t, D input)
{
    {T(input)} -> std::same_as<T>;
};

template<typename T, typename D>
concept Serializer = requires(T t)
{
    {t.serialize()} -> std::same_as<D>;
};

template<typename T>
concept JSONSerializer = Serializer<T, nlohmann::json>;

template<typename T>
concept JSONDeserializer = Deserializer<T, nlohmann::json>;

template<typename T>
concept StringSerializer = Serializer<T, std::string>;

template<typename T>
concept StringDeserializer = Deserializer<T, std::string>;

template<typename T>
concept RMQSerializer = JSONSerializer<T> || StringSerializer<T> || \
    std::is_same_v<T, nlohmann::json> || std::is_same_v<T, std::string>;

template<typename T>
concept RMQDeserializer = JSONDeserializer<T> || StringDeserializer<T>;
}



