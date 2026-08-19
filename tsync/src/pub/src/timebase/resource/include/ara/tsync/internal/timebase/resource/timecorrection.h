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
/// @file       timecorrection.h
/// @brief      time correction related operations, including rate correction/offset jump correction/offset adaptive correction.
/// @details
/// @date       2022-05-25
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_TSYNC_TIME_CORRECTION_H_
#define _ARA_TSYNC_TIME_CORRECTION_H_

#include <chrono>
#include <cmath>

#include "ara/core/vector.h"
#include "ara/tsync/clock.h"
#include "ara/tsync/internal/timevalue.h"
#include "ara/tsync/timestamp.h"
#include "isoft/util/mix.h"
#include "tcpoint.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace resource {

/// @brief  time correction, synchronization time base calculates rrc, synchronization/offset time base gets time
class TimeCorrection
{
public:
    /// @brief default constructor
    /// @exception exception specification
    TimeCorrection() = default;

    /// @brief default constructor
    ~TimeCorrection() = default;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    TimeCorrection(TimeCorrection const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    TimeCorrection &operator=(TimeCorrection const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    TimeCorrection(TimeCorrection &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    TimeCorrection &operator=(TimeCorrection &&) &noexcept = default;

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

    /// @brief get rate deviation
    /// @return rate deviation
    double GetRateDeviation() const noexcept
    {
        /// @traceid {SWS_TS_00070} If not yet calculated, should return 0.0. // Because the default value is 1.0, returning Rrc-1 is naturally 0.0.
        /// @traceid {SWS_TS_00053} Return value Rrc - 1
        return dbRrc_ - 1.0;
    }

    /// @brief get flag: whether time has been synchronized
    /// @return whether time has been synchronized
    bool IsTimeSynced() const noexcept { return timeSynced_; }

    /// @brief synchronization time base gets time
    /// @param tCorrInt - adaptation interval
    /// @param jumpThreshold - jump threshold
    /// @return calculated time
    ara::tsync::Timestamp GetSTBCorrectedTime(std::uint64_t const tCorrInt,
                                              std::uint64_t const jumpThreshold) const noexcept;

    /// @brief offset time base gets time
    /// @param tCorrInt - adaptation interval
    /// @param jumpThreshold - jump threshold
    /// @param oTBOffset - deviation of offset time domain relative to synchronization domain
    /// @return calculated time
    ara::tsync::Timestamp GetOTBCorrectedTime(std::uint64_t const tCorrInt,
                                              std::uint64_t const jumpThreshold,
                                              internal::TimeValue const &oTBOffset) const noexcept;

    /// @brief synchronization time base calculates rrc, saves parameters in time domain
    /// @param tmTVsync - current global time
    /// @param syncJumpTs - set parameter
    /// @param tmTLsync - current global time
    /// @param tmTG - received global time
    /// @param dbRrc - calculation result
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

        tmTVsync   = tmTVsync_;
        syncJumpTs = syncJumpTs_;
        tmTLsync   = tmTLsync_;
        tmTG       = tmTG_;
        dbRrc      = dbRrc_;
    }

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
                        double const &dbRrc) noexcept
    {
        tmTVsync_   = tmTVsync;
        syncJumpTs_ = syncJumpTs;
        tmTLsync_   = tmTLsync;
        tmTG_       = tmTG;
        dbRrc_      = dbRrc;
        timeSynced_ = true;
    }

private:
    /// @traceid  {SWS_TS_00044}
    /// @brief initialize parallel rate correction measurement
    /// @param globalTime - current global time
    /// @param syncInLocal - current inbound local time
    /// @param rateCorrtNum - number of parallel measurements
    /// @param duration - measurement interval
    /// @return 0 rate correction measurement succeeded
    void _initParaRCMeasurement(Clock::time_point const &globalTime,
                                Clock::time_point const &syncInLocal,
                                std::size_t const rateCorrtNum,
                                double const duration) noexcept;

    /// @traceid  {SWS_TS_00050}
    /// @brief rate correction measurement
    /// @param syncEnGlobal - global time distributed by this sync/follow
    /// @param syncInLocal - local inbound time of this sync
    /// @return 0 rate correction measurement succeeded
    std::int32_t _doRateCorrectionMeasurement(Clock::time_point const &syncEnGlobal,
                                              Clock::time_point const &syncInLocal) noexcept;

private:
    // Variable name meaning: lowercase suffix indicates the time point when acquired, uppercase prefix indicates meaning.
    // tmTG       Global Time global time value received when synchronization is completed. (Actually this value is calculated during the synchronization process).
    // TL       Local instance of Global Time GlobalTime calculated before the new time synchronization arrives (calculated from TV after offset and rate correction)
    // tV       Virtual local Time, continuous time obtained by chrono::steady_clock::now().
    // Timestamps, used to calculate rate and offset.
    /// @name TGstart_ GlobalTime measurement start time snapshot
    Clock::time_point tGstart_{};

    /// @name TGstop_ GlobalTime measurement end time snapshot
    Clock::time_point tGstop_{};

    /// @name TVstart_ VirtualLocalTime measurement start time snapshot
    Clock::time_point tVstart_{};

    /// @name TVstop_ VirtualLocalTime measurement end time snapshot
    Clock::time_point tVstop_{};

    /// @name tmTVsync_ local virtual time saved when synchronization completion message is received.
    Clock::time_point tmTVsync_{};

    /// @name tmTLsync_ GlobalTime when synchronization completion message is received, but before updating TG. (Actually the GlobalTime calculated based on the previous synchronization offset)
    Clock::time_point tmTLsync_{};

    /// @name tmTG_ received GlobalTime value. (Actually the GlobalTime calculated from the current synchronization offset)
    Clock::time_point tmTG_{};

    // Default set to 1.0, indicating no deviation and can be directly used in calculations.
    /// @name dbRrc_ calculated rate deviation value, value 1.xxxx
    double dbRrc_{1.0};

    /// @name syncJumpTs_ used to mark the jump value of GlobalTime after this synchronization relative to before.
    std::chrono::nanoseconds syncJumpTs_{0};

    /// @name timeSynced_ marks whether time synchronization has been performed; if not, time correction is not performed.
    bool timeSynced_{false};

    /// @name paraMeasure_ parallel calculation measurement data set
    ara::core::Vector< TCPoint > paraMeasure_{};

    /// @name calcTime_ the value that the next calculation should reach
    Clock::time_point calcTime_{};

    /// @name paraIndex_ the Nth parallel calculation next
    std::size_t paraIndex_{0};

    /// @name isConf_ whether parallel measurement data has been set
    bool isConf_{false};
};  // class TimeCorrection

}  // namespace resource
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  // _ARA_TSYNC_TIME_CORRECTION_H_
