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
/// @file       debounce_user_defined.h
/// @brief      This file provides debounce_user_defined functionality
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
/// @trace_id_sr=57fe8d5b-de31-489c-b770-51fa6668be20
/// @unit_name=DebounceUserDefined
/// @unit_description=This file provides debounce_user_defined functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_DEBOUNCE_USER_DEFINED_H__
#define __ISOFT_UDS_FAULT_DEBOUNCE_USER_DEFINED_H__

#include "fault_management/debounce_interface.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief User-defined debouncing class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02565
/// @needwork = dd
/// @endcode
class DebounceUserDefined : public DebounceInterface
{
public:
    /// @brief User-defined debouncing class constructor
    using DebounceInterface::DebounceInterface;

    /// @brief User-defined debouncing class report status
    /// @param[in] action The MonitorAction parameter passed by Event
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void IndicateReportedStatus(MonitorAction action) override;

    /// @brief User-defined debouncing class reset
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void Reset() override;

    /// @brief User-defined debouncing class freeze
    /// @param[in] b Whether to Freeze
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void Freeze(bool b) noexcept override;

    /// @brief User-defined debouncing class get type
    /// @return Debouncing type
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    DebounceTypeEnum GetType() const noexcept override;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __DEBOUNCE_USER_DEFINED_H__