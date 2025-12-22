#pragma once

#include <cstdint>

namespace pt::memory {
/**
 * The location the memory is stored
 */
enum class MemoryLocation {
    Host,
    HostPinned,
    Device,
    Unified  /// e.g., cudaMallocManaged
};
}  // namespace pt::memory