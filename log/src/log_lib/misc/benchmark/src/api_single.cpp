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
/// @file       api_single.cpp
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "api_single.h"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <fstream>
#include <iostream>
#include <tuple>

ApiSingle::ApiSingle() {}

void ApiSingle::RunLoggerTest(ara::log::Logger &logger) { logger.LogInfo() << testMessage_; }

void ApiSingle::RunCoutTest() { std::cout << testMessage_ << std::endl; }

void ApiSingle::RunMemCpyTest()
{
    std::string src    = testMessage_;
    std::int32_t *dest = new std::int32_t[128]{};
    size_t copySize    = std::min(src.size() + 1, sizeof(std::int32_t) * 128);

    std::memcpy((void *)src.c_str(), dest, copySize);

    delete[] dest;
}

void ApiSingle::RunStdCpyTest()
{
    std::string src    = testMessage_;
    std::int32_t *dest = new std::int32_t[128]{};

    std::copy(src.begin(), src.end(), reinterpret_cast< char * >(dest));
    // Manually add the string terminator
    reinterpret_cast< char * >(dest)[src.size()] = '\0';

    delete[] dest;
}

void ApiSingle::RunTimestampedCoutTest()
{
    std::atomic< std::uint16_t > msgCount{0};

    auto now          = std::chrono::system_clock::now();
    auto duration     = now.time_since_epoch();
    auto microseconds = std::chrono::duration_cast< std::chrono::microseconds >(duration).count();

    std::time_t tt    = std::chrono::system_clock::to_time_t(now);
    std::tm *timeInfo = std::localtime(&tt);

    std::ostringstream oss;
    oss << std::put_time(timeInfo, "%Y/%m/%d %H:%M:%S") << "." << std::setw(6) << std::setfill('0')
        << (microseconds % 1000000);

    std::cout << oss.str() << " " << std::to_string(msgCount.fetch_add(1)) << testMessage_ << std::endl;
}

void ApiSingle::RunGetTimeAndCpy()
{
    std::unique_lock< std::mutex > const loggerguard{testMutex_};

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
    std::ignore       = timeInfo;
    msgCount++;
}
void ApiSingle::RunCustomCachedFileWriteTest(const std::string &filename, size_t cacheSize)
{
    std::cerr << "-----------------" << __func__ << std::endl;
    std::cerr << "Using custom cache, cache size: " << cacheSize << " bytes" << std::endl;

    std::ofstream file(filename, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return;
    }
    RunGetTimeAndCpy();

    // Allocate custom cache memory block
    std::vector< char > cache(cacheSize);
    size_t currentCachePos = 0;  // Current length of data in the cache

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

    // Handle remaining cached data
    if (currentCachePos > 0) {
        file.write(cache.data(), currentCachePos);
    }

    // Ensure all data is written to disk
    file.flush();
    file.close();
}

void ApiSingle::RunDirectFileWriteTest(const std::string &filename)
{
    std::cerr << "----No cache, each message directly written to file - test------------" << __func__ << std::endl;

    std::ofstream file(filename, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return;
    }

    std::string msg = testMessage_;

    // Prepare the message to write
    // Write directly to file, without custom caching
    RunGetTimeAndCpy();
    file.write(msg.data(), msg.size());
}
