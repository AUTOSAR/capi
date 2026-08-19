// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

/// ================================================================
///
/// File description:
/// ----------------
/// @file       allocator_test.cpp
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

// Define test parameters
constexpr int NUM_THREADS            = 4;       // Number of threads
constexpr int ALLOCATIONS_PER_THREAD = 100000;  // Number of allocations per thread
constexpr int MIN_ALLOC_SIZE         = 16;      // Minimum allocation size (16B)
constexpr int MAX_ALLOC_SIZE         = 65536;   // Maximum allocation size (64KB)

// Memory allocator interface (abstract)
class Allocator
{
public:
    virtual void* allocate(size_t size) = 0;
    virtual void deallocate(void* ptr)  = 0;
    virtual const char* name() const    = 0;
    virtual ~Allocator() {}
};

// Default allocator (ptmalloc)
class DefaultAllocator : public Allocator
{
public:
    void* allocate(size_t size) override { return malloc(size); }
    void deallocate(void* ptr) override { free(ptr); }
    const char* name() const override { return "ptmalloc"; }
};

// jemalloc
#ifdef USE_JEMALLOC
    #include <jemalloc/jemalloc.h>
class JemallocAllocator : public Allocator
{
public:
    void* allocate(size_t size) override { return je_malloc(size); }
    void deallocate(void* ptr) override { je_free(ptr); }
    const char* name() const override { return "jemalloc"; }
};
#endif

// tcmalloc
#ifdef USE_TCMALLOC
    #include <gperftools/tcmalloc.h>
class TCMallocAllocator : public Allocator
{
public:
    void* allocate(size_t size) override { return tc_malloc(size); }
    void deallocate(void* ptr) override { tc_free(ptr); }
    const char* name() const override { return "tcmalloc"; }
};
#endif

// mimalloc
#ifdef USE_MIMALLOC
    #include <mimalloc.h>
class MimallocAllocator : public Allocator
{
public:
    void* allocate(size_t size) override { return mi_malloc(size); }
    void deallocate(void* ptr) override { mi_free(ptr); }
    const char* name() const override { return "mimalloc"; }
};
#endif

// Thread task: allocate and free memory
void thread_task(Allocator* allocator, int allocations, int min_size, int max_size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution< int > size_dist(min_size, max_size);

    std::vector< void* > pointers;
    pointers.reserve(allocations);

    // Allocate memory
    for (int i = 0; i < allocations; ++i) {
        size_t size = size_dist(gen);
        void* ptr   = allocator->allocate(size);
        pointers.push_back(ptr);
    }

    // Free memory
    for (void* ptr : pointers) {
        allocator->deallocate(ptr);
    }
}

// Run the benchmark
void run_benchmark(Allocator* allocator)
{
    auto start = std::chrono::high_resolution_clock::now();

    std::vector< std::thread > threads;
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(thread_task, allocator, ALLOCATIONS_PER_THREAD, MIN_ALLOC_SIZE, MAX_ALLOC_SIZE);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >(end - start).count();

    std::cout << allocator->name() << ":\t" << duration << " ms\n";
}

int allocator_main_test()
{
    // List of allocators to test
    std::vector< Allocator* > allocators = {
        new DefaultAllocator(),
#ifdef USE_JEMALLOC
        new JemallocAllocator(),
#endif
#ifdef USE_TCMALLOC
        new TCMallocAllocator(),
#endif
#ifdef USE_MIMALLOC
        new MimallocAllocator(),
#endif
    };

    std::cout << "Benchmarking with " << NUM_THREADS << " threads, " << ALLOCATIONS_PER_THREAD
              << " allocations per thread (" << MIN_ALLOC_SIZE << "B-" << MAX_ALLOC_SIZE << "B)\n";

    for (Allocator* allocator : allocators) {
        run_benchmark(allocator);
        delete allocator;
    }

    return 0;
}