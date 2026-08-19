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
/// @file       condition_interface.h
/// @brief      This file provides condition_interface functionality
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
/// @trace_id_sr=3d645365-e375-4a09-a01b-88da5d2e903c
/// @unit_name=ConditionInterface
/// @unit_description=This file provides condition_interface functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_CONDITION_INTERFACE_H__
#define __ISOFT_UDS_FAULT_CONDITION_INTERFACE_H__

#include <cstdint>
#include <functional>
#include <memory>

namespace isoft {
namespace uds {
namespace server {

/// @brief Set condition callback
using SetConditionCallBack = std::function< void(bool) >;
/// @brief Get condition callback
using GetConditionCallBack = std::function< bool() >;

/// @brief Condition interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01369
/// @trace_id_dd=DD_UDS_02661
/// @needwork = ad
/// @endcode
class ConditionInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01015
    /// @needwork = dda
    /// @endcode
    ConditionInterface(ConditionInterface const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01016
    /// @needwork = dda
    /// @endcode
    ConditionInterface(ConditionInterface&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01017
    /// @needwork = dda
    /// @endcode
    ConditionInterface& operator=(ConditionInterface const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01018
    /// @needwork = dda
    /// @endcode
    ConditionInterface& operator=(ConditionInterface&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01122
    /// @trace_id_dd=DD_UDS_02236
    /// @needwork = ad
    /// @endcode
    ConditionInterface() noexcept = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01123
    /// @trace_id_dd=DD_UDS_02237
    /// @needwork = ad
    /// @endcode
    virtual ~ConditionInterface() noexcept = default;

public:
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note Register set condition callback
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01124
    /// @trace_id_dd=DD_UDS_02238
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterSetCondition(SetConditionCallBack const& callBack) = 0;

    /// @brief Register get condition callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note Register get condition callback
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01125
    /// @trace_id_dd=DD_UDS_02239
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterGetCondition(GetConditionCallBack const& callBack) = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // DM_UDS_GENERIC_UDS_SERVICE_H_