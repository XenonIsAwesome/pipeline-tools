#include "../MockDeleter.hpp"

#include <benchmark/benchmark.h>
#include <utils/memory/smart_buffers/unique_buffer.hpp>

static void BM_UniqueBufferConstruction(benchmark::State& state) {
    for (auto _ : state) {
        int* ptr = new int[1024];
        pt::memory::unique_buffer<int> buf(ptr, 1024, [](int* p) { delete[] p; });
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_UniqueBufferConstruction);

static void BM_UniqueBufferSelfAllocatingConstruction(benchmark::State& state) {
    for (auto _ : state) {
        pt::memory::unique_buffer<int> buf(1024);
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_UniqueBufferSelfAllocatingConstruction);

static void BM_UniqueBufferMoveConstruction(benchmark::State& state) {
    for (auto _ : state) {
        int* ptr = new int[1024];
        pt::memory::unique_buffer<int> source(ptr, 1024, [](int* p) { delete[] p; });

        // Move construct
        pt::memory::unique_buffer<int> moved(std::move(source));
        benchmark::DoNotOptimize(moved);
    }
}
BENCHMARK(BM_UniqueBufferMoveConstruction);

static void BM_UniqueBufferMoveAssignment(benchmark::State& state) {
    for (auto _ : state) {
        int* ptr1 = new int[1024];
        int* ptr2 = new int[1024];

        pt::memory::unique_buffer<int> buf1(ptr1, 1024, [](int* p) { delete[] p; });
        pt::memory::unique_buffer<int> buf2(ptr2, 1024, [](int* p) { delete[] p; });

        // Move assign
        buf1 = std::move(buf2);
    }
}
BENCHMARK(BM_UniqueBufferMoveAssignment);

static void BM_UniqueBufferSizeAccess(benchmark::State& state) {
    int* ptr = new int[1024];
    pt::memory::unique_buffer<int> buf(ptr, 1024, [](int* p) { delete[] p; });

    for (auto _ : state) {
        benchmark::DoNotOptimize(buf.size());
    }
}
BENCHMARK(BM_UniqueBufferSizeAccess);

static void BM_UniqueBufferGet(benchmark::State& state) {
    int* ptr = new int[1024];
    pt::memory::unique_buffer<int> buf(ptr, 1024, [](int* p) { delete[] p; });

    for (auto _ : state) {
        benchmark::DoNotOptimize(buf.get());
    }
}
BENCHMARK(BM_UniqueBufferGet);

static void BM_UniqueBufferReset(benchmark::State& state) {
    for (auto _ : state) {
        int* ptr = new int[1024];
        pt::memory::unique_buffer<int> buf(ptr, 1024, [](int* p) { delete[] p; });

        // Reset should call deleter
        buf.reset();
    }
}
BENCHMARK(BM_UniqueBufferReset);

static void BM_UniqueBufferRelease(benchmark::State& state) {
    for (auto _ : state) {
        int* ptr = new int[1024];
        pt::memory::unique_buffer<int> buf(ptr, 1024, [](int* p) { delete[] p; });

        // Release should transfer ownership
        auto released = buf.release();
        benchmark::DoNotOptimize(released);
    }
}
BENCHMARK(BM_UniqueBufferRelease);

// Test with different sizes to see performance characteristics
static void BM_UniqueBufferSmall(benchmark::State& state) {
    for (auto _ : state) {
        int* ptr = new int[1];
        pt::memory::unique_buffer<int> buf(ptr, 1, [](int* p) { delete[] p; });
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_UniqueBufferSmall);

static void BM_UniqueBufferLarge(benchmark::State& state) {
    for (auto _ : state) {
        int* ptr = new int[1024 * 1024];  // 1MB
        pt::memory::unique_buffer<int> buf(ptr, 1024 * 1024, [](int* p) { delete[] p; });
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_UniqueBufferLarge);

// Test with different deleter patterns to understand overhead
static void BM_UniqueBufferFunctionDeleter(benchmark::State& state) {
    for (auto _ : state) {
        int* ptr = new int[1024];
        auto deleter = [](int* p) {
            delete[] p;
        };
        pt::memory::unique_buffer<int> buf(ptr, 1024, deleter);
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_UniqueBufferFunctionDeleter);

// Test with lambda deleter to see performance impact
static void BM_UniqueBufferLambdaDeleter(benchmark::State& state) {
    for (auto _ : state) {
        int* ptr = new int[1024];
        auto deleter = [ptr](int* p) {
            delete[] p;
        };
        pt::memory::unique_buffer<int> buf(ptr, 1024, deleter);
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_UniqueBufferLambdaDeleter);

// Benchmark to compare with raw pointer operations
static void BM_RawPointerAllocation(benchmark::State& state) {
    for (auto _ : state) {
        int* ptr = new int[1024];
        benchmark::DoNotOptimize(ptr);
        delete[] ptr;
    }
}
BENCHMARK(BM_RawPointerAllocation);

// Test with released buffer conversion
static void BM_UniqueBufferFromReleased(benchmark::State& state) {
    // Create a released buffer first
    int* ptr = new int[1024];

    // Create and immediately release to get a released_buffer
    pt::memory::unique_buffer<int> source(ptr, 1024, [](int* p) { delete[] p; });
    auto released = source.release();

    for (auto _ : state) {
        // Convert back to pt::memory::unique_buffer - this is the benchmarked operation
        pt::memory::unique_buffer<int> buf(std::move(released));
        released = buf.release();  // Reacquire for next iteration
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_UniqueBufferFromReleased);

// Test comparison operations
static void BM_UniqueBufferComparison(benchmark::State& state) {
    int* ptr = new int[1024];
    pt::memory::unique_buffer<int> buf(ptr, 1024, [](int* p) { delete[] p; });

    for (auto _ : state) {
        bool result = (buf == nullptr);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_UniqueBufferComparison);

// Test boolean conversion
static void BM_UniqueBufferBooleanConversion(benchmark::State& state) {
    int* ptr = new int[1024];
    pt::memory::unique_buffer<int> buf(ptr, 1024, [](int* p) { delete[] p; });

    for (auto _ : state) {
        bool result = static_cast<bool>(buf);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_UniqueBufferBooleanConversion);

// Test view creation
static void BM_UniqueBufferView(benchmark::State& state) {
    int* ptr = new int[1024];
    pt::memory::unique_buffer<int> buf(ptr, 1024, [](int* p) { delete[] p; });

    for (auto _ : state) {
        auto view = buf.view();
        benchmark::DoNotOptimize(view);
    }
}
BENCHMARK(BM_UniqueBufferView);

// Test with std::function deleter to measure overhead
static void BM_UniqueBufferFunctionDeleterOverhead(benchmark::State& state) {
    for (auto _ : state) {
        int* ptr = new int[1024];
        std::function<void(int*)> deleter = [](int* p) {
            delete[] p;
        };
        pt::memory::unique_buffer<int> buf(ptr, 1024, deleter);
        benchmark::DoNotOptimize(buf.get());
    }
}
BENCHMARK(BM_UniqueBufferFunctionDeleterOverhead);

// Test small buffer allocation/deallocation
static void BM_UniqueBufferSmallAllocation(benchmark::State& state) {
    for (auto _ : state) {
        int* ptr = new int[1];
        pt::memory::unique_buffer<int> buf(ptr, 1, [](int* p) { delete[] p; });
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_UniqueBufferSmallAllocation);

// Test large buffer allocation/deallocation
static void BM_UniqueBufferLargeAllocation(benchmark::State& state) {
    const size_t size = 1024 * 1024;  // 1MB
    for (auto _ : state) {
        int* ptr = new int[size];
        pt::memory::unique_buffer<int> buf(ptr, size, [](int* p) { delete[] p; });
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_UniqueBufferLargeAllocation);

// Test move performance under different conditions
static void BM_UniqueBufferMovePerformance(benchmark::State& state) {
    for (auto _ : state) {
        int* ptr = new int[1024];
        pt::memory::unique_buffer<int> source(ptr, 1024, [](int* p) { delete[] p; });

        // Move multiple times
        pt::memory::unique_buffer<int> moved1(std::move(source));
        auto temp = std::move(moved1);
    }
}
BENCHMARK(BM_UniqueBufferMovePerformance);

//// Test exception safety path (if any)
static void BM_UniqueBufferExceptionSafety(benchmark::State& state) {
    for (auto _ : state) {
        int* ptr = new int[1024];

        // Create with deleter that might throw
        auto throwing_deleter = [](int* p) {
            delete[] p;
            // No exception here, but demonstrates concept
        };

        pt::memory::unique_buffer<int> buf(ptr, 1024, throwing_deleter);
        benchmark::DoNotOptimize(buf.get());
    }
}
BENCHMARK(BM_UniqueBufferExceptionSafety);

//// Test with different memory locations (if applicable)
static void BM_UniqueBufferMemoryLocation(benchmark::State& state) {
    for (auto _ : state) {
        MockDeleter mock_deleter{};

        auto* ptr = new int[1024];

        pt::memory::unique_buffer<int> buf(ptr, 1024, std::move(mock_deleter), pt::memory::MemoryLocation::Host);
        benchmark::DoNotOptimize(buf.get());
    }
}
BENCHMARK(BM_UniqueBufferMemoryLocation);
