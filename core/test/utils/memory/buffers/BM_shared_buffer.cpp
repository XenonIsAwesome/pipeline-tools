#include "../MockDeleter.hpp"
#include <utils/memory/smart_buffers/shared_buffer.hpp>
#include <utils/memory/smart_buffers/unique_buffer.hpp>

#include <benchmark/benchmark.h>

static void BM_SharedBufferConstruction(benchmark::State& state) {
    for (auto _ : state) {
        MockDeleter mock_deleter;
        auto* ptr = new int[1024];
        benchmark::DoNotOptimize(ptr);

        pt::memory::shared_buffer<int> sb(ptr, 1024, std::move(mock_deleter));
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SharedBufferConstruction);

static void BM_SharedBufferCopyConstruction(benchmark::State& state) {
    MockDeleter mock_deleter;
    auto* ptr = new int[1024];
    pt::memory::shared_buffer<int> sb1(ptr, 1024, mock_deleter);

    for (auto _ : state) {
        pt::memory::shared_buffer sb2(sb1);  // Copy construction
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SharedBufferCopyConstruction);

static void BM_SharedBufferMoveConstruction(benchmark::State& state) {
    for (auto _ : state) {
        MockDeleter mock_deleter;
        auto* ptr = new int[1024];

        pt::memory::shared_buffer<int> sb1(ptr, 1024, std::move(mock_deleter));
        pt::memory::shared_buffer sb2(std::move(sb1));  // Move construction
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SharedBufferMoveConstruction);

static void BM_SharedBufferCopyAssignment(benchmark::State& state) {
    auto deleter = [](int* p) {
        delete[] p;
    };
    auto* ptr1 = new int[1024];
    auto* ptr2 = new int[1024];

    pt::memory::shared_buffer<int> sb1(ptr1, 1024, deleter);
    pt::memory::shared_buffer<int> sb2(ptr2, 1024, deleter);

    for (auto _ : state) {
        sb2 = sb1;  // Copy assignment
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_SharedBufferCopyAssignment);

static void BM_SharedBufferUseCount(benchmark::State& state) {
    auto deleter = [](int* p) {
        delete[] p;
    };
    auto* ptr = new int[1024];
    pt::memory::shared_buffer<int> sb(ptr, 1024, deleter);

    for (auto _ : state) {
        benchmark::DoNotOptimize(sb.use_count());
    }
}
BENCHMARK(BM_SharedBufferUseCount);

static void BM_SharedBufferRelease(benchmark::State& state) {
    MockDeleter mock_deleter;
    auto* ptr = new int[1024];
    pt::memory::shared_buffer<int> sb(ptr, 1024, std::move(mock_deleter));

    for (auto _ : state) {
        MockDeleter mock_deleter2;
        auto released = sb.release();
        if (released) {
            // Reconstruct for next iteration
            sb = pt::memory::shared_buffer<int>(ptr, 1024, mock_deleter2);
        }
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SharedBufferRelease);

static void BM_SharedBufferView(benchmark::State& state) {
    MockDeleter mock_deleter;
    auto* ptr = new int[1024];
    pt::memory::shared_buffer<int> sb(ptr, 1024, std::move(mock_deleter));

    for (auto _ : state) {
        auto view = sb.view();
        benchmark::DoNotOptimize(view.data());
    }
}
BENCHMARK(BM_SharedBufferView);

static void BM_SharedBufferSpan(benchmark::State& state) {
    MockDeleter mock_deleter;
    auto* ptr = new int[1024];
    pt::memory::shared_buffer<int> sb(ptr, 1024, std::move(mock_deleter));

    for (auto _ : state) {
        auto span = sb.span();
        benchmark::DoNotOptimize(span.size());
    }
}
BENCHMARK(BM_SharedBufferSpan);

// Test unique_buffer to shared_buffer conversion
static void BM_UniqueToSharedConversion(benchmark::State& state) {
    for (auto _ : state) {
        MockDeleter mock_deleter;
        pt::memory::unique_buffer<int> ub(new int[1024]{}, 1024, mock_deleter);

        // Convert to shared buffer
        pt::memory::shared_buffer sb(std::move(ub));
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_UniqueToSharedConversion);

// Test high-frequency refcount operations (stress test)
static void BM_SharedBufferRefCountStress(benchmark::State& state) {
    for (auto _ : state) {
        MockDeleter mock_deleter;
        auto* ptr = new int[1024];

        pt::memory::shared_buffer<int> sb(ptr, 1024, std::move(mock_deleter));

        // Create multiple copies
        std::vector<pt::memory::shared_buffer<int>> copies(100);
        for (int i = 0; i < 100; ++i) {
            copies[i] = sb;
        }

        // Then destroy them all at once
        copies.clear();
    }
}
BENCHMARK(BM_SharedBufferRefCountStress);

// Test thread-safe refcount operations (if using threads)
static void BM_SharedBufferThreadSafeRefOp(benchmark::State& state) {
    for (auto _ : state) {
        MockDeleter mock_deleter;
        auto* ptr = new int[1024];

        pt::memory::shared_buffer<int> sb(ptr, 1024, std::move(mock_deleter));

        // This tests the atomic operations
        benchmark::DoNotOptimize(sb.use_count());

        // Simulate concurrent access pattern
        auto copy = sb;
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SharedBufferThreadSafeRefOp);

// Test memory access patterns (not for CPU-bound operations but to ensure no overhead)
static void BM_SharedBufferAccessPattern(benchmark::State& state) {
    MockDeleter mock_deleter;
    auto* ptr = new int[1024];

    // Initialize data
    for (int i = 0; i < 1024; ++i) {
        ptr[i] = i;
    }

    pt::memory::shared_buffer<int> sb(ptr, 1024, std::move(mock_deleter));

    for (auto _ : state) {
        auto span = sb.span();
        int sum = 0;
        for (size_t i = 0; i < span.size(); ++i) {
            sum += span[i];
        }
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_SharedBufferAccessPattern);

// Test memory access with view
static void BM_SharedBufferViewAccess(benchmark::State& state) {
    MockDeleter mock_deleter;
    auto* ptr = new int[1024];

    // Initialize data
    for (int i = 0; i < 1024; ++i) {
        ptr[i] = i;
    }

    pt::memory::shared_buffer<int> sb(ptr, 1024, std::move(mock_deleter));

    for (auto _ : state) {
        auto view = sb.view();
        int sum = 0;
        for (size_t i = 0; i < view.size(); ++i) {
            sum += view.data()[i];
        }
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_SharedBufferViewAccess);
