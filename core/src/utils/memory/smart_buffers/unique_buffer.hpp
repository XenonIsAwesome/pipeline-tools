#pragma once

#include "buffer_base.hpp"
#include "released_buffer.hpp"

#include <functional>

namespace pt::memory {
/**
 * Move-only owning container for a contiguous block of T elements, similar to unique_ptr.
 *
 * - Constructed from (T* ptr, size_t count, deleter, location).
 * - On destruction, if owning, calls the provided deleter (if non-empty).
 * - `release()` transfers raw ownership to the caller and prevents automatic
 * deletion by this object.
 *
 * Usage notes:
 * - This type is intentionally light-weight and has no reference counting.
 */
template<typename T>
class unique_buffer : public buffer_base<T> {
public:
    using value_type = T;
    using pointer = T*;
    using size_type = std::size_t;
    using deleter_type = std::function<void(T*)>;

    /**
     * Default constructs an empty unique_buffer (no ownership).
     */
    unique_buffer() noexcept = default;

    /**
     * Self-allocating constructor.
     * To avoid the situation where client code will have `new`, which isn't RAII.
     * @param count The amount of elements
     * @param loc Memory location metadata
     */
    unique_buffer(size_type count, MemoryLocation loc = MemoryLocation::Host) :
        buffer_base<T>(new T[count], count, loc), deleter_([](T* p) { delete[] p; }) {}

    /**
     * Construct from raw pointer, count, deleter and memory location.
     * @param ptr pointer to contiguous memory for `count` elements (may be nullptr if count == 0)
     * @param count number of elements (size)
     * @param deleter function that will be called with ptr when this buffer is destroyed (if
     * owning)
     * @param loc memory location metadata (host/device/pinned/managed)
     */
    unique_buffer(pointer ptr,
                  size_type count,
                  deleter_type deleter,
                  MemoryLocation loc = MemoryLocation::Host) noexcept :
        buffer_base<T>(ptr, count, loc), deleter_(std::move(deleter)) {}

    /// No copy semantics
    unique_buffer(const unique_buffer&) = delete;
    unique_buffer& operator=(const unique_buffer&) = delete;

    /**
     * Construct a unique_buffer from an already-released buffer.
     *
     * This is useful for transferring ownership of memory that has been
     * released and may contain deleter information, etc.
     */
    explicit unique_buffer(released_buffer<T> rb) noexcept :
        buffer_base<T>(rb.ptr, rb.count, rb.location), deleter_(std::move(rb.deleter)) {}

    /// Move semantics
    unique_buffer(unique_buffer&& other) noexcept {
        steal_from(std::move(other));
    }
    unique_buffer& operator=(unique_buffer&& other) noexcept {
        if (this != &other) {
            reset();
            // free current
            steal_from(std::move(other));
        }
        return *this;
    }

    /**
     * Destructor calls the stored deleter if we still own memory.
     */
    ~unique_buffer() {
        reset();
    }

    /**
     * @return The owned pointer (may be nullptr).
     */
    pointer get() const noexcept {
        return this->data_;
    }

    /**
     * Releases ownership. After release, this unique_buffer becomes empty and will not free memory.
     * @return released_buffer<T> with the deleter moved out.
     */
    released_buffer<T> release() noexcept {
        released_buffer<T> out{this->data_, this->count_, std::move(deleter_), this->location_};
        // Prevent destructor from freeing
        this->data_ = nullptr;
        this->count_ = 0;
        this->location_ = MemoryLocation::Host;
        deleter_ = nullptr;
        return out;
    }

    /**
     * Reset the buffer — if this object owns memory, call the deleter.
     */
    void reset() noexcept {
        if (this->data_) {
            if (deleter_) {
                // deleter should be noexcept or at least not throw in destructor context
                try {
                    deleter_(this->data_);
                } catch (...) {
                    // Swallow exceptions in destructor/reset context (can't throw).
                }
            }
            this->data_ = nullptr;
            this->count_ = 0;
            this->location_ = MemoryLocation::Host;
        }
        deleter_ = nullptr;
    }

    /**
     * Swap with another unique_buffer (noexcept).
     * @param other The buffer to swap with.
     */
    void swap(unique_buffer& other) noexcept {
        using std::swap;
        swap(this->data_, other.data_);
        swap(this->count_, other.count_);
        swap(this->location_, other.location_);
        swap(deleter_, other.deleter_);
    }

    bool operator==(std::nullptr_t) const noexcept {
        return this->data_ == nullptr;
    }

    explicit operator bool() const noexcept {
        return this->data_ != nullptr;
    }

    /**
     * Convenience: produce a non-owning base view
     */
    buffer_base<T> view() const noexcept {
        return buffer_base<T>(this->data_, this->count_, this->location_);
    }

    deleter_type get_deleter() {
        return deleter_;
    }

private:
    /**
     * Steal resources from another unique_buffer without copying.
     *
     * Transfers all internal state (pointer, size, location, and deleter)
     * from the source buffer to this one. After stealing, the source buffer
     * becomes empty and will not free memory on destruction.
     *
     * This method is used internally during move construction and move
     * assignment operations to transfer ownership without unnecessary copies.
     *
     * @param other The unique_buffer to steal resources from
     */
    void steal_from(unique_buffer&& other) noexcept {
        this->data_ = other.data_;
        this->count_ = other.count_;
        this->location_ = other.location_;
        deleter_ = std::move(other.deleter_);
        other.data_ = nullptr;
        other.count_ = 0;
        other.location_ = MemoryLocation::Host;
        other.deleter_ = nullptr;
    }
    deleter_type deleter_;
};
}