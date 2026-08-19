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
/// @file       base_tester.h
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef BASE_TESTER_H
#define BASE_TESTER_H

#include <ara/log/logger.h>

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include "ecu_logger_common.h"
// Base test utility class
class BaseTester
{
private:
    std::int64_t loopCount_;
    std::int64_t intervals_;
    std::string testMessage_ = "start----General test information--------General test information--------General test information---- end";
    std::int32_t loggerConsoleDuration_{-1};
    std::int32_t loggerFileDuration_{-1};
    std::int32_t loggerFileConsoleDuration_{-1};
    float coutDuration_{0.0};
    std::int32_t timestampedDuration_{-1};
    std::int32_t memcpyDuration_{-1};
    std::int32_t stdcpyDuration_{-1};
    float directWriteFile_{0.0};

public:
    BaseTester(std::int64_t loopCount, std::int64_t intervals);

    void runApiBenchMark();

    void RunGetTimeAndCpy();

    // Run Logger test
    std::int64_t RunLoggerTest(ara::log::Logger &logger);

    // Run idle test (only sleep)
    std::int32_t RunIdleTest();

    // Run standard output test
    std::int32_t RunCoutTest();

    // Run memcpy test
    std::int32_t RunMemCpyTest();

    // Run std::copy test
    std::int32_t RunStdCpyTest();

    // Run timestamped standard output test
    std::int32_t RunTimestampedCoutTest();

    // File write test using custom memory cache
    std::int32_t RunCustomCachedFileWriteTest(const std::string &filename, size_t cacheSize = 524288);

    // No cache, direct file write test
    std::int32_t RunDirectFileWriteTest(const std::string &filename);
};

#endif  // BASE_TESTER_H
