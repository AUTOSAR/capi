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
/// @file       phcclockadj.h
/// @brief      OSI network module Ether class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_NETWORK_PHCCLOCKADJ_H
#define ISOFT_OSI_NETWORK_PHCCLOCKADJ_H
#include <linux/ptp_clock.h>

#include <cstdint>
#include <memory>

#include "ara/tsync/internal/common.h"
#include "phcdef.h"

namespace isoft {
namespace osi {
namespace network {

/// @brief constant declaration array index
static constexpr std::uint32_t kARRAY_INDEX_2{2U};
/// @brief constant declaration array index
static constexpr std::uint32_t kARRAY_INDEX_3{3U};

/// @brief constant declaration kNsRatio
static constexpr double const kMAX_FREQ_EST_INTERVAL{1000.0};

/// @brief constant declaration kNsRatio
static constexpr double const kNsRatio{1e9};

/// @brief PhcClockInspect hardware clock inspection class
struct PhcClockInspect
{
    /// @name freqLimit - maximum frequency
    std::int32_t freqLimit{0};
    /// @name freqHasSet - whether frequency has been set
    bool freqHasSet{false};
    /// @name currentFreq - current frequency
    std::int32_t currentFreq{0};
    /// @name maxFreq - maximum frequency
    std::int32_t maxFreq{0};
    /// @name minFreq - minimum frequency
    std::int32_t minFreq{0};
    /// @name lastSystemTime - last system time
    int64_t lastSystemTime{0};
    /// @name lastMonoTime - last system steady time
    int64_t lastMonoTime{0};
};

/// @brief PIServo proportional-integral servo
struct PIServo
{
    /// @name maxFrequency maximum frequency
    double maxFrequency{0.0};
    /// @name stepThreshold  step threshold
    double stepThreshold{0.0};
    /// @name firstStepThreshold initial step threshold
    double firstStepThreshold{0.0};
    /// @name firstUpdate whether first time
    bool firstUpdate{false};
    /// @name offsetThreshold - offset threshold
    int64_t offsetThreshold{0};
    /// @name numOffsetValues -
    std::int32_t numOffsetValues{0};
    /// @name currOffsetValues -
    std::int32_t currOffsetValues{0};

    /// @name offset -
    int64_t offset[ara::tsync::internal::kTS_NUM_2]{};
    /// @name local -
    int64_t local[ara::tsync::internal::kTS_NUM_2]{};
    /// @name drift -
    double drift{0.0};
    /// @name kp -
    double kp{0.0};
    /// @name ki -
    double ki{0.0};
    /// @name lastFreq -
    double lastFreq{0.0};
    /// @name count -
    std::int32_t count{0};
    /* configuration: */
    /// @name configuredPiKp -
    double configuredPiKp{0.0};
    /// @name configuredPiKi -
    double configuredPiKi{0.0};
    /// @name configuredPiKpScale -
    double configuredPiKpScale{0.0};
    /// @name configuredPiKpExponent -
    double configuredPiKpExponent{0.0};
    /// @name configuredPiKpNormMax -
    double configuredPiKpNormMax{0.0};
    /// @name configuredPiKiScale -
    double configuredPiKiScale{0.0};
    /// @name configuredPiKiExponent -
    double configuredPiKiExponent{0.0};
    /// @name configuredPiKiNormMax -
    double configuredPiKiNormMax{0.0};
};

/// @brief PhcServoState servo state
enum class PhcServoState : std::uint8_t
{
    kPhcServoUnlocked     = 0U,
    kPhcServoJump         = 1U,
    kPhcServoLocked       = 2U,
    kPhcServoLockedStable = 3U,
};

/// @brief SysOffMethod system offset calculation method
enum class SysOffMethod : std::uint8_t
{
    kSysOffPrecise  = 0,
    kSysOffExtended = 1,
    kSysOffBasic    = 2
};

/// @brief PhcClockCtrl phc clock control class
struct PhcClockCtrl
{
    /// @name clockId - clock ID
    clockid_t clockId{};
    /// @name sysoffMethod - system deviation detection method
    SysOffMethod sysoffMethod{SysOffMethod::kSysOffBasic};
    /// @name piServo - servo
    PIServo piServo{};
    /// @name phcServoState - state
    enum PhcServoState phcServoState
    {
        PhcServoState::kPhcServoUnlocked
    };

    /// @name phcCheck -
    PhcClockInspect phcCheck{};
};

/// @brief PHC clock adjustment class
class PhcClockAdj final
{
public:
    /// @brief constructor
    PhcClockAdj() = default;

    /// @brief destructor
    ~PhcClockAdj() noexcept;

    /// @brief copy constructor is prohibited
    /// @param other other object
    PhcClockAdj(PhcClockAdj const &other) = delete;

    /// @brief copy is prohibited
    /// @param other other object
    /// @return reference to class object
    PhcClockAdj &operator=(PhcClockAdj const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    PhcClockAdj(PhcClockAdj &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    PhcClockAdj &operator=(PhcClockAdj &&) &noexcept = default;

    /// @brief initialize PHC clock adjustment
    /// @param clockId clock ID
    /// @param maxAdj maximum frequency adjustment
    /// @return 0 initialization successful
    std::int32_t InitClockAdj(clockid_t const clockId, std::int32_t const maxAdj) noexcept;

    /// @brief measure the deviation between phc clock and system clock
    /// @param offset - deviation value
    /// @param delay - delay
    /// @param ts - phc clock
    /// @return <0 indicates failure.
    std::int32_t GetClockSysOffset(int64_t &offset, int64_t &delay, int64_t &ts) noexcept;

    /// @brief adjust the deviation between phc clock and system clock
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
    static double GetfrequencyOffset(clockid_t const clockId) noexcept;

    /// @brief  adjust clock offset
    /// @param clockId clock ID
    /// @param step clock offset
    /// @return 0 adjustment successful
    static std::int32_t AdjustTimeOffset(clockid_t const clockId, int64_t step) noexcept;

    /// @brief  adjust frequency offset
    /// @param clockId clock ID
    /// @param freq frequency offset
    /// @return 0 adjustment successful
    static std::int32_t AdjustFrequencyOffset(clockid_t const clockId, double const freq) noexcept;

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

    /// @brief calculate the offset between PHC device and system time, using PTP_SYS_OFFSET_PRECISE
    /// @param fd PHC device handle
    /// @param result offset value (nanoseconds)
    /// @param ts system time
    /// @return 0 success.
    static std::int32_t DoSysoffPrecise(std::int32_t const fd, int64_t &result, int64_t &ts) noexcept;

    /// @brief calculate offset
    /// @param pct ptp clock pointer
    /// @param extended whether to use PTP_SYS_OFFSET_EXTENDED
    /// @param sampleCount number of samples
    /// @param ts system time
    /// @param delay delay in reading PHC clock
    /// @return offset.
    static int64_t EstimateSysOff(struct ptp_clock_time const *const pct,
                                  bool const extended,
                                  std::uint32_t const sampleCount,
                                  int64_t &ts,
                                  int64_t &delay) noexcept;

    /// @brief calculate the offset between PHC device and system time, using PTP_SYS_OFFSET_EXTENDED
    /// @param fd PHC device handle
    /// @param sampleCount number of samples
    /// @param result offset value (nanoseconds)
    /// @param ts system time
    /// @param delay delay in reading PHC clock
    /// @return 0 success.
    static std::int32_t DosysoffExtend(
        std::int32_t const fd, std::uint32_t const sampleCount, int64_t &result, int64_t &ts, int64_t &delay) noexcept;

    /// @brief calculate the offset between PHC device and system time, using PTP_SYS_OFFSET
    /// @param fd  PHC device handle
    /// @param sampleCount number of samples
    /// @param result offset value (nanoseconds)
    /// @param ts system time
    /// @param delay delay in reading PHC clock
    /// @return 0 success.
    static std::int32_t DoSysoffBasic(
        std::int32_t const fd, std::uint32_t const sampleCount, int64_t &result, int64_t &ts, int64_t &delay) noexcept;

    /// @brief calculate the offset between PHC device and system time
    /// @param fd PHC device handle
    /// @param method method enumeration
    /// @param sampleCount - number of samples
    /// @param result - offset value (nanoseconds)
    /// @param ts - system time
    /// @param delay - delay in reading PHC clock
    /// @return 0 success.
    static std::int32_t CalculatSysOff(std::int32_t const fd,
                                       SysOffMethod const method,
                                       std::uint32_t const sampleCount,
                                       int64_t &result,
                                       int64_t &ts,
                                       int64_t &delay) noexcept;

    /// @brief probe the method supported by the device for calculating system deviation
    /// @param fd - PHC device handle
    /// @param sampleCount - number of samples
    /// @param method - calculation method
    /// @return method enumeration.
    static std::int32_t GetSysOffMethod(std::int32_t const fd,
                                        std::uint32_t const sampleCount,
                                        SysOffMethod &method) noexcept;

private:
    /// @name clkid_ ptp clock id
    clockid_t clkid_{-1};

    /// @name phcCtrl_ phc clock control
    std::shared_ptr< PhcClockCtrl > phcCtrl_{nullptr};

    /// @name lastMinOffset_ historical minimum offset, to eliminate burrs and jitter
    int64_t lastMinOffset_{0};

    /// @name averOffset_ average offset, to eliminate burrs and jitter
    std::int64_t averOffset_{0};

    /// @name averDelay_ average delay, to eliminate burrs and jitter
    std::int64_t averDelay_{0};

    /// @name count_ number of samples
    int64_t count_{0};

};  /// class PhcClockAdj

}  // namespace network
}  // namespace osi
}  // namespace isoft
#endif  // ISOFT_OSI_NETWORK_PHCCLOCKADJ_H
