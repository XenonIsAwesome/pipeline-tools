#pragma once

#include <atomic>
#include <optional>
#include <utils/memory/smart_buffers/buffer_base.hpp>
#include <utils/memory/smart_buffers/unique_buffer.hpp>

namespace pt::memory {
/**
 * Reference-counted owning container similer to std::shared_ptr for a contiguous block of data.
 * Thread-safe refcounting is used (via atomic). The control block holds the pointer, the count, the
 * deleter, and the memory location. Copies increment the refcount, moves are cheap.
 *
 * Important behaviour:
 * - `release()` can transfer ownership out as a `released_buffer<T>` *only* if
 * this is the unique owner (use_count == 1); otherwise release fails and
 * returns std::nullopt.
 *
 * Implementation detail:
 * - The control block is allocated by this class (via new). The control block
 * will be destroyed either:
 * * when last owner drops the refcount to zero (then the deleter is invoked),
 * * or when `release()` successfully transfers ownership (the control block
 * is deleted but the deleter is moved into the returned raw_buffer).
 */
template<typename T>
class shared_buffer {
public:
    using value_type = T;
    using pointer = T*;
    using size_type = std::size_t;
    using deleter_type = std::function<void(T*)>;

    shared_buffer() noexcept = default;

    /**
     * Construct from raw pointer, count, deleter and memory location.
     * @param ptr Pointer to contiguous memory for `count` elements (may be nullptr if count == 0)
     * @param size Number of elements (size)
     * @param deleter function that will be called with ptr when this buffer is destroyed (if
     * owning)
     * @param loc memory location metadata (host/device/pinned/managed)
     */
    explicit shared_buffer(pointer ptr,
                           size_type size,
                           deleter_type deleter,
                           MemoryLocation loc = MemoryLocation::Host) : ctrl_(nullptr) {
        if (ptr == nullptr || size == 0) {
            /// Empty
            return;
        }
        ctrl_ = new control_block{ptr, size, std::move(deleter), loc};
    }

    /**
     * Construct a shared_buffer from an already-released buffer.
     *
     * This is useful for transferring ownership of memory that has been
     * released and may contain deleter information, etc.
     */
    explicit shared_buffer(released_buffer<T> rb) noexcept :
        shared_buffer(rb.ptr, rb.count, std::move(rb.deleter), rb.location) {}

    /**
     * Construct a shared_buffer from a unique_buffer
     */
    shared_buffer(unique_buffer<T>&& u_b) : shared_buffer(u_b.release()) {}

    /// Copy semantic: increment refcount
    shared_buffer(const shared_buffer& other) noexcept {
        ctrl_ = other.ctrl_;
        if (ctrl_) {
            ctrl_->ref.fetch_add(1, std::memory_order_relaxed);
        }
    }

    /**
     * Copy assignment operator
     * @param other The other shared buffer to copy from
     * @return A reference to ourselves
     */
    shared_buffer& operator=(const shared_buffer& other) noexcept {
        if (this == &other)
            return *this;
        /// Increment new before decrementing old (strong exception safety, but noexcept here)
        control_block* new_ctrl = other.ctrl_;
        if (new_ctrl) {
            new_ctrl->ref.fetch_add(1, std::memory_order_relaxed);
        }
        release_ctrl();  // decrement old (and possibly free)
        ctrl_ = new_ctrl;
        return *this;
    }

    /// Move semantic: steal control block pointer
    shared_buffer(shared_buffer&& other) noexcept {
        ctrl_ = other.ctrl_;
        other.ctrl_ = nullptr;
    }

    shared_buffer& operator=(shared_buffer&& other) noexcept {
        if (this != &other) {
            release_ctrl();
            ctrl_ = other.ctrl_;
            other.ctrl_ = nullptr;
        }
        return *this;
    }

    ~shared_buffer() {
        release_ctrl();
    }

    /// Observers
    pointer data() const noexcept {
        return ctrl_ ? ctrl_->ptr : nullptr;
    }

    size_type size() const noexcept {
        return ctrl_ ? ctrl_->size : 0;
    }

    bool empty() const noexcept {
        return size() == 0;
    }

    MemoryLocation location() const noexcept {
        return ctrl_ ? ctrl_->location : MemoryLocation::Host;
    }

    /**
     * @return non-owning view
     */
    buffer_base<T> view() const noexcept {
        return buffer_base<T>(data(), size(), location());
    }

    std::span<T> span() const noexcept {
        return view().span();
    }

    std::span<std::byte> byte_span() const noexcept {
        return view().byte_span();
    }

    /**
     * Return current use count (approximate; may be racing)
     * @return current use count (approximate; may be racing)
     */
    size_t use_count() const noexcept {
        return ctrl_ ? static_cast<long>(ctrl_->ref.load(std::memory_order_relaxed)) : 0;
    }

    /**
     * Release ownership only if this is the unique owner (use_count == 1).
     * On success, returns a released_buffer<T> containing ptr, count and the deleter
     * (moved out). After success, this shared_buffer becomes empty and the
     * control block is destroyed. On failure (multiple owners), returns nullopt.
     * @return a released_buffer<T> containing ptr, count and the deleter
     */
    std::optional<released_buffer<T>> release() noexcept {
        if (!ctrl_) {
            return std::nullopt;
        }

        /// Try to transition from ref==1 -> ref==0 (we will delete the control block
        /// and move out the deleter). Use a compare_exchange to ensure atomicity.
        std::size_t expected = 1;
        if (!ctrl_->ref.compare_exchange_strong(expected,
                                                0,
                                                std::memory_order_acq_rel,
                                                std::memory_order_relaxed)) {
            /// Someone else shares ownership; can't release
            return std::nullopt;
        }
        /// We are now the unique owner and have "consumed" the refcount.
        pointer p = ctrl_->ptr;
        size_type c = ctrl_->size;
        MemoryLocation loc = ctrl_->location;
        deleter_type d = std::move(ctrl_->deleter);
        /// delete control block (do not call deleter)
        delete ctrl_;
        ctrl_ = nullptr;
        return released_buffer<T>{p, c, std::move(d), loc};
    }

    /**
     * Reset (drop ownership). If this was the last owner, the stored deleter is
     * invoked to free memory. If this was shared, refcount is decremented.
     */
    void reset() noexcept {
        release_ctrl();
        ctrl_ = nullptr;
    }

    /**
     * Swap two shared_buffer objects (noexcept)
     * @param other The other shared_buffer object
     */
    void swap(shared_buffer& other) noexcept {
        std::swap(ctrl_, other.ctrl_);
    }

    explicit operator bool() const noexcept {
        return ctrl_ != nullptr;
    }

private:
    struct control_block {
        control_block(pointer p, size_type c, deleter_type d, MemoryLocation loc_val) noexcept :
            ref(1), ptr(p), size(c), deleter(std::move(d)), location(loc_val) {}
        std::atomic<std::size_t> ref;


        pointer ptr;
        size_type size;
        deleter_type deleter;
        MemoryLocation location;
    };
    control_block* ctrl_ = nullptr;

    /**
     * Decrement the refcount and, if reaching zero, call deleter and delete block.
     */
    void release_ctrl() noexcept {
        if (!ctrl_) {
            return;
        }

        /// If fetch_sub returns 1, *we* were the last owner after decrement.
        std::size_t prev = ctrl_->ref.fetch_sub(1, std::memory_order_acq_rel);
        if (prev == 1) {
            /// last owner: call deleter (if set) then delete control block
            if (ctrl_->deleter) {
                try {
                    ctrl_->deleter(ctrl_->ptr);
                } catch (...) {
                    /// swallow exceptions in destructor path
                }
            }
            delete ctrl_;
        }
        ctrl_ = nullptr;
    }
};
}  // namespace pt::memory