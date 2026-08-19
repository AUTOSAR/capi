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
/// @file       idsm_trans_limit.cpp
/// @brief      Security event remote transmission check class
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/qualified security event storage
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0013
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_trans_limit.h"

namespace ara {
namespace idsm {
/// @brief Check if traffic exceeds threshold
/// @param bytes Number of bytes to transmit
/// @param limitedCounter Number of times traffic has been limited
/// @return Returns false if traffic exceeds threshold, otherwise returns true
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
bool TrafficChecker::Check(uint64_t const bytes, uint8_t& limitedCounter) noexcept
{
    std::ignore = limitedCounter;
    std::chrono::steady_clock::duration const drNanSecond{std::chrono::steady_clock::now().time_since_epoch()};
    time_t const currentTimestamp{(std::chrono::duration_cast< std::chrono::seconds >(drNanSecond)).count()};
    ///@details First time interval
    if (lastTimestamp_ == 0) {
        lastTimestamp_ = currentTimestamp;
    }
    /// @details This transmission is a new time interval, update the start timestamp of the time interval
    if ((currentTimestamp - lastTimestamp_) >= timeInterval_) {
        lastTimestamp_ = currentTimestamp;
        bytesSum_      = 0U;
        limitedTimes_  = 0U;
    }

    limitedCounter = limitedTimes_;
    if ((bytesSum_ + bytes) <= maxBytes_) {
        bytesSum_ += bytes;
        return true;
    }

    if (limitedTimes_ == 0U) {
        limitedTimes_ += 1U;
    }
    return false;
}

/// @brief Check if rate exceeds threshold
/// @param events Number of events to transmit
/// @param limitedCounter
/// @return Returns false if rate exceeds threshold, otherwise returns true
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
bool RateChecker::Check(uint32_t const events, uint8_t& limitedCounter) noexcept
{
    std::ignore = limitedCounter;
    std::chrono::steady_clock::duration const drNanSecond{std::chrono::steady_clock::now().time_since_epoch()};
    time_t const currentTimestamp{(std::chrono::duration_cast< std::chrono::seconds >(drNanSecond)).count()};

    ///@details First time interval
    if (lastTimestamp_ == 0) {
        lastTimestamp_ = currentTimestamp;
    }
    /// @details This transmission is a new time interval, update the start timestamp of the time interval
    if ((currentTimestamp - lastTimestamp_) >= timeInterval_) {
        lastTimestamp_ = currentTimestamp;
        eventsSum_     = 0U;
        limitedTimes_  = 0U;
    }

    limitedCounter = limitedTimes_;
    if ((eventsSum_ + events) <= maxEvents_) {
        eventsSum_ += events;
        return true;
    }

    if (limitedTimes_ == 0U) {
        limitedTimes_ += 1U;
    }
    return false;
}

}  // namespace idsm
}  // namespace ara