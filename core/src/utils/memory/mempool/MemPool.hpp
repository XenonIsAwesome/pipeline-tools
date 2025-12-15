#pragma once

#include <mutex>
#include <queue>
#include <stdexcept>
#include <utils/memory/smart_buffers/unique_buffer.hpp>

namespace pt::memory {
template<typename T>
class MemPool {
public:
    /**
     *
     * @param block_size The size to allocate each block
     * @param block_count The amount of blocks to allocate
     * @param loc The location the memory is stored
     */
    MemPool(std::size_t block_size,
            std::size_t block_count,
            MemoryLocation loc = MemoryLocation::Host) :
        block_size_(block_size), block_count_(block_count), location_(loc) {
        if (block_size_ == 0 || block_count_ == 0) {
            throw std::invalid_argument("allocation_size and allocation_count must be > 0");
        }

        data_.resize(block_size_ * block_count_);
        for (std::size_t i = 0; i < block_count_; ++i) {
            free_blocks_.push(&data_[i * block_size_]);
        }
    }

    /**
     * Allocate a unique_buffer from the pool
     * @return pt::memory::unique_buffer<T> from the mempool
     */
    unique_buffer<T> allocate() {
        std::lock_guard lock(mtx_);
        if (free_blocks_.empty()) {
            throw std::runtime_error("MemPool: out of buffers");
        }
        T* ptr = free_blocks_.front();
        free_blocks_.pop();

        /// Custom deleter returns the block to the pool
        return unique_buffer<T>(
            ptr,
            block_size_,
            [this](T* p) { this->release_block(p); },
            location_);
    }
    std::size_t block_size() const noexcept {
        return block_size_;
    }
    std::size_t capacity() const noexcept {
        return block_count_;
    }
    std::size_t available() const noexcept {
        std::lock_guard lock(mtx_);
        return free_blocks_.size();
    }

private:
    void release_block(T* p) {
        std::lock_guard<std::mutex> lock(mtx_);
        free_blocks_.push(p);
    }

    std::size_t block_size_;
    std::size_t block_count_;
    MemoryLocation location_;
    std::vector<T> data_;
    std::queue<T*> free_blocks_;
    mutable std::mutex mtx_;
};
}  // namespace pt::memory