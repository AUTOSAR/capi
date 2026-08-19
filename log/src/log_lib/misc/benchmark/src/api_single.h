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
/// @file       api_single.h
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef LOG_single_aspi_H
#define LOG_single_aspi_H

#include <ara/log/logger.h>

#include <cstdint>
#include <mutex>
#include <string>
#include <tuple>

#include "ecu_logger_common.h"
// Log test utility class
class ApiSingle
{
private:
    std::int32_t loopCount_;
    std::string testMessage_ = "start----General test information--------General test information--------General test information---- end";
    std::mutex testMutex_;

public:
    ApiSingle();
    void RunIdleTest(std::string something) { std::ignore = something; };
    void RunGetTimeAndCpy();
    // Run Logger test
    void RunLoggerTest(ara::log::Logger &logger);

    // Run standard output test
    void RunCoutTest();

    // Run memcpy test
    void RunMemCpyTest();

    // Run std::copy test
    void RunStdCpyTest();

    // Run timestamped standard output test
    void RunTimestampedCoutTest();

    void RunCustomCachedFileWriteTest(const std::string &filename, size_t cacheSize = 128 * 1024);

    // No cache, direct file write test
    void RunDirectFileWriteTest(const std::string &filename);
};

#endif  // LOG_single_aspi_H
