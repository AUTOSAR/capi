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
/// @file       base_tester.cpp
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "base_tester.h"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "api_benchmark.h"
BaseTester::BaseTester(std::int64_t loopCount, std::int64_t intervals) : loopCount_(loopCount), intervals_(intervals)
{
    std::cerr << "looopCount_ " << loopCount_ << std::endl;
    std::cerr << "intervals_ " << intervals_ << std::endl;
}

void BaseTester::runApiBenchMark()
{
    std::cerr << "---------Start api benchmark test--------" << __func__ << std::endl;

    // Configure loop count
    const std::int32_t loopCount = 1000;
    // Create a tester and run the test
    ApiBenchMark tester(loopCount);
    std::int32_t loggerConsoleDuration     = tester.RunLoggerTest("ctx1");
    loggerConsoleDuration_                 = loggerConsoleDuration / loopCount;
    std::int32_t loggerFileDuration        = tester.RunLoggerTest("ctx2");
    loggerFileDuration_                    = loggerFileDuration / loopCount;
    std::int32_t loggerFileConsoleDuration = tester.RunLoggerTest("ctx3");
    loggerFileConsoleDuration_             = loggerFileConsoleDuration / loopCount;
    std::int32_t coutDuration              = tester.RunCoutTest();
    coutDuration_                          = (float)coutDuration / (float)loopCount;
    std::int32_t timestampedDuration       = tester.RunTimestampedCoutTest();
    timestampedDuration_                   = timestampedDuration / loopCount;
    std::int32_t memcpyDuration            = tester.RunMemCpyTest();
    memcpyDuration_                        = memcpyDuration / loopCount;

    std::int32_t RunStdCpyTest = tester.RunStdCpyTest();
    float RunStdCpyTest_       = (float)RunStdCpyTest / (float)loopCount;

    std::int32_t directWriteFile = tester.RunDirectFileWriteTest("./ttt.log");
    directWriteFile_             = (float)directWriteFile / (float)loopCount;

    std::int32_t clocktimeall = tester.clockGetTimeTest();
    float clocktime_          = (float)clocktimeall / (float)loopCount;

    std::int32_t cppSysTime = tester.cppSysTime();
    float cppSysTime_       = (float)cppSysTime / (float)loopCount;

    std::int32_t functionOneTime = tester.functionOneTime();
    float functionOneTime_       = (float)functionOneTime / (float)loopCount;

    std::int32_t functionTenTime = tester.functionTenTime();
    float functionTenTime_       = (float)functionTenTime / (float)loopCount;

    std::int32_t mutexLockTime = tester.mutexLockTime();
    float mutexLockTime_       = (float)mutexLockTime / (float)loopCount;

    std::string outputfile = "benchmark_api.md";
    std::ofstream outfile(outputfile, std::ios::app);
    outfile << "| Test Item | Total time for " + std::to_string(loopCount) + " iterations (μs) | Time per iteration (μs) |\n";
    outfile << "|---------|------|------|\n";
    outfile << "| Log Console| " << loggerConsoleDuration << "|" << loggerConsoleDuration_ << " |\n";

    outfile << "| Log Write File| " << loggerFileDuration << "|" << loggerFileDuration_ << " |\n";

    outfile << "| Log Write File and Console| " << loggerFileConsoleDuration << "|" << loggerFileConsoleDuration_ << " |\n";
    outfile << "| Only cout| " << coutDuration << "|" << coutDuration_ << " |\n";
    outfile << "| Timestamped cout| " << timestampedDuration << "|" << timestampedDuration_ << " |\n";
    outfile << "| memcpy memory copy| " << memcpyDuration << "|" << memcpyDuration_ << " |\n";
    outfile << "| stdcpy memory copy| " << RunStdCpyTest << "|" << RunStdCpyTest_ << " |\n";
    outfile << "| Run empty function once| " << functionOneTime << "|" << functionOneTime_ << " |\n";
    outfile << "| Run empty function 10 times| " << functionTenTime << "|" << functionTenTime_ << " |\n";

    outfile << "| Direct write file| " << directWriteFile << "|" << directWriteFile_ << " |\n";
    outfile << "| mutexLockTime| " << mutexLockTime << "|" << mutexLockTime_ << " |\n";
    outfile << "| system_clock::now| " << cppSysTime << "|" << cppSysTime_ << " |\n";

    outfile << "| Time get clocktime_ | " << clocktimeall << "|" << clocktime_ << " |\n\n\n";
    outfile.close();
    std::cerr << "api benchmark test results saved to file: " << outputfile << " \n" << std::endl;
    // std::cerr << "--------Unit μs, iterations:" << loopCount << "-------" << std::endl;
    // std::cerr << "Log Console : " << loggerConsoleDuration
    //           << ", average:" << loggerConsoleDuration / loopCount << std::endl;
    // std::cerr << "Log File single api : " << loggerFileDuration
    //           << ", average:" << loggerFileDuration / loopCount << std::endl;
    // std::cerr << "loggerFileConsoleDuration : " << loggerFileConsoleDuration
    //           << ", average:" << loggerFileConsoleDuration / loopCount <<
    //           std::endl;
    // std::cerr << "coutDuration : " << coutDuration
    //           << ", average:" << coutDuration / loopCount << std::endl;
    // std::cerr << "timestampedDuration : " << timestampedDuration
    //           << ", average:" << timestampedDuration / loopCount << std::endl;
    // std::cerr << "memcpyDuration : " << memcpyDuration << std::endl;
    // std::cerr << "directWriteFile : " << directWriteFile << std::endl;
}

std::int64_t BaseTester::RunLoggerTest(ara::log::Logger &logger)
{
    std::cerr << "--------BaseTester::" << __func__ << "---------" << std::endl;
    for (std::int64_t i = 0; i < loopCount_; i++) {
        logger.LogInfo() << testMessage_;
        std::this_thread::sleep_for(std::chrono::microseconds(intervals_));
    }
    return 0;
}

std::int32_t BaseTester::RunIdleTest()
{
    for (std::int64_t i = 0; i < loopCount_; i++) {
        std::this_thread::sleep_for(std::chrono::microseconds(intervals_));
    }
    return 0;
}

std::int32_t BaseTester::RunCoutTest()
{
    std::cerr << "--------BaseTester::" << __func__ << "---------" << std::endl;
    for (std::int64_t i = 0; i < loopCount_; i++) {
        std::cout << testMessage_ << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(intervals_));
    }
    return 0;
}

std::int32_t BaseTester::RunMemCpyTest()
{
    std::cerr << "--------BaseTester::" << __func__ << "---------" << std::endl;

    std::int32_t *dest = new std::int32_t[128]{};
    size_t copySize    = std::min(testMessage_.size() + 1, sizeof(std::int32_t) * 128);
    for (std::int64_t i = 0; i < loopCount_; i++) {
        std::memcpy((void *)testMessage_.c_str(), dest, copySize);
        std::this_thread::sleep_for(std::chrono::microseconds(intervals_));
    }

    delete[] dest;
    return 0;
}

std::int32_t BaseTester::RunStdCpyTest()
{
    std::cerr << "--------BaseTester::" << __func__ << "---------" << std::endl;

    std::int32_t *dest = new std::int32_t[128]{};

    for (int j = 0; j < loopCount_; j++) {
        // Use std::copy to copy the string data to dest
        std::copy(testMessage_.begin(), testMessage_.end(), reinterpret_cast< char * >(dest));
        std::this_thread::sleep_for(std::chrono::microseconds(intervals_));
    }

    delete[] dest;
    return 0;
}
void BaseTester::RunGetTimeAndCpy()
{
    std::int32_t *dest = new std::int32_t[1024]{};
    size_t copySize    = std::min(testMessage_.size() + 1, sizeof(std::int32_t) * 1024);
    std::memcpy((void *)testMessage_.c_str(), dest, copySize);
    delete[] dest;

    std::atomic< std::uint16_t > msgCount{0};
    auto now          = std::chrono::system_clock::now();
    auto duration     = now.time_since_epoch();
    auto microseconds = std::chrono::duration_cast< std::chrono::microseconds >(duration).count();

    std::time_t tt    = std::chrono::system_clock::to_time_t(now);
    std::tm *timeInfo = std::localtime(&tt);
    msgCount++;
}

std::int32_t BaseTester::RunTimestampedCoutTest()
{
    std::cerr << "--------BaseTester::" << __func__ << "---------" << std::endl;
    std::atomic< std::uint16_t > msgCount{0};
    for (int k = 0; k < loopCount_; ++k) {
        auto now          = std::chrono::system_clock::now();
        auto duration     = now.time_since_epoch();
        auto microseconds = std::chrono::duration_cast< std::chrono::microseconds >(duration).count();

        std::time_t tt    = std::chrono::system_clock::to_time_t(now);
        std::tm *timeInfo = std::localtime(&tt);

        std::ostringstream oss;
        oss << std::put_time(timeInfo, "%Y/%m/%d %H:%M:%S") << "." << std::setw(6) << std::setfill('0')
            << (microseconds % 1000000);

        std::cout << oss.str() << " " << std::to_string(msgCount.fetch_add(1)) << testMessage_ << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(intervals_));
    }

    return 0;
}

std::int32_t BaseTester::RunCustomCachedFileWriteTest(const std::string &filename, size_t cacheSize)
{
    std::cerr << "--------BaseTester::" << __func__ << "---------" << std::endl;
    std::cerr << "Using custom cache, cache size: " << cacheSize << " bytes" << std::endl;

    std::ofstream file(filename, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return -1;
    }
    RunGetTimeAndCpy();

    // Allocate custom cache memory block
    std::vector< char > cache(cacheSize);
    size_t currentCachePos = 0;  // Current length of data in the cache

    for (std::int64_t i = 0; i < loopCount_; ++i) {
        // Prepare the message to write
        std::string msg = testMessage_;

        const char *msgData = msg.data();
        size_t msgSize      = msg.size();

        // Check whether the cache can accommodate the current message
        if (currentCachePos + msgSize > cacheSize) {
            // Cache full, write to file
            file.write(cache.data(), currentCachePos);
            currentCachePos = 0;  // Reset cache position

            // If the message itself is larger than the cache, write directly
            if (msgSize > cacheSize) {
                file.write(msgData, msgSize);
                file.flush();  // Write to disk immediately
            } else {
                // Otherwise, put it into the newly cleared cache
                std::memcpy(cache.data() + currentCachePos, msgData, msgSize);
                currentCachePos += msgSize;
            }
        } else {
            // Cache not full, add directly to cache
            std::memcpy(cache.data() + currentCachePos, msgData, msgSize);
            currentCachePos += msgSize;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(intervals_));
    }

    // Handle remaining cached data
    if (currentCachePos > 0) {
        file.write(cache.data(), currentCachePos);
    }

    // Ensure all data is written to disk
    file.flush();
    return 0;
}

std::int32_t BaseTester::RunDirectFileWriteTest(const std::string &filename)
{
    std::cerr << "-----------------" << __func__ << std::endl;
    std::cerr << "No cache, each message directly written to file" << std::endl;

    std::ofstream file(filename, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return -1;
    }
    std::string msg = testMessage_;

    for (std::int64_t i = 0; i < loopCount_; ++i) {
        // Prepare the message to write
        // Write directly to file, without custom caching

        RunGetTimeAndCpy();

        // std::ostringstream oss;
        // oss << std::put_time(timeInfo, "%Y/%m/%d %H:%M:%S") << "." <<
        // std::setw(6)
        //     << std::setfill('0') << (microseconds % 1000000);

        file.write(msg.data(), msg.size());
        std::this_thread::sleep_for(std::chrono::microseconds(intervals_));
    }

    return 0;
}
