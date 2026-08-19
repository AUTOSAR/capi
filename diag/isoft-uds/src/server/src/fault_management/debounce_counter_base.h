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
/// @file       debounce_counter_base.h
/// @brief      This file provides debounce_counter_base functionality
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
/// @trace_id_sr=d56d1d16-9e0c-4088-a09b-97c0d4914786
/// @unit_name=DebounceCounterBase
/// @unit_description=This file provides debounce_counter_base functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_DEBOUNCE_COUNTER_BASE_H__
#define __ISOFT_UDS_FAULT_DEBOUNCE_COUNTER_BASE_H__

#include "fault_management/debounce_interface.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Counting debouncing algorithm class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02535
/// @needwork = dd
/// @endcode
class DebounceCounterBase final : public DebounceInterface
{
public:
    /// @brief Constructor
    /// @param[in] event The Event associated with this Debouncing
    /// @param[in] node The pre-parsed configuration data of this Debouncing
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01815
    /// @needwork = dd
    /// @endcode
    explicit DebounceCounterBase(std::shared_ptr< Event > const& event, DebounceAlgorithmConfig& config) noexcept;

    /// @brief Event receives MonitorAction from Monitor, which is called here to trigger Debouncing related state processing
    /// @param[in] action The MonitorAction parameter passed by Event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01816
    /// @needwork = dd
    /// @endcode
    void IndicateReportedStatus(MonitorAction action) final;

    /// @brief Specific implementation of Reset behavior, FDC will be reset to 0
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void Reset() final;

    /// @brief Specific implementation of Freeze behavior
    /// @param[in] b Whether to Freeze
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01817
    /// @needwork = dd
    /// @endcode
    void Freeze(bool b) noexcept final;

    /// @brief Get Debounce type
    /// @return Debounce type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01818
    /// @needwork = dd
    /// @endcode
    DebounceTypeEnum GetType() const noexcept final;

    /// @brief Get Debounce state
    /// @return Debounce state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01819
    /// @needwork = dd
    /// @endcode
    DebouncingState GetDouncingState() noexcept final;

    /// @brief Check whether the FDC storage threshold is met, the trigger mode of MemoryEntry DiagnosticMemoryEntryStorageTriggerEnum::kFdcThreshold will cooperate with this function to determine whether storage is satisfied
    /// @return Returns true if satisfied, otherwise false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01820
    /// @needwork = dd
    /// @endcode
    bool IsFilledFdcThresholdStorage() noexcept final;

    /// @brief Set DebounceCounterParam
    /// @param const Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01821
    /// @needwork = dd
    /// @endcode
    void SetDebounceCounterParam(CounterBased const& counter) noexcept final;

protected:
    /// @brief Check whether the FDC storage threshold is met
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01822
    /// @needwork = dd
    /// @endcode
    void _CheckJumpDown();

    /// @brief Check whether the FDC storage threshold is met
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01823
    /// @needwork = dd
    /// @endcode
    void _CheckJumpUp();

    /// @brief Check whether the FDC condition is met
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01824
    /// @needwork = dd
    /// @endcode
    void _CheckConditions();

private:
    /// @name params_
    /// Configuration parameters of Debouncing
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01825
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< CounterBasedParam > params_{};
    /// @name testFailReached_
    /// Whether error has been detected
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01826
    /// @needwork = dd
    /// @endcode
    bool testFailReached_{false};
    /// @name testPassedReached_
    /// Whether it is Passed state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01827
    /// @needwork = dd
    /// @endcode
    bool testPassedReached_{false};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __DEBOUNCE_COUNTER_BASE_H__