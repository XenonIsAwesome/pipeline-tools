#pragma once

#include <functional>
#include <utils/memory/memory_location.h>

namespace pt::memory {
/**
 * A lightweight Plain Old Data like structure returned by `release()` carrying the raw
 * pointer, size, deleter and location. Ownership of the memory (and the
 * responsibility to call the deleter) transfers to the receiver.
 *
 * returned by `release()` of smart smart_buffers
 */
template<typename T>
struct released_buffer {
    T* ptr = nullptr;
    std::size_t count = 0;
    std::function<void(T*)> deleter;  /// may be empty -> user must still manage
    MemoryLocation location = MemoryLocation::Host;

    released_buffer() = default;
    released_buffer(T* p, std::size_t c, std::function<void(T*)> d, MemoryLocation loc) noexcept :
        ptr(p), count(c), deleter(std::move(d)), location(loc) {}
};
}  // namespace pt::memory