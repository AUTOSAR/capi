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
/// @file       indicator.h
/// @brief      This file provides indicator functionality
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
/// @trace_id_sr=ae8a8bdf-a685-4ba2-82d7-654095dd7263
/// @unit_name=indicator
/// @unit_description=This file provides indicator functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_INDICATOR_H__
#define __ISOFT_UDS_FAULT_INDICATOR_H__

#include <cstdint>
#include <memory>
#include <vector>

#include "fault_management/public.h"
#include "isoft/uds/fault_management/fault_management.h"
#include "isoft/uds/fault_management/indicator_interface.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Event forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02524
/// @needwork = dd
/// @endcode
class Event;

/// @brief OperationCycle forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02525
/// @needwork = dd
/// @endcode
class OperationCycle;

/// @brief indicator class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02526
/// @needwork = dd
/// @endcode
class Indicator final : public std::enable_shared_from_this< Indicator >
{
public:
    /// @brief Indicator default constructor
    /// @param[in] config Configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01732
    /// @needwork = dd
    /// @endcode
    explicit Indicator(IndicatorConfig& config) : config_{config} {}

    /// @brief Indicator copy constructor
    /// @param[in] other Indicator reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01733
    /// @needwork = dd
    /// @endcode
    Indicator(Indicator const& other) = default;

    /// @brief Indicator copy constructor
    /// @param[in] other Indicator rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01734
    /// @needwork = dd
    /// @endcode
    Indicator(Indicator&& other) = default;

    /// @brief DemIndicator assignment operator
    /// @param[in] right Indicator reference
    /// @return Indicator reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01735
    /// @needwork = dd
    /// @endcode
    Indicator& operator=(Indicator const& right) = default;

    /// @brief DemIndicator assignment operator
    /// @param[in] right Indicator rvalue reference
    /// @return Indicator reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01736
    /// @needwork = dd
    /// @endcode
    Indicator& operator=(Indicator&& right) = default;

    /// @brief DemIndicator destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01737
    /// @needwork = dd
    /// @endcode
    ~Indicator() = default;

    /// @brief Init function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool Init();

    /// @brief The modeling of indicator is somewhat special, some parameters are placed in event, so this function is introduced to pass in the parameters parsed by event
    /// @param[in] behavior The indicator parameter configured in event
    /// @param[in] healingCycle The indicator parameter configured in event
    /// @param[in] threshold The indicator parameter configured in event
    /// @param[in] event The bound event
    /// @return Returns true on success, false on failure
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool InitValues(DiagnosticConnectedIndicatorBehaviorEnum const behavior,
                    std::shared_ptr< OperationCycle > const& healingCycle,
                    uint32_t const threshold,
                    std::shared_ptr< Event > const& event);

    /// @brief Check whether the internal count has reached the configured threshold
    /// @param[in] failed When true, indicates a fault, internal count needs to be cleared and restarted
    /// @return Returns true when threshold is reached, otherwise false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01738
    /// @needwork = dd
    /// @endcode
    bool CheckThreshold(bool const failed) noexcept;

    /// @brief Modify the internally recorded WIR state
    /// @param[in] val Whether currently in WIR state
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void SetStatusWIR(bool const val);

    /// @brief When the state of the bound healing cycle changes, it will be called here,
    /// @param[in] oldState Old state
    /// @param[in] newState New state
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void ReportOperationCycleStateTransition(OperationCycleType const oldState, OperationCycleType const newState);

private:
    /// @brief Calculate the type of indicator, the indicator port in modeling needs to obtain this type
    /// @return Calculate indicator type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01739
    /// @needwork = dd
    /// @endcode
    IndicatorType _calcIndicatorType() const noexcept;

    /// @name behavior_
    /// The behavior definition enumeration of this indicator, for example: fast flashing mode, slow flashing mode, etc.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01740
    /// @needwork = dd
    /// @endcode
    DiagnosticConnectedIndicatorBehaviorEnum behavior_{DiagnosticConnectedIndicatorBehaviorEnum::kBlinkMode};

    /// @name healingCycleCounterThreshold_
    /// Threshold of healing cycle count, when this count reaches this value the corresponding WIR state will be reset
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01741
    /// @needwork = dd
    /// @endcode
    uint32_t healingCycleCounterThreshold_{0};

    /// @name counter_
    /// Internal count of healing cycle, this count will increment at the end of each healing cycle when no fault occurs
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01742
    /// @needwork = dd
    /// @endcode
    uint32_t counter_{0};

    /// @name events_
    /// Since the correspondence between event and indicator is many-to-many, all events bound to this indicator are defined here
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01743
    /// @needwork = dd
    /// @endcode
    std::vector< std::weak_ptr< Event > > events_;

    /// @name wir_
    /// Internally recorded WIR state, bound event needs to call SetStatusWIR to modify this state, it will be used when calculating the type of this indicator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01744
    /// @needwork = dd
    /// @endcode
    bool wir_{false};

    /// @brief Configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01745
    /// @needwork = dd
    /// @endcode
    IndicatorConfig config_{};
};
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_INDICATOR_H__