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
/// @file       ecu_logger_common.cpp
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "ecu_logger_common.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

void Timer::Start()
{
    startTime_ = std::chrono::high_resolution_clock::now();
    isRunning_ = true;
}

std::int64_t Timer::GetDurationUs()
{
    if (isRunning_) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast< std::chrono::microseconds >(currentTime - startTime_).count();
    }
    std::int64_t dura = std::chrono::duration_cast< std::chrono::microseconds >(endTime_ - startTime_).count();
    _durationVec.push_back(dura);
    return dura;
}

void Timer::End()
{
    if (isRunning_) {
        endTime_   = std::chrono::high_resolution_clock::now();
        isRunning_ = false;
    }
}

std::int64_t Timer::GetDurationMs() const
{
    if (isRunning_) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast< std::chrono::milliseconds >(currentTime - startTime_).count();
    }
    return std::chrono::duration_cast< std::chrono::milliseconds >(endTime_ - startTime_).count();
}

std::string TimeHelper::getFormattedTime()
{
    auto now          = std::chrono::system_clock::now();
    auto now_time_t   = std::chrono::system_clock::to_time_t(now);
    std::tm *timeInfo = std::localtime(&now_time_t);

    auto duration     = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast< std::chrono::milliseconds >(duration).count() % 1000;

    std::ostringstream oss;
    oss << std::put_time(timeInfo, "%Y/%m/%d %H:%M:%S") << "." << std::setfill('0') << std::setw(3) << milliseconds;
    return oss.str();
}

std::string TimeHelper::getTimestampWithMicroseconds()
{
    auto now          = std::chrono::system_clock::now();
    auto duration     = now.time_since_epoch();
    auto microseconds = std::chrono::duration_cast< std::chrono::microseconds >(duration).count();

    std::time_t tt    = std::chrono::system_clock::to_time_t(now);
    std::tm *timeInfo = std::localtime(&tt);

    std::ostringstream oss;
    oss << std::put_time(timeInfo, "%Y/%m/%d %H:%M:%S") << "." << std::setw(6) << std::setfill('0')
        << (microseconds % 1000000);
    return oss.str();
}
