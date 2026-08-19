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
/// @file       operation_cycle.h
/// @brief      This file provides operation_cycle functionality
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
/// @trace_id_sr=887d3c38-14f0-442b-869b-8ea1df7256c2
/// @unit_name=OperationCycle
/// @unit_description=This file provides operation_cycle functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_OPERATION_CYCLE_H__
#define __ISOFT_UDS_FAULT_OPERATION_CYCLE_H__

#include <functional>
#include <memory>
#include <set>
#include <vector>

#include "isoft/uds/fault_management/fault_management.h"
#include "isoft/uds/fault_management/operation_cycle_interface.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Event class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02549
/// @needwork = dd
/// @endcode
class Event;

/// @brief Indicator class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02550
/// @needwork = dd
/// @endcode
class Indicator;

/// @brief Aging class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02551
/// @needwork = dd
/// @endcode
class Aging;

/// @brief operation cycle class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02552
/// @needwork = dd
/// @endcode
class OperationCycle final
{
public:
    /// @brief OperationCycle constructor
    /// @param config Configuration information
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01881
    /// @needwork = dd
    /// @endcode
    explicit OperationCycle(OperationCycleConfig config) : config_{std::move(config)} {}

    /// @brief OperationCycle copy constructor
    /// @param[in] other OperationCycle reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01882
    /// @needwork = dd
    /// @endcode
    OperationCycle(OperationCycle const& other) = default;

    /// @brief OperationCycle copy constructor
    /// @param[in] other OperationCycle rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01883
    /// @needwork = dd
    /// @endcode
    OperationCycle(OperationCycle&& other) = default;

    /// @brief DemOperationCycle assignment operator
    /// @param[in] right OperationCycle reference
    /// @return OperationCycle reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01884
    /// @needwork = dd
    /// @endcode
    OperationCycle& operator=(OperationCycle const& right) = default;

    /// @brief DemOperationCycle assignment operator
    /// @param[in] right OperationCycle rvalue reference
    /// @return OperationCycle reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01885
    /// @needwork = dd
    /// @endcode
    OperationCycle& operator=(OperationCycle&& right) = default;

    /// @brief DemOperationCycle destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01886
    /// @needwork = dd
    /// @endcode
    ~OperationCycle() = default;

    /// @brief Initialize operation cycle object based on configuration information
    /// @param[in] node Configuration node object
    /// @return true/false Success/Failure
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool Init();

    /// @brief Start service
    /// @return Result
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void Start();

    /// @brief Stop service
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void Stop();

    /// @brief Whether to automatically end operation cycle
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01887
    /// @needwork = dd
    /// @endcode
    bool IsAutoEnd() const noexcept { return config_.autoEnd; }

    /// @brief Whether operation cycle needs to be persisted and stored
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01888
    /// @needwork = dd
    /// @endcode
    bool IsCycleStatusStorage() const noexcept { return config_.cycleStatusStorage; }

    /// @brief Return current operation cycle status
    /// @return Operation cycle status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01889
    /// @needwork = dd
    /// @endcode
    OperationCycleType GetOperationCycleState() const noexcept { return state_; }

    /// @brief Register bound diagnostic event
    /// @param[in] event Diagnostic event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01890
    /// @needwork = dd
    /// @endcode
    void RegisterEvent(std::shared_ptr< Event > const& event) noexcept { events_.push_back(event); }

    /// @brief Register bound fault indicator
    /// @param[in] indicator Fault indicator
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void RegisterIndicator(std::shared_ptr< Indicator > const& indicator) { indicators_.push_back(indicator); }

    /// @brief Register bound aging algorithm object
    /// @param[in] aging Aging algorithm object
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void RegisterAging(std::shared_ptr< Aging > const& aging) { agings_.push_back(aging); }

    /// @brief Get operation cycle ID
    /// @return ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01891
    /// @needwork = dd
    /// @endcode
    uint32_t GetOperationCycle() const noexcept { return config_.operationCycleId; }

private:
    /// @brief Set operation cycle status
    /// @param[in] newState
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void _setOperationCycleState(OperationCycleType const newState);

    /// @name state_
    /// state of operation cycle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01892
    /// @needwork = dd
    /// @endcode
    OperationCycleType state_{OperationCycleType::kOperationCycleEnd};

    /// @name event_
    /// pointer of event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01893
    /// @needwork = dd
    /// @endcode
    std::vector< std::shared_ptr< Event > > events_;

    /// @name indicators_
    /// Indicator vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01894
    /// @needwork = dd
    /// @endcode
    std::vector< std::shared_ptr< Indicator > > indicators_;

    /// @name agings_
    /// Aging vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01895
    /// @needwork = dd
    /// @endcode
    std::vector< std::shared_ptr< Aging > > agings_;

    /// @brief Operation cycle configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01896
    /// @needwork = dd
    /// @endcode
    OperationCycleConfig config_{};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft
#endif  // __ISOFT_UDS_FAULT_OPERATION_CYCLE_H__