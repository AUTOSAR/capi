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
/// @file       main_logic.cpp
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "main_logic.h"

#include "api_benchmark.h"
#include "base_tester.h"
#include "signal_handler.h"
// #include <ara/exec/execution_client.h>
#include <ara/log/logger.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <thread>

#include "define.h"

// Definition of global atomic variables
std::atomic_bool continueExecution{true};

std::int32_t MainLogicCpuUsage(std::int64_t const &msgCount, std::int32_t const &methond)
{
    // std::cerr << "-----------------" << __func__ << std::endl;

    // Register signal handlers
    SignalHandler signalHandler(continueExecution);
    if (!signalHandler.Register()) {
        std::cout << "Unable to register signal handler" << std::endl;
    }
    if (msgCount <= 0) {
        std::cerr << "Error: The number of prints per second must be positive" << std::endl;
        return -1;
    }

    // Calculate the interval between each print per second (microseconds)
    auto intervalUs           = 1000000 / msgCount;  // 1 second = 1000000 microseconds
    std::int64_t lastTime     = msgCount * 20;
    ara::log::Logger &logger1 = ara::log::CreateLogger("ctx1", "LTApp0", ara::log::LogLevel::kVerbose);
    ara::log::Logger &logger2 = ara::log::CreateLogger("ctx2", "LTApp0", ara::log::LogLevel::kVerbose);
    ara::log::Logger &logger3 = ara::log::CreateLogger("ctx3", "LTApp0", ara::log::LogLevel::kVerbose);

    auto start = std::chrono::high_resolution_clock::now();

    BaseTester btester(lastTime, intervalUs);
    btester.runApiBenchMark();

    auto end = std::chrono::high_resolution_clock::now();

    auto duration_us = std::chrono::duration_cast< std::chrono::microseconds >(end - start);
    std::cerr << "Code execution time = " << duration_us.count() << " microseconds (μs)" << std::endl;

    switch (methond) {
        case 0: {
            btester.RunIdleTest();
            break;
        }
        case 1: {
            btester.RunLoggerTest(logger1);
            break;
        }
        case 2: {
            btester.RunLoggerTest(logger2);
            break;
        }
        case 3: {
            btester.RunLoggerTest(logger3);
            break;
        }
        case 4: {
            btester.RunCoutTest();
            break;
        }
        case 5: {
            btester.RunTimestampedCoutTest();
            break;
        }
        case 6: {
            btester.RunCustomCachedFileWriteTest("./tmp.txt");
            break;
        }
        case 7: {
            btester.RunDirectFileWriteTest("./dtmp.txt");
            break;
        }
        default:
            break;
    }

    return 0;
}
