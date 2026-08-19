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
/// @file       timecorrection.cpp
/// @brief      time correction related operations
/// @details
/// @date       2023-02-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/timebase/resource/timecorrection.h"

#include <cstdint>

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

//
/// @traceid  {SWS_TS_00050}
/// @brief parallel rate correction measurement
/// @param syncEnGlobal - global time distributed by this sync/follow
/// @param syncInLocal - local inbound time of this sync
/// @param rateCorrtNum - number of parallel measurements
/// @param duration - measurement interval
/// @return 0 rate correction measurement succeeded
std::int32_t TimeCorrection::DoParaRCMeasurement(Clock::time_point const &syncEnGlobal,
                                                 Clock::time_point const &syncInLocal,
                                                 std::size_t const rateCorrtNum,
                                                 double const duration) noexcept
{
    if ((0U == rateCorrtNum) || isoft::util::IsEq0(duration)) {
        return _doRateCorrectionMeasurement(syncEnGlobal, syncInLocal);
    }
    tmTVsync_ = syncInLocal;
    tmTG_     = syncEnGlobal;
    if (false == timeSynced_) {
        tmTLsync_ = syncEnGlobal;
    } else {
        /// TLsync_ does not consider smoothing or jumping for now
        std::int64_t const period{(tmTVsync_ - tVstart_).count()};
        double const dPeriod{static_cast< double >(period) * dbRrc_};
        std::chrono::nanoseconds const tElaps{static_cast< std::int64_t >(dPeriod)};

        tmTLsync_ = tGstart_ + tElaps;
    }

    tGstop_ = tGstart_;
    tVstop_ = tVstart_;

    /// @traceid{SWS_TS_00046} At the start of rate correction measurement, save TGstart_ and TOstart snapshots.
    tGstart_    = syncEnGlobal;
    tVstart_    = syncInLocal;
    syncJumpTs_ = tmTG_ - tmTLsync_;
    if (!isConf_) {
        paraMeasure_.reserve(rateCorrtNum);
        _initParaRCMeasurement(syncEnGlobal, syncInLocal, rateCorrtNum, duration);
        return 0;
    }
    for (std::size_t i{0U}; i < paraMeasure_.size(); ++i) {
        bool const overBeginTime{syncEnGlobal >= paraMeasure_[i].beginTime};
        if ((!paraMeasure_[i].isConf) && (overBeginTime)) {
            paraMeasure_[i].realBeginTime = syncEnGlobal;
            paraMeasure_[i].tVStart       = syncInLocal;
            paraMeasure_[i].isConf        = true;
            return 0;
        }
    }
    double const perGap{duration / static_cast< double >(rateCorrtNum)};
    std::chrono::nanoseconds const calcMaxNs{static_cast< std::int64_t >(isoft::util::TimeS2NS(perGap))};
    bool const overCalcTime{tVstart_ < (calcTime_ + calcMaxNs)};
    if ((tVstart_ >= calcTime_) && (overCalcTime)) {
        // Rate deviation measurement (dbRrc) of synchronization time base.
        /// @traceid{SWS_TS_00042} Continuously perform rate correction measurement, ending means next start. And align with receiving time synchronization.
        /// @traceid{SWS_TS_00048} At the end of rate correction measurement, save TGstop_ and TVstop_ snapshots.
        Clock::time_point const paraTGstop{paraMeasure_[paraIndex_].realBeginTime};
        Clock::time_point const paraTVstop{paraMeasure_[paraIndex_].tVStart};

        /// @traceid{SWS_TS_00046} At the start of rate correction measurement, save TGstart_ and TOstart snapshots.
        Clock::time_point const paraTGstart{syncEnGlobal};
        Clock::time_point const paraTVstart{syncInLocal};

        /// @traceid{SWS_TS_00050} dbRrc calculation formula
        /// @traceid{SWS_TS_00051} Rrc must always remain available.

        // NOTE: This calculation formula has a problem: whenever GlobalTime changes, Rrc changes significantly.
        // Solution: Use a threshold to limit the growth of Rrc.
        std::int64_t const paraGPeriod{(paraTGstop - paraTGstart).count()};
        std::int64_t const paraVPeriod{(paraTVstop - paraTVstart).count()};
        double const r{static_cast< double >(paraGPeriod) * 1.0 / static_cast< double >(paraVPeriod)};
        if ((r > (internal::config::kUSER_RATE_MIN - internal::config::kUSER_CALC_VIBRATION))
            && (r < (internal::config::kUSER_RATE_MAX + internal::config::kUSER_CALC_VIBRATION))) {
            dbRrc_ = r;
        }

        /// @traceid {SWS_TS_00055}
        // For synchronization TBR, when the time base is synchronized within the snapshot range of TLSync and TVSync, the offset between its local instance and the global time base should be measured.
        syncJumpTs_                            = tmTG_ - tmTLsync_;
        timeSynced_                            = true;
        paraMeasure_[paraIndex_].realBeginTime = paraTGstart;
        paraMeasure_[paraIndex_].tVStart       = paraTVstart;
        if ((paraIndex_ + 1U) >= rateCorrtNum) {
            paraIndex_ = 0U;
        } else {
            paraIndex_ = paraIndex_ + 1U;
        }
        std::chrono::nanoseconds const durationNs{static_cast< std::int64_t >(isoft::util::TimeS2NS(duration))};
        calcTime_ = paraMeasure_[paraIndex_].tVStart + durationNs;
    } else if (tVstart_ < calcTime_) {
        /// @traceid{SWS_TS_00045} Measurement moment not reached, skip
    } else {
        /// Timeout, restart
        _initParaRCMeasurement(syncEnGlobal, syncInLocal, rateCorrtNum, duration);
    }
    return 0;
}

/// @brief synchronization time base gets time
/// @param tCorrInt - adaptation interval
/// @param jumpThreshold - jump threshold
/// @return calculated time
ara::tsync::Timestamp TimeCorrection::GetSTBCorrectedTime(std::uint64_t const tCorrInt,
                                                          std::uint64_t const jumpThreshold) const noexcept
{
    /// @traceid {SWS_TS_00056}
    /// If abs(tmTG - tmTLsync_) >= TimeSyncCorrection.offsetCorrectionJumpThreshold.
    /// Then calculate according to the formula: TL = tmTG + (tV - tmTVsync_) * dbRrc

    /// @traceid {SWS_TS_00057}
    /// @traceid {SWS_TS_00058}
    /// @traceid {SWS_TS_00059}
    /// If abs(tmTG-TLSync） < TimeSyncCorrection.offsetCorrectionJumpThreshold .
    /// During the period defined by parameter TimeSyncCorrection.offsetCorrectionAdaptionInterval.
    /// The calculation formula for roc is as follows: roc = (tmTG - tmTLsync_) / (TCorrInt) + 1
    /// TL = tmTLsync_ + (rrc * (tV - tmTVsync_) * roc).
    /// After exceeding the TimeSyncCorrection.offsetCorrectionAdaptionInterval period
    /// Calculate the corrected time (TL) of the local instance of the time base according to SWS_TS_00056.

    /// @traceid {SWS_TS_00060}
    // If TimeSyncCorrection.offsetCorrectionJumpThreshold is set to 0, offset correction only uses jump correction.

    std::chrono::seconds const steadyElapse{
        std::move(std::chrono::duration_cast< std::chrono::seconds >(Clock::now() - tmTVsync_))};
    std::uint64_t const nowOffsetSync{static_cast< std::uint64_t >(steadyElapse.count())};
    std::chrono::seconds const syncJump{std::chrono::duration_cast< std::chrono::seconds >(syncJumpTs_).count()};
    std::int64_t const jump{static_cast< std::int64_t >(syncJump.count())};
    bool const inCorrInt{(static_cast< double >(nowOffsetSync) * dbRrc_) < static_cast< double >(tCorrInt)};
    if ((static_cast< std::uint32_t >(std::abs(jump)) < jumpThreshold) && (inCorrInt)) {
        // Offset rate adaptive correction + rate correction
        double const roc{static_cast< double >(syncJumpTs_.count()) / (static_cast< double >(tCorrInt) * 1e9) + 1.0};
        Clock::time_point const tV{std::chrono::nanoseconds(Clock::now().time_since_epoch().count())};
        double const vPeriod{dbRrc_ * static_cast< double >((tV - tmTVsync_).count()) * roc};
        std::chrono::nanoseconds const rateAptCorrTs{static_cast< std::int64_t >(vPeriod)};
        std::chrono::nanoseconds const nseds{tmTLsync_.time_since_epoch().count()};
        return ara::tsync::Timestamp(nseds + rateAptCorrTs);
    }
    // Offset jump correction + rate correction
    Clock::time_point const tV{std::chrono::nanoseconds(Clock::now().time_since_epoch().count())};
    double const rateCorrTs{static_cast< double >((tV - tmTVsync_).count()) * dbRrc_};
    std::chrono::nanoseconds const nseds{tmTG_.time_since_epoch().count()};
    ara::tsync::Timestamp ts{nseds + std::chrono::nanoseconds(static_cast< std::int64_t >(rateCorrTs))};
    return ts;
}

/// @brief offset time base gets time
/// @param tCorrInt - adaptation interval
/// @param jumpThreshold - jump threshold
/// @param oTBOffset - deviation of offset time domain relative to synchronization domain
/// @return calculated time
ara::tsync::Timestamp TimeCorrection::GetOTBCorrectedTime(std::uint64_t const tCorrInt,
                                                          std::uint64_t const jumpThreshold,
                                                          internal::TimeValue const &oTBOffset) const noexcept
{
    // Offset time base GetCurrentTime() = OffsetTime + syncTimebase.GetCurrentTime().
    /// @traceid {SWS_TS_00071}
    /// @traceid {SWS_TS_00052}
    /// Offset time base must not perform rate correction; use the rate correction of the underlying synchronization time base.

    Clock::time_point const tV{Clock::now()};
    std::chrono::seconds const steadyElapse{std::chrono::duration_cast< std::chrono::seconds >(tV - tmTVsync_).count()};
    std::uint64_t const nowOffsetSync{static_cast< std::uint64_t >(steadyElapse.count())};
    std::int64_t const jump{
        static_cast< std::int64_t >(std::chrono::duration_cast< std::chrono::seconds >(syncJumpTs_).count())};
    bool const inGap{(static_cast< double >(nowOffsetSync) * dbRrc_) < static_cast< double >(tCorrInt)};
    if ((static_cast< std::uint64_t >(std::abs(jump)) < jumpThreshold) && (inGap)) {
        // Offset rate adaptive correction + rate correction
        double const roc{static_cast< double >(syncJumpTs_.count()) / (static_cast< double >(tCorrInt) * 1e9) + 1.0};
        std::int64_t const vPeriod{(tV - tmTVsync_).count()};
        double const dPeriod{dbRrc_ * static_cast< double >(vPeriod) * roc};
        std::chrono::nanoseconds const rateAptCorrTs{static_cast< std::int64_t >(dPeriod)};
        std::chrono::nanoseconds const nseds{tmTLsync_.time_since_epoch().count()};
        return ara::tsync::Timestamp(nseds + rateAptCorrTs + oTBOffset.ToChrono());
    }
    // Offset jump correction + rate correction
    std::int64_t const vPeriod{(tV - tmTVsync_).count()};
    double const rateCorrTs{static_cast< double >(vPeriod) * dbRrc_};
    std::chrono::nanoseconds const nseds{tmTG_.time_since_epoch().count()};
    ara::tsync::Timestamp ts{nseds + std::chrono::nanoseconds(static_cast< std::int64_t >(rateCorrTs))
                             + oTBOffset.ToChrono()};
    return ts;
}

/// @traceid  {SWS_TS_00044}
/// @brief initialize parallel rate correction measurement
/// @param globalTime - current inbound global time
/// @param syncInLocal - current inbound local time
/// @param rateCorrtNum - number of parallel measurements
/// @param duration - measurement interval
/// @return 0 rate correction measurement succeeded
void TimeCorrection::_initParaRCMeasurement(Clock::time_point const &globalTime,
                                            Clock::time_point const &syncInLocal,
                                            std::size_t const rateCorrtNum,
                                            double const duration) noexcept
{
    /// @traceid{SWS_TS_00044} The number of parallel calculations is determined by the parameter 'TimeSyncCorrection.rateCorrectionsPerMeasurementDuration'
    /// @traceid{SWS_TS_00045} Each offset of parallel calculation is evenly distributed within the measurement interval starting from the first time.
    for (std::size_t i{0U}; i < rateCorrtNum; i++) {
        double const perDuation{(duration / static_cast< double >(rateCorrtNum)) * static_cast< double >(i)};
        std::chrono::nanoseconds const durationNs{static_cast< std::int64_t >(isoft::util::TimeS2NS(perDuation))};
        paraMeasure_[i].beginTime = globalTime + durationNs;
        paraMeasure_[i].isConf    = false;
        paraMeasure_[i].tVStart   = syncInLocal + durationNs;
    }
    paraMeasure_[0U].realBeginTime = paraMeasure_[0U].beginTime;
    std::chrono::nanoseconds const calcNs{static_cast< std::int64_t >(isoft::util::TimeS2NS(duration))};
    paraMeasure_[0U].tVStart = syncInLocal;
    paraMeasure_[0U].isConf  = true;
    /// @traceid SWS_TS_00043 Parallel calculation intervals need to use stable time to prevent global time jumps
    calcTime_  = paraMeasure_[0U].tVStart + calcNs;
    isConf_    = true;
    paraIndex_ = 0U;
}

/// @traceid  {SWS_TS_00050}
/// @brief rate correction measurement
/// @param syncEnGlobal - global time distributed by this sync/follow
/// @param syncInLocal - local inbound time of this sync
/// @return 0 rate correction measurement succeeded
std::int32_t TimeCorrection::_doRateCorrectionMeasurement(Clock::time_point const &syncEnGlobal,
                                                          Clock::time_point const &syncInLocal) noexcept
{
    // Offset domain does not perform measurement, data is obtained from synchronization domain

    tmTG_     = syncEnGlobal;
    tmTVsync_ = syncInLocal;

    // Rate deviation measurement (dbRrc) of synchronization time base.
    /// @traceid{SWS_TS_00042} Continuously perform rate correction measurement, ending means next start. And align with receiving time synchronization.
    /// @traceid{SWS_TS_00048} At the end of rate correction measurement, save TGstop_ and TVstop_ snapshots.
    tGstop_ = tGstart_;
    tVstop_ = tVstart_;

    if (false == timeSynced_) {
        tmTLsync_ = syncEnGlobal;
    } else {
        std::int64_t const period{(tmTVsync_ - tVstart_).count()};
        double const dPeriod{static_cast< double >(period) * dbRrc_};
        std::chrono::nanoseconds const tElaps{static_cast< std::int64_t >(dPeriod)};
        tmTLsync_ = tGstart_ + tElaps;
    }

    /// @traceid{SWS_TS_00046} At the start of rate correction measurement, save TGstart_ and TOstart snapshots.
    tGstart_ = syncEnGlobal;
    tVstart_ = syncInLocal;
    /// @traceid{SWS_TS_00050} dbRrc calculation formula
    /// @traceid{SWS_TS_00051} Rrc must always remain available.
    // NOTE: Whenever GlobalTime changes, Rrc changes significantly.
    // Solution: Use a threshold to limit the growth of Rrc
    std::int64_t const gPeriod{(tGstop_ - tGstart_).count()};
    std::int64_t const vPeriod{(tVstop_ - tVstart_).count()};
    double const r{static_cast< double >(gPeriod) * 1.0 / static_cast< double >(vPeriod)};
    if ((r > (internal::config::kUSER_RATE_MIN - internal::config::kUSER_CALC_VIBRATION))
        && (r < (internal::config::kUSER_RATE_MAX + internal::config::kUSER_CALC_VIBRATION))) {
        dbRrc_ = r;
    }

    /// @traceid {SWS_TS_00055}
    // For synchronization TBR, when the time base is synchronized within the snapshot range of TLSync and TVSync, the offset between its local instance and the global time base should be measured.
    syncJumpTs_ = tmTG_ - tmTLsync_;
    timeSynced_ = true;

    return 0;
}

}  // namespace resource
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara