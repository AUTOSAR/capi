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
/// @file       phcclockadj.cpp
/// @brief      OSI hardware clock management
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "isoft/osi/network/phcclockadj.h"

#include <fcntl.h>
#include <linux/ptp_clock.h>
#include <sys/ioctl.h>

#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/log/logger.h"
#include "isoft/util/mix.h"
#include "isoft/util/time.h"

namespace isoft {
namespace osi {
namespace network {

/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log(); }

/// @brief destructor
PhcClockAdj::~PhcClockAdj() noexcept { phcCtrl_ = nullptr; }

/// @brief  initialize servo basic data
/// @param nowAdj - current offset
/// @param maxAdj - maximum offset
/// @param servo - servo reference
void PhcClockAdj::InitBasicServo(double const nowAdj, std::int32_t const maxAdj, PIServo &servo) noexcept
{
    double const servoFirstStepThreshold{0.00002};
    std::int32_t const servoMaxFrequency{900000000};
    servo.drift          = nowAdj;
    servo.lastFreq       = nowAdj;
    servo.kp             = 0.0;
    servo.ki             = 0.0;
    servo.configuredPiKp = kHWTS_KP_SCALE;
    servo.configuredPiKi = kHWTS_KI_SCALE;
    // servo.configuredPiKpScale = 0;.
    // servo.configuredPiKpExponent = 0-kHWTS_KI_SCALE;.
    // servo.configuredPiKpNormMax = kHWTS_KP_SCALE;.
    // servo.configuredPiKiScale = 0;.
    // servo.configuredPiKiExponent = kHWTS_KP_KI_EXPONENT;.
    // servo.configuredPiKiNormMax = kHWTS_KP_KI_NORMAL_MAX;.

    // if (servo.configuredPiKp && servo.configuredPiKi).
    {
        /* Use the constants as configured by the user without
		   adjusting for sync interval unless they make the servo
		   unstable. */
        servo.configuredPiKpScale    = servo.configuredPiKp;
        servo.configuredPiKiScale    = servo.configuredPiKi;
        servo.configuredPiKpExponent = 0.0;
        servo.configuredPiKiExponent = 0.0;
        servo.configuredPiKpNormMax  = kMAX_KP_NORM_MAX;
        servo.configuredPiKiNormMax  = kMAX_KI_NORM_MAX;
    }
    // else if (!servo.configuredPiKpScale || !servo.configuredPiKiScale) {.
    // 	if (sw_ts) {.
    // 		servo.configuredPiKpScale = kSWTS_KP_SCALE;.
    // 		servo.configuredPiKiScale = kSWTS_KI_SCALE;.
    // 	} else {.
    // 		servo.configuredPiKpScale = kHWTS_KP_SCALE;.
    // 		servo.configuredPiKiScale = kHWTS_KI_SCALE;.
    // 	}.
    // }.
    if (servoFirstStepThreshold > 0.0) {
        servo.firstStepThreshold = servoFirstStepThreshold * static_cast< double >(kTS_NSPERSEC);
    } else {
        servo.firstStepThreshold = 0.0;
    }
    servo.maxFrequency = static_cast< double >(maxAdj);
    if (servo.maxFrequency > static_cast< double >(servoMaxFrequency)) {
        servo.maxFrequency = static_cast< double >(servoMaxFrequency);
    }

    servo.firstUpdate      = true;
    servo.offsetThreshold  = 0;
    servo.numOffsetValues  = kDefNumOffsetValues;
    servo.currOffsetValues = servo.numOffsetValues;
}

/// @brief  get clock frequency offset
/// @param servo - servo object reference
/// @param offset - clock offset
/// @param localTs - system clock
/// @param weight - weight
/// @param state - state machine state
/// @return frequency offset
double PhcClockAdj::GetServoSample(PIServo &servo,
                                   int64_t const offset,
                                   int64_t const localTs,
                                   double const weight,
                                   enum PhcServoState &state) noexcept
{
    double r{0.0};
    double ppb{servo.lastFreq};
    std::ignore = state;
    switch (servo.count) {
        case 0: {
            servo.offset[0] = offset;
            servo.local[0]  = localTs;
            state           = PhcServoState::kPhcServoUnlocked;
            servo.count     = 1;
            break;
        }
        case 1: {
            servo.offset[1] = offset;
            servo.local[1]  = localTs;

            /* Make sure the first sample is older than the second. */
            if (servo.local[0] >= servo.local[1]) {
                state       = PhcServoState::kPhcServoUnlocked;
                servo.count = 0;
                break;
            }

            /* Wait long enough before estimating the frequency offset. */
            std::int64_t diffOffset{servo.local[1] - servo.local[0]};
            double localdiff{static_cast< double >(diffOffset) / kNsRatio};
            localdiff += localdiff * kFREQ_EST_MARGIN;
            constexpr double kNum{0.016};
            double freqEstInterval{kNum / servo.ki};
            if (freqEstInterval > kMAX_FREQ_EST_INTERVAL) {
                freqEstInterval = kMAX_FREQ_EST_INTERVAL;
            }
            if (localdiff < freqEstInterval) {
                state = PhcServoState::kPhcServoUnlocked;
                break;
            }

            /* Adjust drift by the measured frequency offset. */
            diffOffset = (servo.offset[1] - servo.offset[0]);
            std::int64_t const diffOffset2{servo.local[1] - servo.local[0]};
            servo.drift
                += (kNsRatio - servo.drift) * (static_cast< double >(diffOffset) / static_cast< double >(diffOffset2));

            if (servo.drift < -servo.maxFrequency) {
                servo.drift = -servo.maxFrequency;
            } else if (servo.drift > servo.maxFrequency) {
                servo.drift = servo.maxFrequency;
            } else {
            }
            bool const bigOffset2{servo.stepThreshold < static_cast< double >(llabs(offset))};
            bool const stepThreshold0{isoft::util::IsEq0(servo.stepThreshold)};
            bool const firstStepThreshold0{isoft::util::IsEq0(servo.firstStepThreshold)};
            bool const bigOffset1{servo.firstStepThreshold < static_cast< double >(llabs(offset))};
            if ((servo.firstUpdate && !firstStepThreshold0 && (bigOffset1)) || (!stepThreshold0 && (bigOffset2))) {
                state = PhcServoState::kPhcServoJump;
            } else {
                state = PhcServoState::kPhcServoLocked;
            }
            ppb         = servo.drift;
            servo.count = ara::tsync::internal::kTS_NUM_INT_2;
            break;
        }
        case ara::tsync::internal::kTS_NUM_INT_2: {
            /*
			* reset the clock servo when offset is greater than the max
			* offset value. Note that the clock jump will be performed in
			* step 1, so it is not necessary to have clock jump
			* immediately. This allows re-calculating drift as in initial
			* clock startup.
			*/
            bool const bigOffset{servo.stepThreshold < static_cast< double >(llabs(offset))};
            if (!isoft::util::IsEq0(servo.stepThreshold) && (bigOffset)) {
                state       = PhcServoState::kPhcServoUnlocked;
                servo.count = 0;
                break;
            }

            double const kIterm{servo.ki * static_cast< double >(offset) * weight};
            ppb = servo.kp * static_cast< double >(offset) * weight + servo.drift + kIterm;
            if (ppb < -servo.maxFrequency) {
                ppb = -servo.maxFrequency;
            } else if (ppb > servo.maxFrequency) {
                ppb = servo.maxFrequency;
            } else {
                servo.drift += kIterm;
            }
            state = PhcServoState::kPhcServoLocked;
            break;
        }
        default: {
            break;
        }
    }

    servo.lastFreq = ppb;
    r              = ppb;

    switch (state) {
        case PhcServoState::kPhcServoUnlocked: {
            servo.currOffsetValues = servo.numOffsetValues;
            break;
        }
        case PhcServoState::kPhcServoJump: {
            servo.currOffsetValues = servo.numOffsetValues;
            servo.firstUpdate      = false;
            break;
        }
        case PhcServoState::kPhcServoLocked: {
            servo.firstUpdate = false;
            break;
        }
        case PhcServoState::kPhcServoLockedStable: {
            /*
			* This case will never occur since the only place
			* PhcServoState::kPhcServoLockedStable is set is in this switch/case block .
			* (case PhcServoState::kPhcServoLocked)..
			*/
            break;
        }
    }

    return r;
}

/// @brief set clock synchronization interval
/// @param servo - clock servo object
/// @param interval - synchronization interval
void PhcClockAdj::SetSyncinterval(PIServo &servo, double const interval) noexcept
{
    servo.kp = servo.configuredPiKpScale * pow(interval, servo.configuredPiKpExponent);
    if (servo.kp > servo.configuredPiKpNormMax / interval) {
        servo.kp = servo.configuredPiKpNormMax / interval;
    }

    servo.ki = servo.configuredPiKiScale * pow(interval, servo.configuredPiKiExponent);
    if (servo.ki > servo.configuredPiKiNormMax / interval) {
        servo.ki = servo.configuredPiKiNormMax / interval;
    }
}

/// @brief clear clock servo count
/// @param servo - clock servo object
void PhcClockAdj::ClearServo(PIServo &servo) noexcept { servo.count = 0; }

/// @brief  get clock frequency offset
/// @param clockId clock id
/// @return frequency offset
double PhcClockAdj::GetfrequencyOffset(clockid_t const clockId) noexcept
{
    double f{0.0F};
    timex tx{};
    std::ignore = memset(&tx, 0, sizeof(tx));
    if (clock_adjtime(clockId, &tx) < 0) {
        LOG().Error() << "GetfrequencyOffset clock_adjtime error:" << ara::tsync::internal::GetErrString();
        return -1.0;
    }
    f = static_cast< double >(tx.freq) / kFreqScaled;
    return f;
}

/// @brief  adjust clock offset
/// @param clockId - clock ID
/// @param step - clock offset
/// @return 0 adjustment successful
std::int32_t PhcClockAdj::AdjustTimeOffset(clockid_t const clockId, int64_t step) noexcept
{
    timex tx{};
    std::int32_t sign{1};
    if (step < 0) {
        sign = -1;
        step *= -1;
    }
    std::ignore     = memset(&tx, 0, sizeof(tx));
    tx.modes        = static_cast< std::uint32_t >(ADJ_SETOFFSET | ADJ_NANO);
    tx.time.tv_sec  = sign * (step / kTS_NSPERSEC);
    tx.time.tv_usec = sign * (step % kTS_NSPERSEC);
    /*
	 * The value of a timeval is the sum of its fields, but the
	 * field tv_usec must always be non-negative.
	 */
    if (tx.time.tv_usec < 0) {
        tx.time.tv_sec -= 1;
        tx.time.tv_usec += kTS_NSPERSEC;
    }
    std::int32_t const ret{clock_adjtime(clockId, &tx)};
    std::ignore = tx;
    if (ret < 0) {
        LOG().Error() << "AdjustTimeOffset clock_adjtime error:" << ara::tsync::internal::GetErrString();
        return -1;
    }
    return 0;
}

/// @brief  adjust frequency offset
/// @param clockId - clock ID
/// @param freq - frequency offset
/// @return 0 adjustment successful
std::int32_t PhcClockAdj::AdjustFrequencyOffset(clockid_t const clockId, double const freq) noexcept
{
    timex tx{};
    std::ignore = memset(&tx, 0, sizeof(tx));
    tx.modes |= static_cast< std::uint32_t >(ADJ_FREQUENCY);
    tx.freq = static_cast< int64_t >((freq * kFreqScaled));
    std::int32_t const ret{clock_adjtime(clockId, &tx)};
    std::ignore = tx;
    if (ret < 0) {
        LOG().Error() << "AdjustFrequencyOffset clock_adjtime error:" << ara::tsync::internal::GetErrString();
        return -1;
    }
    return 0;
}

/// @brief  initialize clock inspection object
/// @param inspectObj - hardware clock inspection object
/// @param freqLimit - maximum frequency adjustment in ppb
void PhcClockAdj::InitPhcClockInspect(PhcClockInspect &inspectObj, std::int32_t const freqLimit) noexcept
{
    inspectObj.freqLimit      = freqLimit;
    inspectObj.freqHasSet     = false;
    inspectObj.maxFreq        = -kCHECK_MAX_FREQ;
    inspectObj.minFreq        = kCHECK_MAX_FREQ;
    inspectObj.lastSystemTime = 0;
}

/// @brief  check hardware timestamp
/// @param inspectObj - hardware clock inspection object
/// @param ts - timestamp
/// @return 0 passed check
std::int32_t PhcClockAdj::CheckPhcClockInspect(PhcClockInspect &inspectObj, int64_t const ts) noexcept
{
    int64_t monoTs{0};
    int64_t interval{0};
    int64_t monoInterval{0};
    timespec now{};
    std::int32_t ret{0};

    /* Check the sanity of the synchronized clock by comparing its
	   uncorrected frequency with the system monotonic clock. If
	   the synchronized clock is the system clock, the measured
	   frequency offset will be the current frequency correction of
	   the system clock. */

    if (false == inspectObj.freqHasSet) {
        return ret;
    }

    interval = ts - inspectObj.lastSystemTime;
    if ((interval >= 0) && (interval < kCHECK_MIN_INTERVAL)) {
        return ret;
    }

    std::ignore  = clock_gettime(CLOCK_MONOTONIC, &now);
    monoTs       = isoft::util::TimeSpec2Int64(&now);
    monoInterval = monoTs - inspectObj.lastMonoTime;

    if (monoInterval < kCHECK_MIN_INTERVAL) {
        return ret;
    }

    if ((0 != inspectObj.lastSystemTime) && (inspectObj.maxFreq <= kCHECK_MAX_FREQ)) {
        double const dInterval{static_cast< double >(interval)};
        double const dMinFreq{static_cast< double >(inspectObj.minFreq)};
        double const dMaxFreq{static_cast< double >(inspectObj.maxFreq)};
        double const dMonoInterval{static_cast< double >(monoInterval)};
        double const maxFoffset{kNsRatio * (dInterval / (1.0 + dMinFreq / kNsRatio) / dMonoInterval - 1.0)};
        double const minFoffset{kNsRatio * (dInterval / (1.0 + dMaxFreq / kNsRatio) / dMonoInterval - 1.0)};

        if (minFoffset > static_cast< double >(inspectObj.freqLimit)) {
            LOG().Error() << "CheckPhcClockInspect minFoffset=" << minFoffset
                          << " bigger than inspectObj.freqLimit=" << inspectObj.freqLimit;
            ret = 1;
        } else if (maxFoffset < static_cast< double >(0 - inspectObj.freqLimit)) {
            LOG().Error() << "CheckPhcClockInspect maxFoffset=" << maxFoffset
                          << " is smaller than -inspectObj.freqLimit=" << -inspectObj.freqLimit;
            ret = 1;
        } else {
        }
    }

    inspectObj.lastMonoTime   = monoTs;
    inspectObj.lastSystemTime = ts;
    inspectObj.minFreq        = inspectObj.currentFreq;
    inspectObj.maxFreq        = inspectObj.minFreq;

    return ret;
}

/// @brief set frequency of hardware clock inspection object
/// @param inspectObj - hardware clock inspection object
/// @param freq - current frequency correction
/// @return 0 no change
void PhcClockAdj::SetFrequency(PhcClockInspect &inspectObj, std::int32_t const freq) noexcept
{
    if (inspectObj.maxFreq < freq) {
        inspectObj.maxFreq = freq;
    }

    if (inspectObj.minFreq > freq) {
        inspectObj.minFreq = freq;
    }

    inspectObj.currentFreq = freq;
    inspectObj.freqHasSet  = true;
}

/// @brief check whether frequency correction has changed abnormally
/// @param inspectObj - hardware clock inspection object
/// @param freq - current frequency correction
/// @return 0 no change
std::int32_t PhcClockAdj::CheckFrequencyChange(PhcClockInspect const &inspectObj, std::int32_t const freq) noexcept
{
    if ((abs(inspectObj.currentFreq - freq) > 1) && inspectObj.freqHasSet) {
        LOG().Error() << "CheckFrequencyChange frequency error";
        return 1;
    }
    return 0;
}

/// @brief adjust step size
/// @param inspectObj - hardware clock inspection object
/// @param step - step size
void PhcClockAdj::StepPhcClockInspect(PhcClockInspect &inspectObj, int64_t const step) noexcept
{
    if (0 != inspectObj.lastSystemTime) {
        inspectObj.lastSystemTime += step;
    }
}

/// @brief initialize clock servo
/// @param maxAdj - maximum frequency adjustment
/// @param servo - clock servo object
void PhcClockAdj::_initServo(std::int32_t const maxAdj, PIServo &servo) noexcept
{
    double ppb{0.0};

    ppb = GetfrequencyOffset(phcCtrl_->clockId);
    InitBasicServo(-ppb, maxAdj, servo);

    SetSyncinterval(servo, kPHC_PI_INTERNVAL);
}

/// @brief get clock control pointer
/// @param clockId - clock ID
/// @param maxAdj - maximum frequency adjustment
/// @return clock control pointer
std::int32_t PhcClockAdj::InitClockAdj(clockid_t const clockId, std::int32_t const maxAdj) noexcept
{
    if (clockId == kCLOCK_INVALID) {
        return -1;
    }
    clkid_   = clockId;
    phcCtrl_ = std::make_shared< PhcClockCtrl >();
    if (nullptr == phcCtrl_) {
        LOG().Error() << "InitClockAdj failed to allocate memory";
        return -1;
    }
    phcCtrl_->clockId       = clockId;
    phcCtrl_->phcServoState = PhcServoState::kPhcServoUnlocked;

    InitPhcClockInspect(phcCtrl_->phcCheck, kCLOCKCHECK_FREQUENCY_LIMIT);
    if (true == phcCtrl_->phcCheck.freqHasSet) {
        // just for qac
    }
    _initServo(maxAdj, phcCtrl_->piServo);

    std::int32_t const ret{GetSysOffMethod(CLOCKID_TO_FD(clockId), kPHC_SAMPLE_COUNT, phcCtrl_->sysoffMethod)};
    if (0 != ret) {
        LOG().Error() << "InitClockAdj GetSysOffMethod failed";
        phcCtrl_ = nullptr;
        return -1;
    }
    return 0;
}

/// @brief update clock
/// @param offset - deviation value
/// @param ts - system clock
/// @return <0 indicates failure.
void PhcClockAdj::_updateClock(int64_t const offset, int64_t const ts) noexcept
{
    enum PhcServoState state
    {
        PhcServoState::kPhcServoUnlocked
    };
    double ppb{0.0};

    if (0 != CheckPhcClockInspect(phcCtrl_->phcCheck, ts)) {
        ClearServo(phcCtrl_->piServo);
    }
    LOG().Verbose() << "PhcClockAdj::_updateClock offset=" << offset << ", ts=" << ts;
    ppb                     = GetServoSample(phcCtrl_->piServo, offset, ts, kPHC_PI_INTERNVAL, state);
    phcCtrl_->phcServoState = state;

    switch (state) {
        case PhcServoState::kPhcServoUnlocked: {
            break;
        }
        case PhcServoState::kPhcServoJump: {
            if (0 != AdjustTimeOffset(phcCtrl_->clockId, -offset)) {
                ClearServo(phcCtrl_->piServo);
                phcCtrl_->phcServoState = PhcServoState::kPhcServoUnlocked;
                return;
            }
            StepPhcClockInspect(phcCtrl_->phcCheck, -offset);
            std::ignore = CheckFrequencyChange(phcCtrl_->phcCheck,
                                               static_cast< std::int32_t >(GetfrequencyOffset(phcCtrl_->clockId)));
            if (0 != AdjustFrequencyOffset(phcCtrl_->clockId, -ppb)) {
                ClearServo(phcCtrl_->piServo);
                phcCtrl_->phcServoState = PhcServoState::kPhcServoUnlocked;
                return;
            }
            SetFrequency(phcCtrl_->phcCheck, 0 - static_cast< std::int32_t >(ppb));
            break;
        }
        /* Fall through. */
        case PhcServoState::kPhcServoLocked:
        case PhcServoState::kPhcServoLockedStable: {
            std::ignore = CheckFrequencyChange(phcCtrl_->phcCheck,
                                               static_cast< std::int32_t >(GetfrequencyOffset(phcCtrl_->clockId)));
            if (0 != AdjustFrequencyOffset(phcCtrl_->clockId, -ppb)) {
                ClearServo(phcCtrl_->piServo);
                phcCtrl_->phcServoState = PhcServoState::kPhcServoUnlocked;
                return;
            }
            SetFrequency(phcCtrl_->phcCheck, 0 - static_cast< std::int32_t >(ppb));
            break;
        }
    }
    return;
}

/// @brief measure the deviation between phc clock and system clock
/// @param offset - deviation value
/// @param delay - delay
/// @param ts - phc clock
/// @return <0 indicates failure.
std::int32_t PhcClockAdj::GetClockSysOffset(int64_t &offset, int64_t &delay, int64_t &ts) noexcept
{
    std::ignore = offset;
    std::ignore = delay;

    int64_t offsetTmp{0};
    int64_t delayTmp{0};
    int64_t tsTmp{0};
    std::int32_t err{0};
    err = CalculatSysOff(CLOCKID_TO_FD(clkid_), phcCtrl_->sysoffMethod, kPHC_SAMPLE_COUNT, offsetTmp, tsTmp, delayTmp);
    if (0 == err) {
        count_++;
        offsetTmp = -offsetTmp;
        offsetTmp += kUTC_TAI_OFFSET_NS * (-1);
        offset = offsetTmp;
        delay  = delayTmp;
        ts     = tsTmp;
    }
    LOG().Verbose() << "PhcClockAdj::GetClockSysOffset offset=" << offset << ", delay=" << delay << ", err=" << err;
    return err;
}
/// @brief adjust the deviation between phc clock and system clock
/// @param offset - deviation value
/// @param ts - delay
/// @return <0 indicates failure.
std::int32_t PhcClockAdj::AdjustClock(int64_t offset, int64_t ts) noexcept
{
    ts += offset;
    _updateClock(offset, ts);
    return 0;
}  // namespace network

/// @brief get time, in nanoseconds
/// @param t - ptp clock pointer
/// @return time, in nanoseconds
int64_t PhcClockAdj::GetNs(struct ptp_clock_time const *const t) noexcept
{
    if (nullptr == t) {
        return 0;
    }
    return t->sec * kTS_NSPERSEC + static_cast< std::int64_t >(t->nsec);
}

/// @brief calculate the offset between PHC device and system time, using PTP_SYS_OFFSET_PRECISE
/// @param fd - PHC device handle
/// @param result - offset value (nanoseconds)
/// @param ts - system time
/// @return 0 success.
std::int32_t PhcClockAdj::DoSysoffPrecise(std::int32_t const fd, int64_t &result, int64_t &ts) noexcept
{
    std::ignore = result;
    std::ignore = ts;

    ptp_sys_offset_precise pso{};
    std::ignore = memset(&pso, 0, sizeof(pso));
    if (0 != ioctl(fd, static_cast< std::size_t >(PTP_SYS_OFFSET_PRECISE), &pso)) {
        LOG().Warn() << "DoSysoffPrecise PTP_SYS_OFFSET_PRECISE " << ara::tsync::internal::GetErrString();
        return -ara::tsync::internal::GetErrNo();
    }
    result = GetNs(&pso.sys_realtime) - GetNs(&pso.device);
    ts     = GetNs(&pso.sys_realtime);
    return 0;
}

/// @brief calculate offset
/// @param pct - ptp clock pointer
/// @param extended - whether to use PTP_SYS_OFFSET_EXTENDED
/// @param sampleCount - number of samples
/// @param ts - system time
/// @param delay - delay in reading PHC clock
/// @return offset.
int64_t PhcClockAdj::EstimateSysOff(struct ptp_clock_time const *const pct,
                                    bool const extended,
                                    std::uint32_t const sampleCount,
                                    int64_t &ts,
                                    int64_t &delay) noexcept
{
    int64_t t1{0};
    int64_t t2{0};
    int64_t tp{0};
    int64_t interval{0};
    int64_t timestamp{0};
    int64_t offset{0};
    int64_t shortestInterval{0};
    int64_t bestTimestamp{0};
    int64_t bestOffset{0};
    std::uint32_t i{0U};
    std::ignore = ts;
    std::ignore = delay;

    if (extended) {
        t1 = GetNs(&pct[kARRAY_INDEX_3 * i]);
        tp = GetNs(&pct[kARRAY_INDEX_3 * i + 1U]);
        t2 = GetNs(&pct[kARRAY_INDEX_3 * i + kARRAY_INDEX_2]);
    } else {
        t1 = GetNs(&pct[kARRAY_INDEX_2 * i]);
        tp = GetNs(&pct[kARRAY_INDEX_2 * i + 1U]);
        t2 = GetNs(&pct[kARRAY_INDEX_2 * i + kARRAY_INDEX_2]);
    }
    shortestInterval = t2 - t1;
    bestTimestamp    = (t2 + t1) / ara::tsync::internal::kTS_NUM_INT_2;
    bestOffset       = bestTimestamp - tp;

    for (i = 1U; i < sampleCount; i++) {
        if (extended) {
            t1 = GetNs(&pct[kARRAY_INDEX_3 * i]);
            tp = GetNs(&pct[kARRAY_INDEX_3 * i + 1U]);
            t2 = GetNs(&pct[kARRAY_INDEX_3 * i + kARRAY_INDEX_2]);
        } else {
            t1 = GetNs(&pct[kARRAY_INDEX_2 * i]);
            tp = GetNs(&pct[kARRAY_INDEX_2 * i + 1U]);
            t2 = GetNs(&pct[kARRAY_INDEX_2 * i + kARRAY_INDEX_2]);
        }
        interval  = t2 - t1;
        timestamp = (t2 + t1) / ara::tsync::internal::kTS_NUM_INT_2;
        offset    = timestamp - tp;
        if (interval < shortestInterval) {
            shortestInterval = interval;
            bestTimestamp    = timestamp;
            bestOffset       = offset;
        }
    }
    ts    = bestTimestamp;
    delay = shortestInterval;
    return bestOffset;
}

/// @brief calculate the offset between PHC device and system time, using PTP_SYS_OFFSET_EXTENDED
/// @param fd - PHC device handle
/// @param sampleCount - number of samples
/// @param result - offset value (nanoseconds)
/// @param ts - system time
/// @param delay - delay in reading PHC clock
/// @return 0 success.
std::int32_t PhcClockAdj::DosysoffExtend(
    std::int32_t const fd, std::uint32_t const sampleCount, int64_t &result, int64_t &ts, int64_t &delay) noexcept
{
    std::ignore = result;
    std::ignore = ts;
    std::ignore = delay;
#ifdef PTP_SYS_OFFSET_EXTENDED
    ptp_sys_offset_extended pso{};
    std::ignore   = memset(&pso, 0, sizeof(pso));
    pso.n_samples = sampleCount;
    if (0 != ioctl(fd, static_cast< std::size_t >(PTP_SYS_OFFSET_EXTENDED), &pso)) {
        LOG().Error() << "DosysoffExtend ioctl PTP_SYS_OFFSET_EXTENDED" << ara::tsync::internal::GetErrString();
        return -1;
    }
    result = EstimateSysOff(&pso.ts[0][0], true, sampleCount, ts, delay);
    return 0;
#else
    return -1;
#endif
}

/// @brief calculate the offset between PHC device and system time, using PTP_SYS_OFFSET
/// @param fd - PHC device handle
/// @param sampleCount - number of samples
/// @param result - offset value (nanoseconds)
/// @param ts - system time
/// @param delay - delay in reading PHC clock
/// @return 0 success.
std::int32_t PhcClockAdj::DoSysoffBasic(
    std::int32_t const fd, std::uint32_t const sampleCount, int64_t &result, int64_t &ts, int64_t &delay) noexcept
{
    std::ignore = result;
    std::ignore = ts;
    std::ignore = delay;
    ptp_sys_offset pso{};
    std::ignore   = memset(&pso, 0, sizeof(pso));
    pso.n_samples = sampleCount;
    if (0 != ioctl(fd, static_cast< std::size_t >(PTP_SYS_OFFSET), &pso)) {
        LOG().Error() << "DoSysoffBasic ioctl PTP_SYS_OFFSET" << ara::tsync::internal::GetErrString();
        return -1;
    }
    result = EstimateSysOff(static_cast< struct ptp_clock_time const * >(&pso.ts[0]), false, sampleCount, ts, delay);
    return 0;
}

/// @brief calculate the offset between PHC device and system time
/// @param fd - PHC device handle
/// @param method - method enumeration
/// @param sampleCount - number of samples
/// @param result - offset value (nanoseconds)
/// @param ts - system time
/// @param delay - delay in reading PHC clock
/// @return 0 success.
std::int32_t PhcClockAdj::CalculatSysOff(std::int32_t const fd,
                                         SysOffMethod const method,
                                         std::uint32_t const sampleCount,
                                         int64_t &result,
                                         int64_t &ts,
                                         int64_t &delay) noexcept
{
    std::int32_t ret{0};
    std::ignore = result;
    std::ignore = ts;
    std::ignore = delay;
    switch (method) {
        case SysOffMethod::kSysOffPrecise: {
            delay = 0;
            ret   = DoSysoffPrecise(fd, result, ts);
            break;
        }
        case SysOffMethod::kSysOffExtended: {
            ret = DosysoffExtend(fd, sampleCount, result, ts, delay);
            break;
        }
        case SysOffMethod::kSysOffBasic: {
            ret = DoSysoffBasic(fd, sampleCount, result, ts, delay);
            break;
        }
        default: {
            ret = -1;
            break;
        }
    }
    return ret;
}

/// @brief probe the method supported by the device for calculating system deviation
/// @param fd - PHC device handle
/// @param sampleCount - number of samples
/// @param method - calculation method
/// @return method enumeration.
std::int32_t PhcClockAdj::GetSysOffMethod(std::int32_t const fd,
                                          std::uint32_t const sampleCount,
                                          SysOffMethod &method) noexcept
{
    int64_t junk{0};
    int64_t delay{0};
    std::int32_t j{0};
    std::int32_t err{-1};
    int64_t ts{0};
    std::ignore = method;  // just for qac
    if (sampleCount > static_cast< std::uint32_t >(PTP_MAX_SAMPLES)) {
        LOG().Error() << "GetSysOffMethod sampleCount=" << sampleCount
                      << " is bigger than PTP_MAX_SAMPLES=" << PTP_MAX_SAMPLES;
        return -1;
    }
    std::array< SysOffMethod, ara::tsync::internal::kTS_NUM_INT_3 > allCalcMethod{
        SysOffMethod::kSysOffPrecise, SysOffMethod::kSysOffExtended, SysOffMethod::kSysOffBasic};
    for (SysOffMethod const calcMethod : allCalcMethod) {
        for (j = 0; j < ara::tsync::internal::kTS_NUM_INT_3; j++) {
            err         = CalculatSysOff(fd, calcMethod, sampleCount, junk, ts, delay);
            std::ignore = junk;
            std::ignore = ts;
            std::ignore = delay;
            if (ara::tsync::internal::GetErrNo() == EBUSY) {
                continue;
            }
            if (0 == err) {
                method = calcMethod;
            }
            break;
        }
    }
    return err;
}

}  // namespace network
}  // namespace osi
}  // namespace isoft
