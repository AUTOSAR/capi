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
/// @file       ecu_logger_common.h
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef ECU_LOGGER_COMMON_H
#define ECU_LOGGER_COMMON_H

#include <atomic>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

// Time measurement utility class
class Timer
{
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point endTime_;
    bool isRunning_;
    std::vector< std::int64_t > _durationVec;

public:
    // Constructor
    Timer() : isRunning_(false) {}

    // Start timing
    void Start();

    // Get time interval in microseconds
    std::int64_t GetDurationUs();

    // End timing
    void End();

    // Get time interval in milliseconds
    std::int64_t GetDurationMs() const;
};

// Time handling utility class
class TimeHelper
{
public:
    // Get formatted time string (with milliseconds)
    static std::string getFormattedTime();

    // Get timestamp with microseconds
    static std::string getTimestampWithMicroseconds();
};

#endif  // ECU_LOGGER_COMMON_H
