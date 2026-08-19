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
/// @file       sys_benchmark.h
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef SYSTEM_BENCHMARK_H
#define SYSTEM_BENCHMARK_H

#include <sys/sysinfo.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

class SystemBenchmark
{
public:
    SystemBenchmark();
    ~SystemBenchmark() = default;

    int run();

private:
    double test_cpu_speed();
    double test_memory_bandwidth();
    double test_disk_io();
    double test_mutex_lock();
    std::vector< double > test_memory_allocation();
    std::vector< double > test_memory_copy();

    void generate_markdown_table(double cpu,
                                 double mem_bw,
                                 double disk_io,
                                 double mutex_time,
                                 std::vector< double > &alloc_times,
                                 std::vector< double > &copy_times);
};

#endif  // SYSTEM_BENCHMARK_H