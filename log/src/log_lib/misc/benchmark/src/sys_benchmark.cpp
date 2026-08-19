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
/// @file       sys_benchmark.cpp
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "sys_benchmark.h"

using namespace std;
using namespace std::chrono;

SystemBenchmark::SystemBenchmark()
{
    // The constructor can initialize necessary resources
}

int SystemBenchmark::run()
{
    cout << "Starting Linux system performance test..." << endl;

    double cpu_speed     = test_cpu_speed();
    double mem_bandwidth = test_memory_bandwidth();
    double disk_io_time  = test_disk_io();
    double mutex_time    = test_mutex_lock();

    vector< double > alloc_times = test_memory_allocation();
    vector< double > copy_times  = test_memory_copy();

    generate_markdown_table(cpu_speed, mem_bandwidth, disk_io_time, mutex_time, alloc_times, copy_times);

    cout << "Test completed, results saved to benchmark_result.md" << endl;
    return 0;
}

double SystemBenchmark::test_cpu_speed()
{
    const int iterations = 100000000;
    auto start           = high_resolution_clock::now();

    volatile double result = 1.0;
    for (int i = 1; i <= iterations; ++i) {
        result += 1.0 / i;
    }

    auto end                   = high_resolution_clock::now();
    duration< double > elapsed = end - start;
    return iterations / elapsed.count() / 1e6;  // MFLOPS
}

double SystemBenchmark::test_memory_bandwidth()
{
    const size_t size = 100 * 1024 * 1024;  // 100MB
    char *buffer      = new char[size];

    auto start = high_resolution_clock::now();

    // Sequential write
    for (size_t i = 0; i < size; i++) {
        buffer[i] = (char)(i % 256);
    }

    auto end                   = high_resolution_clock::now();
    duration< double > elapsed = end - start;
    delete[] buffer;

    return (size / (1024.0 * 1024.0)) / elapsed.count();  // MB/s
}

double SystemBenchmark::test_disk_io()
{
    const size_t total_size = 100 * 1024 * 1024;  // 100MB
    const size_t min_chunk  = 16;
    const size_t max_chunk  = 256;
    size_t written          = 0;

    // Generate random data buffer
    char *buffer = new char[max_chunk];
    for (size_t i = 0; i < max_chunk; i++) {
        buffer[i] = rand() % 256;
    }

    ofstream outfile("temp_io_test.bin", ios::binary);
    auto start = high_resolution_clock::now();

    while (written < total_size) {
        size_t chunk_size = min_chunk + (rand() % (max_chunk - min_chunk + 1));
        chunk_size        = min(chunk_size, total_size - written);

        outfile.write(buffer, chunk_size);
        written += chunk_size;
    }

    outfile.close();
    auto end                   = high_resolution_clock::now();
    duration< double > elapsed = end - start;

    delete[] buffer;
    remove("temp_io_test.bin");

    return elapsed.count();  // Return total time (seconds)
}

double SystemBenchmark::test_mutex_lock()
{
    const int iterations = 1000000;
    mutex mtx1, mtx2;

    auto start = high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        lock_guard< mutex > lock1(mtx1);
        lock_guard< mutex > lock2(mtx2);
    }

    auto end                   = high_resolution_clock::now();
    duration< double > elapsed = end - start;
    return elapsed.count() / iterations * 1e9;  // nanoseconds per operation
}

vector< double > SystemBenchmark::test_memory_allocation()
{
    vector< size_t > sizes = {16, 64, 256, 1024, 4096, 16384, 65536};  // bytes
    vector< double > times;

    for (size_t size : sizes) {
        auto start = high_resolution_clock::now();

        for (int i = 0; i < 1000; ++i) {
            char *ptr = new char[size];
            delete[] ptr;
        }

        auto end                   = high_resolution_clock::now();
        duration< double > elapsed = end - start;
        times.push_back(elapsed.count() / 1000 * 1e6);  // microseconds per operation
    }

    return times;
}

vector< double > SystemBenchmark::test_memory_copy()
{
    vector< size_t > sizes = {16, 64, 256, 1024, 4096, 16384, 65536};  // bytes
    vector< double > times;

    for (size_t size : sizes) {
        char *src = new char[size];
        char *dst = new char[size];
        memset(src, 'A', size);

        auto start = high_resolution_clock::now();

        for (int i = 0; i < 1000; ++i) {
            memcpy(dst, src, size);
        }

        auto end                   = high_resolution_clock::now();
        duration< double > elapsed = end - start;
        times.push_back(elapsed.count() / 1000 * 1e6);  // microseconds per operation

        delete[] src;
        delete[] dst;
    }

    return times;
}

void SystemBenchmark::generate_markdown_table(double cpu,
                                              double mem_bw,
                                              double disk_io,
                                              double mutex_time,
                                              vector< double > &alloc_times,
                                              vector< double > &copy_times)
{
    ofstream outfile("benchmark_result.md");

    outfile << "# Linux System Performance Test Report\n\n";
    outfile << "| Test Item | Result |\n";
    outfile << "|---------|------|\n";
    outfile << "| CPU Speed (MFLOPS) | " << fixed << setprecision(2) << cpu << " |\n";
    outfile << "| Memory Bandwidth (MB/s) | " << fixed << setprecision(2) << mem_bw << " |\n";
    outfile << "| Disk IO (100MB write time in seconds) | " << fixed << setprecision(4) << disk_io << " |\n";
    outfile << "| Single-threaded mutex lock overhead (ns/op) | " << fixed << setprecision(2) << mutex_time << " |\n\n";

    outfile << "## Memory allocation overhead (μs/op)\n\n";
    outfile << "| Size (bytes) | 16 | 64 | 256 | 1K | 4K | 16K | 64K |\n";
    outfile << "|------------|----|----|-----|----|----|-----|-----|\n";
    outfile << "| Overhead | ";
    for (double t : alloc_times) {
        outfile << fixed << setprecision(2) << t << " | ";
    }
    outfile << "\n\n";

    outfile << "## Memory copy overhead (μs/op)\n\n";
    outfile << "| Size (bytes) | 16 | 64 | 256 | 1K | 4K | 16K | 64K |\n";
    outfile << "|------------|----|----|-----|----|----|-----|-----|\n";
    outfile << "| Overhead | ";
    for (double t : copy_times) {
        outfile << fixed << setprecision(2) << t << " | ";
    }

    outfile.close();
}