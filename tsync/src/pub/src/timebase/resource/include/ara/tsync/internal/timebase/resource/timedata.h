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
/// @file       timedata.h
/// @brief      time base resource context
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/TimeBase
/// module_path=/TimeSync/TimeBase
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_TIMEDATA_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_TIMEDATA_H_

#include <cstdint>

#include "ara/tsync/internal/TimeDomainId.h"
#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/timebase/resource/processlock.h"
#include "ara/tsync/internal/timestatus.h"
#include "ara/tsync/internal/timevalue.h"
#include "ara/tsync/internal/userdata.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace resource {

/// @brief time related data
class TimeData final
{
public:
    /// @brief default constructor
    TimeData() = default;

    /// @brief copy constructor
    /// @param    other
    /// @returns object reference
    TimeData &operator=(TimeData const &other) = default;

    /// @brief move constructor
    /// @param    other
    /// @returns object reference
    TimeData &operator=(TimeData &&other) = default;

    /// @brief copy constructor
    /// @param other - other object
    TimeData(TimeData const &other) = default;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    TimeData(TimeData &&) noexcept = default;
    ~TimeData()                    = default;

    /// @brief constructor with parameters
    /// @param id - time domain ID
    void Init(internal::TimeDomainId const &id) noexcept
    {
        domainId_   = id;
        std::ignore = pthread_mutexattr_init(&mutexattr_);
        std::ignore = pthread_mutexattr_setpshared(&mutexattr_, PTHREAD_PROCESS_SHARED);
        std::ignore = pthread_mutex_init(&(mutex_), &mutexattr_);
    }
    /// @brief get time domain ID
    /// @returns time domain ID
    internal::TimeDomainId DomainId() const noexcept { return domainId_; };

    /// @brief get current offset time value
    /// @returns current offset time value
    internal::TimeValue Offset() const noexcept
    {
        ProcessMutex const pMutex{mutex_};
        return offset_;
    }

    /// @brief set current offset time value
    /// @param val - current offset time value
    void SetOffset(internal::TimeValue const &val) noexcept
    {
        ProcessMutex const pMutex{mutex_};
        offset_ = val;
    }

    /// @brief get user data
    /// @returns user data
    internal::UserData GetUserData() const noexcept
    {
        ProcessMutex const pMutex{mutex_};
        return userData_;
    }

    /// @brief set user data
    /// @param data - user data
    void SetUserData(internal::UserData const data) noexcept
    {
        ProcessMutex const pMutex{mutex_};
        userData_ = data;
    }

    /// @brief get time status
    /// @returns time status
    internal::TimeStatus const &GetTimeStatus() const noexcept
    {
        ProcessMutex const pMutex{mutex_};
        return timeStatus_;
    }

    /// @brief set time status
    /// @param status - time status
    void SetTimeStatus(internal::TimeStatus const &status) noexcept
    {
        ProcessMutex const pMutex{mutex_};
        timeStatus_ = status;
    }

    /// @brief get rate deviation
    /// @returns rate deviation
    double RateDeviation() const noexcept
    {
        ProcessMutex const pMutex{mutex_};
        return rateDeviation_;
    }

    /// @brief set rate deviation
    /// @param val - rate deviation
    void SetRateDeviation(double const val) noexcept
    {
        ProcessMutex const pMutex{mutex_};
        rateDeviation_ = val;
    }

    /// @brief get none jump actual count
    /// @returns none jump actual count
    std::uint32_t TimeLeapHealingRealCounter() const noexcept { return timeLeapHealingRealCounter_; };

    /// @brief set none jump actual count
    /// @param val - none jump actual count
    void SetTimeLeapHealingRealCounter(std::uint32_t const val) noexcept { timeLeapHealingRealCounter_ = val; };

    /// @brief whether the jump is triggered by synchronization domain or offset domain
    /// @returns whether the jump is triggered by synchronization domain or offset domain
    bool TimeLeapbyOwnDomain() const noexcept { return timeLeapbyOwnDomain_; };

    /// @brief set whether the jump is triggered by synchronization domain or offset domain
    /// @param val - whether the jump is triggered by synchronization domain or offset domain
    void SetTimeLeapbyOwnDomain(bool const val) noexcept { timeLeapbyOwnDomain_ = val; };

    /// @brief configuration parameter pHealingCounter
    /// @returns configuration parameter pHealingCounter
    std::uint32_t TimeLeapHealingCounter() const noexcept { return timeLeapHealingCounter_; };

    /// @brief set configuration parameter pHealingCounter
    /// @param val - configuration parameter pHealingCounter
    void SetTimeLeapHealingCounter(std::uint32_t const val) noexcept { timeLeapHealingCounter_ = val; };

private:
    /// @name domainId - time domain ID
    internal::TimeDomainId domainId_;

    /// @name offset - offset time value (ns) stored by offset clock
    internal::TimeValue offset_;

    /// @name userData - user data
    internal::UserData userData_;

    /// @name timeStatus - time status
    internal::TimeStatus timeStatus_;

    /// @name rateDeviation - calculated rate deviation between the current ECU clock and the Master clock
    double rateDeviation_{0.0};

    /// @name timeLeapHealingRealCounter - actual count
    std::uint32_t timeLeapHealingRealCounter_{0U};

    /// @name timeLeapbyOwnDomain - offset time base determines whether the jump is triggered by synchronization domain or offset domain
    bool timeLeapbyOwnDomain_{true};

    /// @name timeLeapHealingCounter - configuration parameter
    std::uint32_t timeLeapHealingCounter_{0U};

    /// @name mutexattr  mutex attribute
    pthread_mutexattr_t mutexattr_{};

    /// @name mutex  mutual exclusion between AA and tsyncd processes, only locks places that need mutual exclusion
    mutable pthread_mutex_t mutex_{};
};

}  // namespace resource
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  // ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_TIMEDATA_H_
