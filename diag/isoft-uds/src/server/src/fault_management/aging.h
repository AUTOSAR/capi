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
/// @file       aging.h
/// @brief      This file provides aging algorithm type definitions.
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
/// @trace_id_sr=407bb799-a443-4c6f-be6a-89a25a6d2286
/// @unit_name=aging
/// @unit_description=This file provides aging functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_AGING_H__
#define __ISOFT_UDS_FAULT_AGING_H__

#include <map>
#include <memory>
#include <vector>

#include "isoft/uds/fault_management/fault_management.h"
#include "isoft/uds/fault_management/operation_cycle_interface.h"

namespace isoft {
namespace uds {
namespace server {

class AgingConfig;

class OperationCycle;

/// @brief Aging implementation class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02564
/// @needwork = dd
/// @endcode
class Aging final : public std::enable_shared_from_this< Aging >
{
public:
    /// @brief Parse parameters from configuration
    /// @param[in] node Configuration node
    /// @param[in] operationCycles All OperationCycles, the configured AgingCycle will be found here
    /// @return Returns true on success, false on failure
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool Init(AgingConfig& config, std::map< uint32_t, std::shared_ptr< OperationCycle > >& operationCycles);

    /// @brief Get the threshold configured for this Aging
    /// @return threshold
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01953
    /// @needwork = dd
    /// @endcode
    uint32_t GetThreshold() const noexcept { return config_.threshold; }

    /// @brief Set callback for AgingCycle state change
    /// @param[in] notifier Callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01954
    /// @needwork = dd
    /// @endcode
    void SetNotifier(std::function< void() > const& notifier);

    /// @brief When the state of the bound OperationCycle changes, it will be called here
    /// @param[in] oldState Old state
    /// @param[in] newState New state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01955
    /// @needwork = dd
    /// @endcode
    void ReportOperationCycleStateTransition(OperationCycleType const oldState,
                                             OperationCycleType const newState) const;

private:
    /// @brief Configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01956
    /// @needwork = dd
    /// @endcode
    AgingConfig config_{};

    /// @brief Callback function when AgingCycle state changes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01957
    /// @needwork = dd
    /// @endcode
    std::vector< std::function< void() > > notifiers_{};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_AGING_H__