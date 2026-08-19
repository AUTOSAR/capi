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
/// @file       tbcontext.cpp
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

#include "ara/tsync/internal/timebase/resource/tbcontext.h"

#include <cstdint>

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/config/common.h"
#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace resource {

/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log2Console(); }
/// @brief initialize
/// @param n - context name
/// @param d - time domain ID
/// @param p - whether it is a provider
/// @param allow - whether rate correction is allowed
/// @param corrInt - adaptation interval
/// @param threshold - jump threshold
/// @param isSteady - whether it is a stable clock
void TBContext::InitContext(ara::core::String const &n,
                            TimeBaseId const &d,
                            bool const &p,
                            bool const &allow,
                            std::uint64_t const corrInt,
                            std::uint64_t const threshold,
                            bool const isSteady) noexcept
{
    name_.FromString(n);
    id_                          = d;
    isProvider_                  = p;
    allowProviderRateCorrection_ = allow;
    tCorrInt_                    = corrInt;
    jumpThreshold_               = threshold;
    isSteadyClock_               = isSteady;

    std::ignore = pthread_mutexattr_init(&mutexattr_);
    std::ignore = pthread_mutexattr_setpshared(&mutexattr_, PTHREAD_PROCESS_SHARED);
    std::ignore = pthread_mutex_init(&(mutex_), &mutexattr_);
}

/// @brief set user data
/// @param userData user data
void TBContext::SetUserData(ara::core::Span< ara::core::Byte const > const &userData) noexcept
{
    internal::UserData data;
    data.FromByteSpan(userData);
    timeData_.SetUserData(data);
}

/// @brief get slave time measurement data
/// @return slave time measurement data
tsync::TimeSlaveMeasurementType const &TBContext::GetSlaveTimingData() const noexcept
{
    ProcessMutex const pMutex{mutex_};
    return tsmt_;
}

/// @brief set the last set global time value
/// @param timePoint the last set global time value
void TBContext::SetLastGlobalTimestamp(internal::TimeValue const &timePoint) noexcept
{
    ProcessMutex const pMutex{mutex_};
    lastGlobalTimestamp_ = timePoint;
}

/// @brief set the steady clock value at the time of the last set global time
/// @param timePoint the steady clock value at the time of the last set global time
void TBContext::SetLastSteadyTimestamp(internal::TimeValue const &timePoint) noexcept
{
    ProcessMutex const pMutex{mutex_};
    lastSteadyTimestamp_ = timePoint;
}

/// @brief provider sets rate deviation
/// @param rateDeviation rate deviation
/// @return whether setting succeeded
std::int32_t TBContext::SyncProviderSetRateDeviation(const double &rateDeviation) noexcept
{
    if (true != allowProviderRateCorrection_) {
        return kRET_E5;
    }
    if (isSteadyClock_) {
        /// When a stable clock, setting the rate requires updating the time, otherwise when the rate decreases, the acquired time may jump to the past
        ProcessMutex const pMutex{mutex_};
        std::ignore = SetTime(GetSyncProviderCurrentTime(), GetUserData().ToByteSpan());
    }
    timeData_.SetRateDeviation(rateDeviation);
    return 0;
}

/// @brief check whether it is a valid offset time base provider
/// @return 0 valid, otherwise invalid
std::int32_t TBContext::ValidateOffsetTimeBaseProvider() const noexcept
{
    if (true != id_.IsOffsetTimeBase()) {
        return kRET_E2;
    }
    if (true != timeData_.DomainId().IsValid()) {
        return kRET_E3;
    }
    if (true != refTimeData_.DomainId().IsValid()) {
        return kRET_E4;
    }
    return 0;
}

/// @brief check whether it is a valid synchronization time base provider
/// @return 0 valid, otherwise invalid
std::int32_t TBContext::ValidateSynchronizedTimeBaseProvider() const noexcept
{
    if (true != id_.IsSyncTimeBase()) {
        return kRET_E2;
    }
    if (true != timeData_.DomainId().IsValid()) {
        return kRET_E3;
    }
    return 0;
}

/// @brief consumer gets current time
/// @return current time
Timestamp TBContext::GetConsumerCurrentTime() const noexcept
{
    ProcessMutex const pMutex{mutex_};
    bool const validDomainId{timeData_.DomainId().IsValid()};
    bool const validRefDomainId{refTimeData_.DomainId().IsValid()};
    /// Synchronization time base
    if (id_.IsSyncTimeBase() && validDomainId) {
        if (!isSteadyClock_) {
            if (!timeCorrection_.IsTimeSynced()) {
                return ara::tsync::Timestamp{std::chrono::nanoseconds{Clock::now().time_since_epoch().count()}};
            }
            return timeCorrection_.GetSTBCorrectedTime(tCorrInt_, jumpThreshold_);
        }
        if (!timeCorrection_.IsTimeSynced()) {
            return ara::tsync::Timestamp(std::chrono::nanoseconds(-1));
        }
        return timeCorrection_.GetSTBCorrectedTime(tCorrInt_, jumpThreshold_);
        /// Offset time base
    }
    if (id_.IsOffsetTimeBase() && validRefDomainId) {
        if (!timeCorrection_.IsTimeSynced()) {
            std::chrono::nanoseconds nanoseds{Clock::now().time_since_epoch().count()
                                              + timeData_.Offset().ToChrono().count()
                                              + refTimeData_.Offset().ToChrono().count()};
            return ara::tsync::Timestamp{nanoseds};
        }
        return timeCorrection_.GetOTBCorrectedTime(tCorrInt_, jumpThreshold_, timeData_.Offset());
    }
    return ara::tsync::Timestamp(std::chrono::nanoseconds(-1));
}  // namespace resource

/// @brief synchronization time base provider gets current time
/// @return current time
Timestamp TBContext::GetSyncProviderCurrentTime() const noexcept
{
    ProcessMutex const pMutex{mutex_};
    std::chrono::nanoseconds const lastTs{lastGlobalTimestamp_.ToChrono().count()};
    std::int64_t const period{Clock::now().time_since_epoch().count() - lastSteadyTimestamp_.ToChrono().count()};
    double const dPeriod{static_cast< double >(period) * (RateDeviation() + 1.0)};
    std::chrono::nanoseconds const jumpTime{static_cast< std::int64_t >(dPeriod)};
    return ara::tsync::Timestamp(jumpTime + lastTs);
}

/// @brief offset time base provider gets current time
/// @return current time
Timestamp TBContext::GetOffsetCurrentTime() const noexcept
{
    ProcessMutex const pMutex{mutex_};
    std::chrono::nanoseconds const lastTs{lastGlobalTimestamp_.ToChrono().count()};
    std::int64_t const period{Clock::now().time_since_epoch().count() - lastSteadyTimestamp_.ToChrono().count()};
    double const dPeriod{static_cast< double >(period) * (RateDeviation() + 1.0)};
    std::chrono::nanoseconds const jumpTime{static_cast< std::int64_t >(dPeriod)};
    return ara::tsync::Timestamp(jumpTime + lastTs + timeData_.Offset().ToChrono());
}

/// @brief offset time base provider sets offset time
/// @param offsetTime offset time
/// @param userData user data
/// @return 0 setting succeeded
std::int32_t TBContext::SetOffsetTime(ara::tsync::Timestamp const &offsetTime,
                                      ara::core::Span< ara::core::Byte const > const &userData) noexcept
{
    timeData_.SetOffset(internal::TimeValue(offsetTime.time_since_epoch()));
    internal::UserData data;
    data.FromByteSpan(userData);
    timeData_.SetUserData(data);
    return 0;
}

/// @brief get rate deviation
/// @return rate deviation
double TBContext::GetConsumerRateDeviation() const noexcept
{
    bool const validDomainID{timeData_.DomainId().IsValid()};
    bool const validRefDomain{refTimeData_.DomainId().IsValid()};
    if (id_.IsSyncTimeBase() && validDomainID) {
        return timeData_.RateDeviation();
        /// Offset time base returns the rate deviation of the underlying synchronization time base
    }
    if (id_.IsOffsetTimeBase() && validRefDomain) {
        return refTimeData_.RateDeviation();
    }
    return 0.0;
}

/// @brief synchronization time base provider sets time
/// @param timePoint time
/// @param userData user data
/// @return 0 setting succeeded
std::int32_t TBContext::SetTime(ara::tsync::Timestamp const &timePoint,
                                ara::core::Span< ara::core::Byte const > const &userData) noexcept
{
    ProcessMutex const pMutex{mutex_};
    if (isSteadyClock_) {
        LOG().Error() << "SteadyClock cant Set Time";
        return -1;
    }
    Clock::time_point const nowTime{Clock::now()};
    timeData_.SetOffset(internal::TimeValue(timePoint.time_since_epoch() - nowTime.time_since_epoch()));
    internal::UserData data;
    data.FromByteSpan(userData);
    timeData_.SetUserData(data);
    lastGlobalTimestamp_.FromChrono(timePoint.time_since_epoch());
    lastSteadyTimestamp_.FromChrono(nowTime.time_since_epoch());
    return 0;
}

/// @brief synchronization time base provider updates time
/// @param timePoint time
/// @param userData user data
/// @return 0 setting succeeded
std::int32_t TBContext::UpdateTime(ara::tsync::Timestamp const &timePoint,
                                   ara::core::Span< ara::core::Byte const > const &userData) noexcept
{
    ProcessMutex const pMutex{mutex_};
    if (isSteadyClock_) {
        LOG().Error() << "SteadyClock cant Update Time";
        return -1;
    }
    Clock::time_point const nowTime{Clock::now()};
    timeData_.SetOffset(internal::TimeValue(timePoint.time_since_epoch() - nowTime.time_since_epoch()));
    internal::UserData data;
    data.FromByteSpan(userData);
    timeData_.SetUserData(data);
    lastGlobalTimestamp_.FromChrono(timePoint.time_since_epoch());
    lastSteadyTimestamp_.FromChrono(nowTime.time_since_epoch());
    return 0;
}

/// @brief get master time measurement data
/// @return master time measurement data
tsync::TimeMasterMeasurementType const &TBContext::GetMasterTimingData() const noexcept
{
    ProcessMutex const pMutex{mutex_};
    return tmmt_;
}

/// @brief get path delay response measurement data
/// @return path delay response measurement data
tsync::PdelayResponderMeasurementType const &TBContext::GetPdelayResponderData() const noexcept
{
    ProcessMutex const pMutex{mutex_};
    return prmt_;
}

/// @brief save last global time
/// @param preGtime last global time
void TBContext::SetTLSyncTime(Clock::time_point const tlSynctime) noexcept
{
    ProcessMutex const pMutex{mutex_};
    tlSyncTime_ = tlSynctime;
}

/// @brief save global time
/// @param gTime global time
void TBContext::SetGlobalTime(Clock::time_point const gTime) noexcept
{
    ProcessMutex const pMutex{mutex_};
    globalTime_ = gTime;
}

/// @brief save path delay
/// @param delay path delay
void TBContext::SetPathDelay(std::uint32_t const delay) noexcept
{
    ProcessMutex const pMutex{mutex_};
    pathDelay_ = delay;
}

/// @brief get global time
/// @return global time
Clock::time_point TBContext::GlobalTime() const noexcept
{
    ProcessMutex const pMutex{mutex_};
    return globalTime_;
}

/// @brief get last global time
/// @return last global time
Clock::time_point TBContext::GetTLSyncTime() const noexcept
{
    ProcessMutex const pMutex{mutex_};
    return tlSyncTime_;
}

/// @brief get path delay
/// @return path delay
std::uint32_t TBContext::PathDelay() const noexcept
{
    ProcessMutex const pMutex{mutex_};
    return pathDelay_;
}

/// @brief save slave time measurement data
/// @param data slave time measurement data
void TBContext::SetTimeSlaveMeasurementData(tsync::TimeSlaveMeasurementType const &data) noexcept
{
    ProcessMutex const pMutex{mutex_};
    tsmt_ = data;
}

/// @brief save path delay measurement data
/// @param data path delay measurement data
void TBContext::SetPdelayMeasurementData(tsync::PdelayInitiatorMeasurementType const &data) noexcept
{
    ProcessMutex const pMutex{mutex_};
    pimt_ = data;
}

/// @brief get path delay measurement data
/// @return path delay measurement data
tsync::PdelayInitiatorMeasurementType const &TBContext::GetPdelayInitiatorData() const noexcept
{
    ProcessMutex const pMutex{mutex_};
    return pimt_;
}

/// @brief save path delay measurement data
/// @param data master time measurement data
void TBContext::SetTimeMasterMeasurementData(tsync::TimeMasterMeasurementType const &data) noexcept
{
    ProcessMutex const pMutex{mutex_};
    tmmt_ = data;
}

/// @brief save path delay response measurement data
/// @param data path delay response measurement data
void TBContext::SetPdelayResponderData(tsync::PdelayResponderMeasurementType const &data) noexcept
{
    ProcessMutex const pMutex{mutex_};
    prmt_ = data;
}

//
/// @traceid  {SWS_TS_00050}
/// @brief parallel rate correction measurement
/// @param syncEnGlobal - global time distributed by this sync/follow
/// @param syncInLocal - local inbound time of this sync
/// @param rateCorrtNum - number of parallel measurements
/// @param duration - measurement interval
/// @return 0 rate correction measurement succeeded
std::int32_t TBContext::DoParaRCMeasurement(Clock::time_point const &syncEnGlobal,
                                            Clock::time_point const &syncInLocal,
                                            std::size_t const rateCorrtNum,
                                            double const duration) noexcept
{
    ProcessMutex const pMutex{mutex_};
    return timeCorrection_.DoParaRCMeasurement(syncEnGlobal, syncInLocal, rateCorrtNum, duration);
}

/// @brief offset time base does not perform rrc calculation; it takes relevant parameters from the synchronization time domain
/// @param tmTVsync - current global time
/// @param syncJumpTs - set parameter
/// @param tmTLsync - current global time
/// @param tmTG - received global time
/// @param dbRrc - calculation result
void TBContext::SetMeasurePara(Clock::time_point const &tmTVsync,
                               std::chrono::nanoseconds const &syncJumpTs,
                               Clock::time_point const &tmTLsync,
                               Clock::time_point const &tmTG,
                               double const &dbRrc) noexcept
{
    ProcessMutex const pMutex{mutex_};
    return timeCorrection_.SetMeasurePara(tmTVsync, syncJumpTs, tmTLsync, tmTG, dbRrc);
}

/// @brief save local time when sync was received
/// @param virualTime local time when sync was received
void TBContext::SetVirtualLocalTime(std::uint32_t const virualTime) noexcept
{
    ProcessMutex const pMutex{mutex_};
    virtualLocalTime_ = virualTime;
}
/// @brief get local time when sync was received
/// @return local time when sync was received
std::uint32_t TBContext::GetVirtualLocalTime() const noexcept
{
    ProcessMutex const pMutex{mutex_};
    return virtualLocalTime_;
}

}  // namespace resource
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara
