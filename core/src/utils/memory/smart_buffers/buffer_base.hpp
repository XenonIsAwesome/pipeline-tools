#pragma once

#include <span>
#include <utils/memory/memory_location.h>

namespace pt::memory {
/**
 * Non-owning base view for buffers. Exposes size, pointer access, and
 * convenient span conversions.
 *
 * buffer_base intentionally does **not** own memory.
 */
template<typename T>
class buffer_base {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;

    buffer_base() noexcept = default;

    /**
     * @param data The pointer to the data
     * @param count The amount of items
     * @param loc The location where the data is stored (i.e. GPU or CPU)
     */
    buffer_base(pointer data, size_type count, MemoryLocation loc = MemoryLocation::Host) noexcept :
        data_(data), count_(count), location_(loc) {}

    /// Observers
    pointer data() noexcept {
        return data_;
    }
    const_pointer data() const noexcept {
        return data_;
    }
    size_type size() const noexcept {
        return count_;
    }
    bool empty() const noexcept {
        return count_ == 0;
    }
    MemoryLocation location() const noexcept {
        return location_;
    }

    /// Span conversions
    std::span<T> span() noexcept {
        return {data_, count_};
    }
    std::span<const T> span() const noexcept {
        return {data_, count_};
    }

    size_t size_bytes() const noexcept {
        return span().size_bytes();
    }

    /// Byte-span conversions: restricted to trivially_copyable types
    std::span<std::byte> byte_span() {
        static_assert(std::is_trivially_copyable_v<T>, "byte_span requires trivially copyable T");
        return {reinterpret_cast<std::byte*>(data_), count_ * sizeof(T)};
    }
    std::span<const std::byte> byte_span() const {
        static_assert(std::is_trivially_copyable_v<T>, "byte_span requires trivially copyable T");
        return {reinterpret_cast<const std::byte*>(data_), count_ * sizeof(T)};
    }

protected:
    pointer data_ = nullptr;
    size_type count_ = 0;
    MemoryLocation location_ = MemoryLocation::Host;
};
}  // namespace pt::memory