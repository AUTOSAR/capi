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
/// @file       api_benchmark.h
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef LOG_TESTER_H
#define LOG_TESTER_H

#include <ara/log/logger.h>

#include <cstdint>
#include <mutex>
#include <string>

#include "ecu_logger_common.h"
// Log test utility class
class ApiBenchMark
{
private:
    std::int32_t loopCount_;
    std::string testMessage_ = "start----General test information--------General test information--------General test information---- end";
    std::mutex testMutex_;

public:
    // to increase the setting of level 1 and level 2 cache sizes
    ApiBenchMark(int loopCount);

    std::int32_t functionTenTime();

    std::int32_t functionOneTime();

    std::int32_t mutexLockTime();

    std::int32_t oneParemeter(std::string msg);

    void RunGetTimeAndCpy();

    std::int32_t clockGetTimeTest();

    std::int32_t cppSysTime();

    // Run Logger test
    std::int64_t RunLoggerTest(const std::string &ctxid);

    // Run standard output test
    std::int32_t RunCoutTest();

    // Run memcpy test
    std::int32_t RunMemCpyTest();

    // Run std::copy test
    std::int32_t RunStdCpyTest();

    // Run timestamped standard output test
    std::int32_t RunTimestampedCoutTest();

    std::int32_t RunCustomCachedFileWriteTest(const std::string &filename, size_t cacheSize);

    // No cache, direct file write test
    std::int32_t RunDirectFileWriteTest(const std::string &filename);
};

#endif  // LOG_TESTER_H
