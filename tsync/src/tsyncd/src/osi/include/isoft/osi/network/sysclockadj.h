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
/// @file       sysclockadj.h
/// @brief      OSI network module Ether class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_NETWORK_SYSCLOCKADJ_H
#define ISOFT_OSI_NETWORK_SYSCLOCKADJ_H
#include <linux/ptp_clock.h>

#include <cstdint>
#include <memory>

#include "ara/tsync/internal/common.h"
#include "phcclockadj.h"

namespace isoft {
namespace osi {
namespace network {

/// @brief system clock adjustment class
class SysClockAdj final
{
public:
    /// @brief constructor
    SysClockAdj() = default;

    /// @brief destructor
    ~SysClockAdj() noexcept;

    /// @brief copy constructor is prohibited
    /// @param other other object
    SysClockAdj(SysClockAdj const &other) = delete;

    /// @brief copy is prohibited
    /// @param other other object
    /// @return reference to class object
    SysClockAdj &operator=(SysClockAdj const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    SysClockAdj(SysClockAdj &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    SysClockAdj &operator=(SysClockAdj &&) &noexcept = default;

    /// @brief initialize system clock adjustment
    /// @param clockId clock ID
    /// @param maxAdj maximum frequency adjustment
    /// @return 0 initialization successful
    std::int32_t InitClockAdj(clockid_t const clockId, std::int32_t const maxAdj) noexcept;

    /// @brief adjust the deviation between system clock and system clock
    /// @param offset deviation value
    /// @param ts delay
    /// @return <0 indicates failure.
    std::int32_t AdjustClock(int64_t offset, int64_t ts) noexcept;

private:
    /// @brief  initialize servo basic data
    /// @param nowAdj current offset
    /// @param maxAdj maximum offset
    /// @param servo servo reference
    static void InitBasicServo(double const nowAdj, std::int32_t const maxAdj, PIServo &servo) noexcept;

    /// @brief  get clock frequency offset
    /// @param servo servo object reference
    /// @param offset clock offset
    /// @param localTs system clock
    /// @param weight weight
    /// @param state state machine state
    /// @return frequency offset
    static double GetServoSample(PIServo &servo,
                                 int64_t const offset,
                                 int64_t const localTs,
                                 double const weight,
                                 enum PhcServoState &state) noexcept;

    /// @brief set clock synchronization interval
    /// @param servo clock servo object
    /// @param interval synchronization interval
    static void SetSyncinterval(PIServo &servo, double const interval) noexcept;

    /// @brief clear clock servo count
    /// @param servo clock servo object
    static void ClearServo(PIServo &servo) noexcept;

    /// @brief  get clock frequency offset
    /// @param clockId clock ID
    /// @return frequency offset
    double _getfrequencyOffset(clockid_t const clockId) const noexcept;

    /// @brief  adjust clock offset
    /// @param clockId clock ID
    /// @param step clock offset
    /// @return 0 adjustment successful
    static std::int32_t AdjustTimeOffset(clockid_t const clockId, int64_t step) noexcept;

    /// @brief  adjust frequency offset
    /// @param clockId clock ID
    /// @param freq frequency offset
    /// @return 0 adjustment successful
    std::int32_t _adjustFrequencyOffset(clockid_t const clockId, double freq) const noexcept;

    /// @brief  initialize clock inspection object
    /// @param inspectObj clock object
    /// @param freqLimit maximum frequency adjustment in ppb.
    static void InitPhcClockInspect(PhcClockInspect &inspectObj, std::int32_t const freqLimit) noexcept;

    /// @brief  check hardware timestamp
    /// @param inspectObj  hardware clock inspection object
    /// @param ts timestamp
    /// @return 0 passed check
    static std::int32_t CheckPhcClockInspect(PhcClockInspect &inspectObj, int64_t const ts) noexcept;

    /// @brief set frequency of hardware clock inspection object
    /// @param inspectObj  hardware clock inspection object
    /// @param freq current frequency correction
    static void SetFrequency(PhcClockInspect &inspectObj, std::int32_t const freq) noexcept;

    /// @brief check whether frequency correction has changed abnormally
    /// @param inspectObj hardware clock inspection object
    /// @param freq current frequency correction
    /// @return 0 no change
    static std::int32_t CheckFrequencyChange(PhcClockInspect const &inspectObj, std::int32_t const freq) noexcept;

    /// @brief adjust step size
    /// @param inspectObj hardware clock inspection object
    /// @param step step size
    static void StepPhcClockInspect(PhcClockInspect &inspectObj, int64_t const step) noexcept;

    /// @brief initialize clock servo
    /// @param maxAdj maximum frequency adjustment
    /// @param servo clock servo object
    void _initServo(std::int32_t const maxAdj, PIServo &servo) noexcept;

    /// @brief update clock
    /// @param offset deviation value
    /// @param ts system clock
    /// @return <0 indicates failure.
    void _updateClock(int64_t const offset, int64_t const ts) noexcept;

    /// @brief get time, in nanoseconds
    /// @param t ptp clock pointer
    /// @return time, in nanoseconds
    static int64_t GetNs(struct ptp_clock_time const *const t) noexcept;

    /// @brief set status flag
    void _setSync() const noexcept;

private:
    /// @name clkid_ ptp clock id
    clockid_t clkid_{-1};

    /// @name phcCtrl_ system clock control
    std::shared_ptr< PhcClockCtrl > phcCtrl_{nullptr};

    /// @name lastMinOffset_ historical minimum offset, to eliminate burrs and jitter
    int64_t lastMinOffset_{0};

    /// @name averOffset_ average offset, to eliminate burrs and jitter
    std::int64_t averOffset_{0};

    /// @name averDelay_ average delay, to eliminate burrs and jitter
    std::int64_t averDelay_{0};

    /// @name count_ number of samples
    int64_t count_{0};

    /// @name leapBit_
    std::int32_t leapBit_{0};

    /// @name realtimeHz_
    std::int64_t realtimeHz_{0};

    /// @name realtimeNominalTick_
    std::int64_t realtimeNominalTick_{0};

};  /// class SysClockAdj

}  // namespace network
}  // namespace osi
}  // namespace isoft
#endif  // ISOFT_OSI_NETWORK_SYSCLOCKADJ_H
