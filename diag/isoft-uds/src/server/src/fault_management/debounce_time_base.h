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
/// @file       debounce_time_base.h
/// @brief      This file provides debounce_time_base functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/fault_management
/// @interface_level=software
/// @trace_id_sr=36f8c80a-3a99-4f86-8412-7b23a0e9bab5
/// @unit_name=DebounceTimeBase
/// @unit_description=This file provides debounce_time_base functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_DEBOUNCE_TIME_BASE_H__
#define __ISOFT_UDS_FAULT_DEBOUNCE_TIME_BASE_H__

#include "common/timer.h"
#include "fault_management/debounce_interface.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Debouncing time class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02498
/// @needwork = dd
/// @endcode
class DebounceTimeBase : public DebounceInterface
{
public:
    /// @brief Constructor
    /// @param[in] event The Event bound to this algorithm
    /// @param[in] config The configuration parameters related to this algorithm, pre-parsed
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    explicit DebounceTimeBase(std::shared_ptr< Event > const& event, DebounceAlgorithmConfig& config);

    /// @brief Report status
    /// @param[in] action The MonitorAction parameter passed by Event
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void IndicateReportedStatus(MonitorAction action) final;

    /// @brief Get FDC
    /// @return FDC
    /// @return FDC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01996
    /// @needwork = dd
    /// @endcode
    std::int8_t GetFaultDetectionCounter() noexcept final;

    /// @brief Reset FDC
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void Reset() final;

    /// @brief Freeze or unfreeze
    /// @param[in] b Whether to Freeze
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void Freeze(bool b) noexcept final;

    /// @brief Get the type of debounce interface
    /// @return Debouncing type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01647
    /// @needwork = dd
    /// @endcode
    DebounceTypeEnum GetType() const noexcept final;

    /// @brief Check whether the FDC storage threshold is met, the trigger mode of MemoryEntry DiagnosticMemoryEntryStorageTriggerEnum::kFdcThreshold will cooperate with this function to determine whether storage is satisfied
    /// @return Returns true if satisfied, otherwise false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01648
    /// @needwork = dd
    /// @endcode
    bool IsFilledFdcThresholdStorage() noexcept final;

    /// @brief Set CounterBased parameters of debounce interface
    /// @param[in] counter CounterBased parameter
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void SetDebounceTimerParam(TimeBased const& timer) noexcept final;

private:
    /// @name params_
    /// The configuration parameters of this algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01649
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< TimeBasedParam > params_{nullptr};

    /// @name timer_
    /// The built-in timer of this algorithm, timing within the algorithm requires it to trigger
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01650
    /// @needwork = dd
    /// @endcode
    Timer timer_{};

    /// @name actionLast_
    /// The recent MonitoAction recorded internally by this algorithm, used to determine the actions that occurred during the timing period when the timer expires
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01651
    /// @needwork = dd
    /// @endcode
    std::atomic< MonitorAction > actionLast_{MonitorAction::kResetDebouncing};

    /// @name timeStartCount_
    /// Start time of Prefailed or Prepassed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01652
    /// @needwork = dd
    /// @endcode
    std::atomic< std::uint64_t > timeStartCount_{0UL};

    /// @name timeConsumed_
    /// When set to Freeze state, record the time already consumed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01653
    /// @needwork = dd
    /// @endcode
    std::atomic< std::uint64_t > timeConsumed_{0UL};

    /// @name freezedFollowePrefailed_
    /// Whether the previous operation of freezing was Prefailed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01654
    /// @needwork = dd
    /// @endcode
    std::atomic< bool > freezedFollowePrefailed_{false};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft
#endif  // __DEBOUNCE_TIME_BASE_H__