#include <benchmark/benchmark.h>
#include <utils/memory/mempool/MemPool.hpp>

static void BM_MemPool_Allocate(benchmark::State& state) {
    const size_t block_size = 1024;
    const size_t block_count = 1000;

    pt::memory::MemPool<char> pool(block_size, block_count);

    for (auto _ : state) {
        auto buf = pool.allocate();
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_MemPool_Allocate)->Range(1 << 10, 1 << 20);  // up to ~1MB

static void BM_MemPool_Reuse(benchmark::State& state) {
    const size_t block_size = 64;
    const size_t block_count = 100;

    for (auto _ : state) {
        pt::memory::MemPool<char> pool(block_size, block_count);
        std::vector<pt::memory::unique_buffer<char>> buffers;

        // Allocate all
        for (size_t i = 0; i < block_count; ++i) {
            buffers.push_back(pool.allocate());
        }

        // Deallocate all
        buffers.clear();
    }
}
BENCHMARK(BM_MemPool_Reuse);

static void BM_NewDelete(benchmark::State& state) {
    const size_t block_size = 64;
    for (auto _ : state) {
        char* ptr = new char[block_size];
        benchmark::DoNotOptimize(ptr);
        delete[] ptr;
    }
}
BENCHMARK(BM_NewDelete);