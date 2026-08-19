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
/// @file       tdcontext.h
/// @brief      time domain context
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/TimeDomain
/// module_path=/TimeSync/TimeDomain
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_TIME_DOMAIN_CONTEXT_H_
#define ARA_TSYNC_TIME_DOMAIN_CONTEXT_H_

#include <ara/core/vector.h>

#include <chrono>
#include <cstddef>
#include <cstdint>

#include "ara/tsync/clock.h"
#include "ara/tsync/internal/TimeDomainId.h"
#include "ara/tsync/internal/timestatus.h"
#include "ara/tsync/internal/timevalue.h"
#include "ara/tsync/internal/userdata.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timedomain {

/// @brief time domain context. Saves all information related to the time domain during the time synchronization process, including various E2E/P2P timestamps, etc.
class TDContext final
{
public:
    /// @brief constructor
    TDContext() noexcept
    {
        static_cast< void >(::memset(static_cast< void * >(this), 0, sizeof(*this)));
        SetStatusNotSynchronized();
    }

    /// @brief default destructor
    ~TDContext() = default;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    TDContext(TDContext const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    TDContext &operator=(TDContext const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    TDContext(TDContext &&other) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    TDContext &operator=(TDContext &&other) &noexcept = default;

    /// @brief get time domain ID
    /// @return time domain ID
    TimeDomainId const &GetDomainId() const noexcept { return domainId_; }

    /// @brief set time domain ID
    /// @param domainId time domain ID
    void SetDomainId(TimeDomainId const &domainId) noexcept { domainId_ = domainId; }

    /// @brief get user data
    /// @return user data
    internal::UserData const &GetUserData() const noexcept { return userData_; }

    /// @brief set user data
    /// @param ud - user data
    void SetUserData(internal::UserData const &ud) noexcept { userData_ = ud; }

    /// @brief get time status
    /// @return time status
    internal::TimeStatus const &GetStatus() const noexcept { return status_; }

    /// @brief set synchronization status to unsynchronized
    void SetStatusNotSynchronized() noexcept
    {
        status_.SetGlobalTimeBase(false);
        status_.SetSyncToGateway(false);
        status_.SetTimeout(false);
    }

    /// @brief check whether synchronization status is unsynchronized
    /// @return bool
    bool IsStatusNotSynchronized() const noexcept
    {
        if (status_.IsGlobalTimeBase()) {
            return false;
        }
        return true;
    }

    /// @brief set synchronization status to synchronized
    void SetStatusSynchronized() noexcept
    {
        status_.SetGlobalTimeBase(true);
        status_.SetSyncToGateway(false);
        status_.SetTimeout(false);
    }

    /// @brief check whether synchronization status is synchronized
    /// @return whether synchronized
    bool IsStatusSynchronized() const noexcept
    {
        if (status_.IsGlobalTimeBase() && !status_.IsSyncToGateway()) {
            return true;
        }
        return false;
    }

    /// @brief set synchronization status to sync to gateway only
    void SetStatusSyncToGateway() noexcept
    {
        status_.SetGlobalTimeBase(false);
        status_.SetSyncToGateway(true);
        status_.SetTimeout(false);
    }

    /// @brief check whether synchronization status is sync to gateway only
    /// @return whether sync to gateway only
    bool IsStatusSyncToGateway() const noexcept
    {
        if (!status_.IsGlobalTimeBase() && status_.IsSyncToGateway()) {
            return true;
        }
        return false;
    }

    /// @brief set synchronization status to timeout
    void SetStatusTimeout() noexcept
    {
        status_.SetGlobalTimeBase(false);
        status_.SetSyncToGateway(false);
        status_.SetTimeout(true);
        syncCount_ = 0U;
    }

    /// @brief check whether synchronization status is timeout
    /// @return whether synchronization status is timeout
    bool IsStatusTimeout() const noexcept
    {
        if (status_.IsGlobalTimeBase() && status_.IsTimeout()) {
            return true;
        }
        return false;
    }

    /// @brief get the deviation value calculated during the last synchronization
    /// @return the deviation value calculated during the last synchronization
    internal::TimeValue const &GetPreOffsetTS() const noexcept { return preOffsetTimestamp_; }

    /// @brief get time difference value
    /// @return time difference value
    internal::TimeValue const &GetOffsetTS() const noexcept { return offsetTimestamp_; }

    /// @brief set time deviation value
    /// @param ts - time deviation value
    void SetOffsetTS(internal::TimeValue const &ts) noexcept
    {
        preOffsetTimestamp_ = offsetTimestamp_;
        offsetTimestamp_    = ts;
    }

    /// @brief offset time domain sets time deviation value
    /// @param ts - time deviation value
    void SetOfsDomainOffsetTS(internal::TimeValue const &ts) noexcept
    {
        offsetTimestamp_    = ts;
        preOffsetTimestamp_ = offsetTimestamp_;
    }
    /// @brief get path delay time value
    /// @return path delay time value
    internal::TimeValue const GetDelayTS() const noexcept { return delayTimestamp_; }

    /// @brief set path delay time value
    /// @param ts - path delay time value
    void SetDelayTS(internal::TimeValue const &ts) noexcept { delayTimestamp_ = ts; }

    /// @brief get rate correction value
    /// @return rate correction value
    double GetRateDeviation() const noexcept { return rateCorrection_; }

    /// @brief set rate correction value
    /// @param r - rate correction value
    void SetRateDeviation(double const r) noexcept { rateCorrection_ = r; }

    // Sync T1
    /// @brief sync outbound timestamp
    /// @return sync outbound timestamp
    internal::TimeValue const &GetSyncEnTS() const noexcept { return syncEngressTimestamp_; }

    /// @brief set sync outbound timestamp
    /// @param ts sync outbound timestamp
    void SetSyncEnTS(internal::TimeValue const &ts) noexcept
    {
        syncEngressTimestamp_ = ts;
        syncCount_++;
    }

    /// @brief get sync inbound timestamp
    /// @return sync inbound timestamp
    internal::TimeValue const &GetSyncInTS() const noexcept { return syncIngressTimestamp_; }

    /// @brief set sync inbound timestamp
    /// @param ts sync inbound timestamp
    void SetSyncInTS(internal::TimeValue const &ts) noexcept { syncIngressTimestamp_ = ts; }

    /// @brief get timestamp correction field value
    /// @return timestamp correction field value
    std::uint64_t GetCorrectionField() const noexcept { return correctionField_; }

    /// @brief set timestamp correction field value
    /// @param cf timestamp correction field value
    void SetCorrectionField(std::uint64_t const cf) noexcept { correctionField_ = cf; }

    /// @brief get Pdelay outbound timestamp
    /// @return Pdelay outbound timestamp
    internal::TimeValue const &GetPdelayReqEnTS() const noexcept { return pdelayReqEngressTimestamp_; }

    /// @brief set Pdelay outbound timestamp
    /// @param ts Pdelay outbound timestamp
    void SetPdelayReqEnTS(internal::TimeValue const &ts) noexcept { pdelayReqEngressTimestamp_ = ts; }

    /// @brief get PdelayReq inbound timestamp
    /// @return PdelayReq inbound timestamp
    internal::TimeValue const &GetPdelayReqInTS() const noexcept { return pdelayReqIngressTimestamp_; }

    /// @brief set PdelayReq inbound timestamp
    /// @param ts PdelayReq inbound timestamp
    void SetPdelayReqInTS(internal::TimeValue const &ts) noexcept { pdelayReqIngressTimestamp_ = ts; }

    /// @brief get PdelayRes outbound timestamp
    /// @return PdelayRes outbound timestamp
    internal::TimeValue GetPdelayRespEnTS() const noexcept { return pdelayRespEngressTimestamp_; }

    /// @brief set PdelayRes outbound timestamp
    /// @param ts PdelayRes outbound timestamp
    void SetPdelayRespEnTS(internal::TimeValue const &ts) noexcept { pdelayRespEngressTimestamp_ = ts; }

    // Pdelay T3
    /// @brief get PdelayRes inbound timestamp
    /// @return PdelayRes inbound timestamp
    internal::TimeValue const &GetPdelayRespInTS() const noexcept { return pdelayRespIngressTimestamp_; }

    /// @brief set PdelayRes inbound timestamp
    /// @param ts PdelayRes inbound timestamp
    void SetPdelayRespInTS(internal::TimeValue const &ts) noexcept { pdelayRespIngressTimestamp_ = ts; }

    /// @brief get synchronization sequence number
    /// @return synchronization sequence number
    std::uint16_t GetSyncSeqId() const noexcept { return syncSeqId_; }

    /// @brief set synchronization sequence number
    /// @param num - sequence number
    void SetSyncSeqId(std::uint16_t const num) noexcept { syncSeqId_ = num; }

    /// @brief get Seq sequence number
    /// @return Seq sequence number
    std::uint16_t GetPdelaySeqId() const noexcept { return pdelaySeqId_; }

    /// @brief set PDelay sequence number
    /// @param num PDelay sequence number
    void SetPdelaySeqId(std::uint16_t const num) noexcept { pdelaySeqId_ = num; }

    /// @brief whether PdelayResp has timed out
    /// @return whether PdelayResp has timed out
    bool IsPdelayRespTimeout() const noexcept { return isPdelayRespTimeout_; }

    /// @brief set whether PdelayResp has timed out
    /// @param is whether PdelayResp has timed out
    void SetPdelayRespTimeout(bool const is) noexcept { isPdelayRespTimeout_ = is; }

    /// @brief whether PdelayRespFlp has timed out
    /// @return whether PdelayRespFlp has timed out
    bool IsPdelayRespFlpTimeout() const noexcept { return isPdelayRespFlpTimeout_; }

    /// @brief set whether PdelayRespFlp has timed out
    /// @param is whether PdelayRespFlp has timed out
    void SetPdelayRespFlpTimeout(bool const is) noexcept { isPdelayRespFlpTimeout_ = is; }

    /// @brief get time
    /// @return time
    Clock::time_point GetPreGlobalTime() const noexcept
    {
        /// Each time time is obtained, rate correction must be performed by multiplying the time interval since the last synchronization by the rate correction value.
        std::chrono::nanoseconds const lastTs{lastGlobalTimestamp_.ToChrono().count()};
        std::chrono::nanoseconds const nowTs{Clock::now().time_since_epoch().count()
                                             + GetPreOffsetTS().ToChrono().count()};

        /// Note: The rate correction done here is only used for Master (saving the value of SetRataCorrection()),
        /// not effective for Slave (under normal circumstances, Slave's RateCorrection is 0.0).
        std::int64_t const period{(nowTs - lastTs).count()};
        double const correctedOffset{static_cast< double >(period) * (GetRateDeviation() + 1.0)};
        return Clock::time_point{lastTs} + std::chrono::nanoseconds(static_cast< std::int64_t >(correctedOffset));
    }

    /// @brief get time
    /// @return time
    Clock::time_point GetGlobalTime() const noexcept
    {
        // Each time time is obtained, rate correction must be performed by multiplying the time interval since the last synchronization by the rate correction value.
        std::chrono::nanoseconds const lastTs{lastGlobalTimestamp_.ToChrono().count()};
        std::chrono::nanoseconds const nowTs{Clock::now().time_since_epoch().count()
                                             + GetOffsetTS().ToChrono().count()};
        std::int64_t const period{(nowTs - lastTs).count()};
        double const correctedOffset{static_cast< double >(period) * (GetRateDeviation() + 1.0)};
        std::chrono::nanoseconds const globalTime{lastTs.count() + static_cast< std::int64_t >(correctedOffset)};
        return Clock::time_point{globalTime};
    }

    /// @brief get time jump value
    /// @return time jump value
    double GetLeapJumpValue() const noexcept
    {
        return isoft::util::TimeNS2S(static_cast< double >((GetOffsetTS() - GetPreOffsetTS()).ToChrono().count()));
    }

    /// @brief convert stable time to global time
    /// @param ts stable time
    /// @return global time
    Clock::time_point Steady2GlobalTime(std::chrono::nanoseconds const ts) const noexcept
    {
        std::chrono::nanoseconds const lastSteadyTs{lastSteadyTimestamp_.ToChrono().count()};
        std::int64_t const period{(ts - lastSteadyTs).count()};
        double const correctedOffset{static_cast< double >(period) * (GetRateDeviation() + 1.0)};
        std::chrono::nanoseconds const gTime{lastGlobalTimestamp_.ToChrono().count()
                                             + static_cast< std::int64_t >(correctedOffset)};
        return Clock::time_point{gTime};
    }

    /// @brief set time
    /// @param tp - time
    void SetGlobalTime(Clock::time_point const &tp) noexcept
    {
        // Record the set timestamp
        Clock::time_point const nowTime{Clock::now()};
        lastGlobalTimestamp_.FromChrono(tp.time_since_epoch());

        lastSteadyTimestamp_.FromChrono(nowTime.time_since_epoch());
        std::chrono::nanoseconds const tsTarget{tp.time_since_epoch().count()};
        std::chrono::nanoseconds const tsNow{nowTime.time_since_epoch().count()};

        // Record the deviation between the time to be set and the current time; all subsequently distributed times will be based on local time + deviation.
        SetOffsetTS(internal::TimeValue(tsTarget - tsNow));
    }

    /// @brief set time
    /// @param lastGlobalTimestamp - the last set global time value
    /// @param lastSteadyTimestamp - the last set steady global time value
    void SetGlobalTimeStamp(internal::TimeValue const &lastGlobalTimestamp,
                            internal::TimeValue const &lastSteadyTimestamp) noexcept
    {
        // Record the set timestamp
        lastGlobalTimestamp_ = lastGlobalTimestamp;
        lastSteadyTimestamp_ = lastSteadyTimestamp;
    }

    /// @brief check whether it is a synchronization time domain
    /// @return whether it is a synchronization time domain
    bool IsSyncDomain() const noexcept { return domainId_.IsSyncDomain(); }

    /// @brief check whether it is an offset time domain
    /// @return whether it is an offset time domain
    bool IsOffsetDomain() const noexcept { return domainId_.IsOffsetDomain(); }

    /// @brief offset time base does not perform rrc calculation; it takes relevant parameters from the synchronization time domain
    /// @param tmTVsync - local virtual time saved when synchronization completion message is received
    /// @param syncJumpTs - used to mark the jump value of GlobalTime after this synchronization relative to before
    /// @param tmTLsync - GlobalTime when synchronization completion message is received, but before updating TG
    /// @param tmTG - received GlobalTime value
    /// @param dbRrc - calculated rate deviation value
    void GetMeasurePara(Clock::time_point &tmTVsync,
                        std::chrono::nanoseconds &syncJumpTs,
                        Clock::time_point &tmTLsync,
                        Clock::time_point &tmTG,
                        double &dbRrc) const noexcept
    {
        std::ignore = tmTVsync;
        std::ignore = syncJumpTs;
        std::ignore = tmTLsync;
        std::ignore = tmTG;
        std::ignore = dbRrc;
        tmTVsync    = tmTVsync_;
        syncJumpTs  = syncJumpTs_;
        tmTLsync    = tmTLsync_;
        tmTG        = tmTG_;
        dbRrc       = dbRrc_;
    }

    /// @brief synchronization time base calculates rrc, saves parameters in time domain
    /// @param tmTVsync - local virtual time saved when synchronization completion message is received
    /// @param syncJumpTs - used to mark the jump value of GlobalTime after this synchronization relative to before
    /// @param tmTLsync - GlobalTime when synchronization completion message is received, but before updating TG
    /// @param tmTG - received GlobalTime value
    /// @param dbRrc - calculated rate deviation value
    void SetMeasurePara(Clock::time_point const &tmTVsync,
                        std::chrono::nanoseconds const &syncJumpTs,
                        Clock::time_point const &tmTLsync,
                        Clock::time_point const &tmTG,
                        double const &dbRrc) noexcept
    {
        tmTVsync_   = tmTVsync;
        syncJumpTs_ = syncJumpTs;
        tmTLsync_   = tmTLsync;
        tmTG_       = tmTG;
        dbRrc_      = dbRrc;
        /// Only slave needs to modify rate deviation; master's rate deviation is set by time base provider and cannot be modified
        if (!isMaster_) {
            SetRateDeviation(dbRrc_ - 1.0);
        }
    }

    /// @brief whether time distribution is allowed
    /// @return whether time distribution is allowed

    bool AllowDispatch() const noexcept { return allowDisPatch_; }

    /// @brief set whether time distribution is allowed
    /// @param bAllow whether allowed
    void SetAllowDispatch(bool const bAllow) noexcept { allowDisPatch_ = bAllow; }

    /// @brief set whether it is a domain master
    /// @param isMaster whether it is a domain master
    void SetMasterFlag(bool const isMaster) noexcept { isMaster_ = isMaster; }

    /// @brief check whether synchronization has occurred twice
    /// @return whether synchronization has occurred twice
    bool HaveSyncOverTwice() const noexcept { return syncCount_ > 1U; }

    /// @brief return the time returned by followup + pathdelay
    internal::TimeValue GetGlobalTimeStamp() const noexcept { return lastGlobalTimestamp_; }

    /// @brief local time when sync was received
    internal::TimeValue GetSteadyTimeStamp() const noexcept { return lastSteadyTimestamp_; }

    /// @brief local time when sync was received
    void SetTlSyncTime(internal::TimeValue const &tlSyntime) noexcept { tlSyncTime_ = tlSyntime; }

    /// @brief local time when sync was received
    internal::TimeValue GetTlSyncTime() const noexcept { return tlSyncTime_; }

private:
    /// @name domainId_ - time domain ID
    internal::TimeDomainId domainId_{};

    /// @name userData_ - user data
    internal::UserData userData_{};

    /// @name status_ - time synchronization status
    internal::TimeStatus status_{};

    // If it is a synchronization time domain, it saves the difference between the calculated global time and the local virtual time.
    // If it is an offset time domain, it means the difference between the offset time domain and the underlying bound synchronization time domain.
    /// @name offsetTimestamp_
    internal::TimeValue offsetTimestamp_{};
    // Saves the deviation value calculated during the last synchronization, used to calculate PreGlobalTime.
    /// @name preOffsetTimestamp_
    internal::TimeValue preOffsetTimestamp_{};
    // The last set global time value, used to calculate the current global time value with additional rate correction
    /// @name lastGlobalTimestamp_
    internal::TimeValue lastGlobalTimestamp_{};
    // The steady clock value at the time of the last set global time, used to calculate the current global time value via steady clock
    /// @name lastSteadyTimestamp_
    internal::TimeValue lastSteadyTimestamp_{};
    // Calculated path delay time value.
    /// @name delayTimestamp_
    internal::TimeValue delayTimestamp_{};
    /// @name tlSyncTime_  TLSync
    internal::TimeValue tlSyncTime_{};
    // Rate correction value set for TimeMaster. Not applicable to Slave, and will be set to 0.0.
    // Value 0.xxx. (Actual correction calculation requires +1)
    /// @name rateCorrection_
    double rateCorrection_{0.0};

    // In the timestamp series, Master distributes globalmaster time (not Master's virtual local time), Slave's are all virtual local time.
    // Timestamp records
    // E2E
    /// @name syncEngressTimestamp_
    internal::TimeValue syncEngressTimestamp_{};  // T1
    /// @name syncIngressTimestamp_
    internal::TimeValue syncIngressTimestamp_{};  // T2
    /// @name correctionField_ from FollowupMesg
    std::uint64_t correctionField_{0U};
    // AUTOSAR does not use delay series messages.
    // internal::TimeValue delayReqEngressTimestamp_   //T3
    // internal::TimeValue delayReqIngressTimestamp_   //T4

    // P2P
    /// @name pdelayReqEngressTimestamp_
    internal::TimeValue pdelayReqEngressTimestamp_{};  // T1
    /// @name pdelayReqIngressTimestamp_
    internal::TimeValue pdelayReqIngressTimestamp_{};  // T2
    /// @name pdelayRespEngressTimestamp_
    internal::TimeValue pdelayRespEngressTimestamp_{};  // T3
    /// @name pdelayRespIngressTimestamp_
    internal::TimeValue pdelayRespIngressTimestamp_{};  // T4

    /// @traceid{PRS_TS_00187} SeqId starts from 0.
    /// @name syncSeqId_
    std::uint16_t syncSeqId_{0U};
    /// @name pdelaySeqId_
    std::uint16_t pdelaySeqId_{0U};
    // Used to detect whether timeout exists
    /// @name isPdelayRespTimeout_
    bool isPdelayRespTimeout_{false};
    /// @name isPdelayRespFlpTimeout_
    bool isPdelayRespFlpTimeout_{false};

    /// @name isMaster_
    bool isMaster_{false};

    /// @name tmTVsync_ local virtual time saved when synchronization completion message is received
    Clock::time_point tmTVsync_{};

    /// @name syncJumpTs_ used to mark the jump value of GlobalTime after this synchronization relative to before
    std::chrono::nanoseconds syncJumpTs_{0};

    /// @name tmTLsync_ GlobalTime when synchronization completion message is received, but before updating TG
    Clock::time_point tmTLsync_{};

    /// @name tmTG_ received GlobalTime value
    Clock::time_point tmTG_{};

    /// @name dbRrc_ calculated rate deviation value
    double dbRrc_{0};
    /// @name allowDisPatch_ whether master distributes time
    bool allowDisPatch_{false};
    /// @name syncCount_ number of synchronizations,
    std::uint64_t syncCount_{0U};

};  // class TDContext

}  // namespace timedomain
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  // ARA_TSYNC_TIME_DOMAIN_CONTEXT_H_
