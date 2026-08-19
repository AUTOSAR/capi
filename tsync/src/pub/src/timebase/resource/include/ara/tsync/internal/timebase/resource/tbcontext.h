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
/// @file       tbcontext.h
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

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_TBCONTEXT_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_TBCONTEXT_H_

#include <cstdint>

#include "ara/tsync/internal/TimeDomainId.h"
#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/timebase/resource/name.h"
#include "ara/tsync/internal/timebase/resource/processlock.h"
#include "ara/tsync/internal/timebase/resource/timebaseid.h"
#include "ara/tsync/internal/timebase/resource/timecorrection.h"
#include "ara/tsync/internal/timebase/resource/timedata.h"
#include "ara/tsync/internal/timestatus.h"
#include "ara/tsync/internal/timevalue.h"
#include "ara/tsync/internal/userdata.h"
#include "ara/tsync/time_validation_measurement_types.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace resource {

/// @brief time base resource context
class TBContext final
{
public:
    /// @brief default constructor
    /// @param id - time domain ID
    /// @exception exception specification
    TBContext() = default;

    /// @brief initialize
    /// @param n - context name
    /// @param d - time domain ID
    /// @param p - whether it is a provider
    /// @param allow - whether rate correction is allowed
    /// @param corrInt - adaptation interval
    /// @param threshold - jump threshold
    /// @param isSteady - whether it is a stable clock
    void InitContext(ara::core::String const &n,
                     TimeBaseId const &d,
                     bool const &p,
                     bool const &allow,
                     std::uint64_t const corrInt,
                     std::uint64_t const threshold,
                     bool const isSteady) noexcept;

    /// @brief get the last set global time value
    /// @return the last set global time value
    internal::TimeValue LastGlobalTimestamp() const noexcept { return lastGlobalTimestamp_; };

    /// @brief get the last set global time value
    /// @return the last set global time value
    internal::TimeValue LastSteadyTimestamp() const noexcept { return lastSteadyTimestamp_; };

    /// @brief get current offset time value
    /// @return current offset time value
    internal::TimeValue RefTimeDataOffset() const noexcept { return refTimeData_.Offset(); };

    /// @brief set offset time value
    /// @param tV offset time value
    void TimeDataSetOffset(internal::TimeValue const &tV) noexcept { timeData_.SetOffset(tV); };

    /// @brief set user data
    /// @param userData user data
    void SetUserData(ara::core::Span< ara::core::Byte const > const &userData) noexcept;

    /// @brief check whether it is an offset time base
    /// @return whether it is an offset time base
    bool IsOffsetTimeBase() const noexcept { return id_.IsOffsetTimeBase(); };

    /// @brief check whether the time domain value is valid
    /// @return whether the time domain value is valid
    bool TimeDataDomainIdIsValid() const noexcept { return timeData_.DomainId().IsValid(); };

    /// @brief check whether the synchronization time domain value associated with the offset time base is valid
    /// @return whether the synchronization time domain value associated with the offset time base is valid
    bool RefTimeDataDomainIdIsValid() const noexcept { return refTimeData_.DomainId().IsValid(); };

    /// @brief check whether it is a synchronization time base
    /// @return whether it is a synchronization time base
    bool IsSyncTimeBase() const noexcept { return id_.IsSyncTimeBase(); };

    /// @brief check whether the time is synchronized
    /// @return whether the time is synchronized
    bool IsTimeSynced() const noexcept { return timeCorrection_.IsTimeSynced(); };

    /// @brief get current offset time value
    /// @return current offset time value
    internal::TimeValue TimeDataOffset() const noexcept { return internal::TimeValue(timeData_.Offset().ToChrono()); };

    /// @brief get rate deviation
    /// @return rate deviation
    double RateDeviation() const noexcept { return timeData_.RateDeviation(); };

    /// @brief get the rate deviation of the synchronization time base associated with the offset time base
    /// @return the rate deviation of the synchronization time base associated with the offset time base
    double RefTimeDataRateDeviation() const noexcept { return refTimeData_.RateDeviation(); };

    /// @brief get time status
    /// @return time status
    internal::TimeStatus const &GetTimeStatus() const noexcept { return timeData_.GetTimeStatus(); };

    /// @brief get user data
    /// @return user data
    internal::UserData const GetUserData() const noexcept { return timeData_.GetUserData(); }

    /// @brief get slave time measurement data
    /// @return slave time measurement data
    tsync::TimeSlaveMeasurementType const &GetSlaveTimingData() const noexcept;

    /// @brief set the last set global time value
    /// @param timePoint the last set global time value
    void SetLastGlobalTimestamp(internal::TimeValue const &timePoint) noexcept;

    /// @brief set the steady clock value at the time of the last set global time
    /// @param timePoint the steady clock value at the time of the last set global time
    void SetLastSteadyTimestamp(internal::TimeValue const &timePoint) noexcept;

    /// @brief check whether rate correction is allowed
    /// @return whether it is a synchronization time base
    bool AllowProviderRateCorrection() const noexcept { return allowProviderRateCorrection_; };

    /// @brief provider sets rate deviation
    /// @param rateDeviation rate deviation
    /// @return whether setting succeeded
    std::int32_t SyncProviderSetRateDeviation(const double &rateDeviation) noexcept;

    /// @brief check whether it is a valid offset time base provider
    /// @return 0 valid, otherwise invalid
    std::int32_t ValidateOffsetTimeBaseProvider() const noexcept;

    /// @brief check whether it is a valid synchronization time base provider
    /// @return 0 valid, otherwise invalid
    std::int32_t ValidateSynchronizedTimeBaseProvider() const noexcept;

    /// @brief consumer gets current time
    /// @return current time
    Timestamp GetConsumerCurrentTime() const noexcept;

    /// @brief synchronization time base provider gets current time
    /// @return current time
    Timestamp GetSyncProviderCurrentTime() const noexcept;

    /// @brief offset time base provider gets current time
    /// @return current time
    Timestamp GetOffsetCurrentTime() const noexcept;

    /// @brief offset time base provider sets offset time
    /// @param offsetTime offset time
    /// @param userData user data
    /// @return 0 setting succeeded
    std::int32_t SetOffsetTime(ara::tsync::Timestamp const &offsetTime,
                               ara::core::Span< ara::core::Byte const > const &userData) noexcept;

    /// @brief get rate deviation
    /// @return rate deviation
    double GetConsumerRateDeviation() const noexcept;

    /// @brief synchronization time base provider sets time
    /// @param timePoint time
    /// @param userData user data
    /// @return 0 setting succeeded
    std::int32_t SetTime(ara::tsync::Timestamp const &timePoint,
                         ara::core::Span< ara::core::Byte const > const &userData) noexcept;

    /// @brief synchronization time base provider updates time
    /// @param timePoint time
    /// @param userData user data
    /// @return 0 setting succeeded
    std::int32_t UpdateTime(ara::tsync::Timestamp const &timePoint,
                            ara::core::Span< ara::core::Byte const > const &userData) noexcept;

    /// @brief get master time measurement data
    /// @return master time measurement data
    tsync::TimeMasterMeasurementType const &GetMasterTimingData() const noexcept;

    /// @brief get path delay response measurement data
    /// @return path delay response measurement data
    tsync::PdelayResponderMeasurementType const &GetPdelayResponderData() const noexcept;

    /// @brief check whether it is a valid time base consumer
    /// @return whether it is a valid time base consumer
    bool IsValidSynchronizedTimeBaseConsumer() const noexcept
    {
        const bool validDomainId{timeData_.DomainId().IsValid()};
        return id_.IsSyncTimeBase() && validDomainId;
    }

    /// @brief get time base name
    /// @return reference to time base name
    Name const &GetName() const noexcept { return name_; };

    /// @brief get time base resource ID
    /// @return time base resource ID
    TimeBaseId const &GetId() const noexcept { return id_; };

    /// @brief check whether it is a time base provider type
    /// @return whether it is a time base provider type
    bool IsProvider() const noexcept { return isProvider_; };

    /// @brief get time data related to the current time base
    /// @return time data related to the current time base
    TimeData &GetTimeData() noexcept { return timeData_; };

    /// @brief get the time data of the synchronization time domain referenced by the offset time base
    /// @return time data of the synchronization time domain referenced by the offset time base
    TimeData &GetRefTimeData() noexcept { return refTimeData_; };

    /// @brief get time correction data
    /// @return time correction data
    TimeCorrection const &GetTimeCorrection() const noexcept { return timeCorrection_; };

    /// @brief save last global time
    /// @param tlSynctime last global time
    void SetTLSyncTime(Clock::time_point const tlSynctime) noexcept;

    /// @brief save global time
    /// @param gTime global time
    void SetGlobalTime(Clock::time_point const gTime) noexcept;

    /// @brief save path delay
    /// @param delay path delay
    void SetPathDelay(std::uint32_t const delay) noexcept;

    /// @brief get global time
    /// @return global time
    Clock::time_point GlobalTime() const noexcept;

    /// @brief get last global time
    /// @return last global time
    Clock::time_point GetTLSyncTime() const noexcept;

    /// @brief get path delay
    /// @return path delay
    std::uint32_t PathDelay() const noexcept;

    /// @brief save slave time measurement data
    /// @param data slave time measurement data
    void SetTimeSlaveMeasurementData(tsync::TimeSlaveMeasurementType const &data) noexcept;

    /// @brief save path delay measurement data
    /// @param data path delay measurement data
    void SetPdelayMeasurementData(tsync::PdelayInitiatorMeasurementType const &data) noexcept;

    /// @brief get path delay measurement data
    /// @return path delay measurement data
    tsync::PdelayInitiatorMeasurementType const &GetPdelayInitiatorData() const noexcept;

    /// @brief save path delay measurement data
    /// @param data master time measurement data
    void SetTimeMasterMeasurementData(tsync::TimeMasterMeasurementType const &data) noexcept;

    /// @brief save path delay response measurement data
    /// @param data path delay response measurement data
    void SetPdelayResponderData(tsync::PdelayResponderMeasurementType const &data) noexcept;

    /// @brief check whether it is a stable clock
    /// @return true if stable clock
    bool IsSteadyClock() const noexcept { return isSteadyClock_; }
    //
    /// @traceid  {SWS_TS_00050}
    /// @brief parallel rate correction measurement
    /// @param syncEnGlobal - global time distributed by this sync/follow
    /// @param syncInLocal - local inbound time of this sync
    /// @param rateCorrtNum - number of parallel measurements
    /// @param duration - measurement interval
    /// @return 0 rate correction measurement succeeded
    std::int32_t DoParaRCMeasurement(Clock::time_point const &syncEnGlobal,
                                     Clock::time_point const &syncInLocal,
                                     std::size_t const rateCorrtNum,
                                     double const duration) noexcept;

    /// @brief offset time base does not perform rrc calculation; it takes relevant parameters from the synchronization time domain
    /// @param tmTVsync - current global time
    /// @param syncJumpTs - set parameter
    /// @param tmTLsync - current global time
    /// @param tmTG - received global time
    /// @param dbRrc - calculation result
    void SetMeasurePara(Clock::time_point const &tmTVsync,
                        std::chrono::nanoseconds const &syncJumpTs,
                        Clock::time_point const &tmTLsync,
                        Clock::time_point const &tmTG,
                        double const &dbRrc) noexcept;

    /// @brief save local time when sync was received
    /// @param virualTime local time when sync was received
    void SetVirtualLocalTime(std::uint32_t const virualTime) noexcept;

    /// @brief get local time when sync was received
    /// @return local time when sync was received
    std::uint32_t GetVirtualLocalTime() const noexcept;

private:
    /// @name name - time base name
    Name name_{};

    /// @name id - time base resource ID
    TimeBaseId id_{};

    /// @name isProvider - whether it is a time base provider type
    bool isProvider_{false};

    /// @name allowProviderRateCorrection - whether rate correction is allowed for time base provider
    bool allowProviderRateCorrection_{false};

    /// @name tCorrInt - adaptation interval
    std::uint64_t tCorrInt_{0U};

    /// @name jumpThreshold - jump threshold
    std::uint64_t jumpThreshold_{0U};

    /// @name timeData - time data related to the current time base
    TimeData timeData_{};

    /// @name refTimeData - if this object is an offset time base, records the time data of the referenced synchronization time domain
    TimeData refTimeData_{};

    /// @name timeCorrection - calculate rrc, get time
    TimeCorrection timeCorrection_{};

    /// @name tlSyncTime_ - local global time when sync was received, TLSync for user process access
    Clock::time_point tlSyncTime_{};

    /// @name globalTime - global time distributed by followup, for user process access
    Clock::time_point globalTime_{};

    /// @name globalTime - local time when sync was received, for user process access
    std::uint32_t virtualLocalTime_{0};

    /// @name pathDelay - path delay time value, for user process access
    std::uint32_t pathDelay_{0U};

    /// @name tsmt - slave time measurement data, for user process access
    tsync::TimeSlaveMeasurementType tsmt_{};

    /// @name pimt - path delay measurement data, for user process access
    tsync::PdelayInitiatorMeasurementType pimt_{};

    /// @name tmmt - master time measurement data, for user process access
    tsync::TimeMasterMeasurementType tmmt_{};

    /// @name prmt - path delay response measurement data, for user process access
    tsync::PdelayResponderMeasurementType prmt_{};

    /// @name lastGlobalTimestamp  the last set global time value, used to calculate the current global time value after additional rate correction; offset stores synchronization related values
    internal::TimeValue lastGlobalTimestamp_{};

    /// @name lastSteadyTimestamp  the steady clock value at the time of the last set global time, used to calculate the current global time value via steady clock; offset stores synchronization related values
    internal::TimeValue lastSteadyTimestamp_{};

    /// @name isSteadyClock  whether it is a stable clock
    bool isSteadyClock_{false};

    /// @name mutexattr  mutex attribute
    pthread_mutexattr_t mutexattr_{};

    /// @name mutex  mutual exclusion between AA and tsyncd processes, only locks places that need mutual exclusion
    mutable pthread_mutex_t mutex_{};

};  /// class TBContext

}  // namespace resource
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  // ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_TBCONTEXT_H_
