\namespace pt::memory

# PipelineTools's Memory Utilities

The `pt::memory` namespace contains helpers to manage memory structures.

## Smart Buffers

Smart Buffers are containers that wrap arrays of data in a manner that can be described
as a hybrid between smart pointers, `std::vector`, and `std::span`.

Since `std::vector` does not support releasing the memory it holds, transferring ownership between threads, or between
CPU and GPU memory is messy to achieve. Previous versions of PipelineTools used a `std::shared_ptr<void>`
paired with a `size` variable to manage buffers, but this provides no type safety and bounds checking is prone to
errors (you must remember the size of the type that was stored in the `shared_ptr`).

This library provides three classes that can manage a (dynamically) allocated buffer, providing both clear ownership, 
adhering to RAII and Rule of 5 as much as possible, and the ability to "release" a buffer to use the underlying pointer
if need be.

Both `unique_buffer` and `shared_buffer` contain `view()` functions that should be used by client code when access to
the underlying data is needed without owner transference. The `view()` function returns a `buffer_base` object that
allows access to the pointer, size, and location (host / device) of the buffer's memory. `buffer_base` also provides a 
`span()` function to view the buffer as a non-owning container. **Client code should prefer receiving `std::span` if
details of the memory ownership are not required.**

### Released Buffer
Think of Released Buffer as a lightweight wrapper around a buffer. It contains a pointer to the start of the buffer,
its length, and a deleter function that provides the means to free the memory of the buffer. This class **Does not manage
the buffer's memory!** It gives the holder of the class the responsibility to free the buffer.
Released Buffers are usually returned by other Smart Buffers when calling the `release()` function.

### Unique Buffer
A move-only owning container for a contiguous block of type `T` elements, similar to `std::unique_ptr`. 
Use a `unique_buffer` when you don't need shared ownership of the buffer's data.

### Shared Buffer
A thread-safe, reference-counted owning container similar to `std::shared_ptr` for a contiguous block of data.
Use a `shared_buffer` when you need shared ownership of the buffer's data.

## Mempool
A Memory Pool, or MemPool for short, allocates a block of memory and efficiently manages many small, frequent memory 
allocations and deallocations. Instead of repeatedly calling the system allocator (`malloc` / `free`), the pool provides 
fixed-size chunks of memory from a reserved region, improving performance.

The implemented Mempool is thread-safe, and returns a `unique_buffer` with a size that was provided during construction
of the pool. The `unique_buffer` can be cast into a `shared_buffer` if the client wishes.
