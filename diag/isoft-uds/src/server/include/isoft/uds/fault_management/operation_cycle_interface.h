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
/// @file       operation_cycle_interface.h
/// @brief      This file provides operation_cycle_interface functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{isoft}
/// @module_path=/isoft/uds/fault_management
/// @interface_level=software
/// @trace_id_sr=4327dbd5-dae7-4303-9a7e-79a8151e721d
/// @unit_name=OperationCycleInterface
/// @unit_description=This file provides operation_cycle_interface functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_OPERATION_INTERFACE_H__
#define __ISOFT_UDS_FAULT_OPERATION_INTERFACE_H__

#include <cstdint>
#include <functional>
#include <memory>

namespace isoft {
namespace uds {
namespace server {

/// @brief Operation cycle type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01380
/// @trace_id_dd=DD_UDS_02672
/// @needwork = ad
/// @endcode
enum class OperationCycleType : uint8_t
{
    kOperationCycleStart = 0x00,  ///< Start/restart the operation cycle.
    kOperationCycleEnd   = 0x01,  ///< End the operation cycle
};

/// @brief Get operation cycle status callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01381
/// @trace_id_dd=DD_UDS_02673
/// @needwork = ad
/// @endcode
using GetOperationCycleCallBack = std::function< OperationCycleType() >;
/// @brief Set operation cycle status callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01382
/// @trace_id_dd=DD_UDS_02674
/// @needwork = ad
/// @endcode
using SetOperationCycleCallBack = std::function< void(OperationCycleType) >;
/// @brief Operation cycle interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01383
/// @trace_id_dd=DD_UDS_02675
/// @needwork = ad
/// @endcode
class OperationCycleInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01027
    /// @needwork = dda
    /// @endcode
    OperationCycleInterface(OperationCycleInterface const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01028
    /// @needwork = dda
    /// @endcode
    OperationCycleInterface(OperationCycleInterface&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01029
    /// @needwork = dda
    /// @endcode
    OperationCycleInterface& operator=(OperationCycleInterface const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01030
    /// @needwork = dda
    /// @endcode
    OperationCycleInterface& operator=(OperationCycleInterface&&) = delete;
    /// @brief Constructor
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01140
    /// @trace_id_dd=DD_UDS_02254
    /// @needwork = ad
    /// @endcode
    OperationCycleInterface() noexcept = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01141
    /// @trace_id_dd=DD_UDS_02255
    /// @needwork = ad
    /// @endcode
    virtual ~OperationCycleInterface() noexcept = default;

public:
    /// @brief Notify operation cycle status
    /// @param operation Operation cycle status
    /// @return Returns 0 on successful call
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01142
    /// @trace_id_dd=DD_UDS_02256
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t NotifyOperationCycle(OperationCycleType const& operation) = 0;
    /// @brief Register get operation cycle status callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01143
    /// @trace_id_dd=DD_UDS_02257
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterGetOperationCycle(GetOperationCycleCallBack const& callBack) = 0;
    /// @brief Register set operation cycle status callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01144
    /// @trace_id_dd=DD_UDS_02258
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterSetOperationCycle(SetOperationCycleCallBack const& callBack) = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // DM_UDS_GENERIC_UDS_SERVICE_H_