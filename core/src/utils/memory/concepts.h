#pragma once

#include <concepts>
#include <cstddef>
#include <span>
#include <type_traits>
#include <utils/memory/memory_location.h>

namespace pt::memory::concepts {
template<typename SpanT, typename T>
concept SpanLike = std::same_as<SpanT, std::span<T>> || std::same_as<SpanT, std::span<const T>>;

template<typename PointerT, typename T>
concept PointerLike = std::same_as<PointerT, T*> || std::same_as<PointerT, const T*>;

template<class BuffT, typename T>
concept SmartBuffer = requires(BuffT smart_buffer) {
    { smart_buffer.size() } -> std::convertible_to<size_t>;
    { smart_buffer.empty() } -> std::same_as<bool>;
    { smart_buffer.location() } -> std::same_as<pt::memory::MemoryLocation>;
    requires PointerLike<decltype(smart_buffer.data()), T>;
    requires SpanLike<decltype(smart_buffer.span()), T>;
    requires SpanLike<decltype(smart_buffer.byte_span()), std::byte>;
};
}  // namespace pt::memory::concepts