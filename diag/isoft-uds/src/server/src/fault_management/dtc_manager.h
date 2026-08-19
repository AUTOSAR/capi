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
/// @file       dtc_manager.h
/// @brief      This file provides dtc_manager functionality
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
/// @trace_id_sr=2a9a2235-e901-4a80-8cb3-16d2d114c719
/// @unit_name=DtcManager
/// @unit_description=This file provides dtc_manager functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_DTC_MANAGER_H__
#define __ISOFT_UDS_FAULT_DTC_MANAGER_H__

#include "fault_management/public.h"
#include "fault_management/types.h"
#include "isoft/uds/uds_nrc_error_domain.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief FaultManagementRuntimeContext class declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02461
/// @needwork = dd
/// @endcode
class FaultManagementRuntimeContext;

/// @brief DtcManager class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02462
/// @needwork = dd
/// @endcode
class DtcManager final
{
public:
    /// @brief Constructor
    /// @param[in] context Runtime context wrapper class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01531
    /// @needwork = dd
    /// @endcode
    explicit DtcManager(FaultManagementRuntimeContext& context) noexcept;

    /// @brief Copy constructor
    /// @param[in] other DtcManager reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01532
    /// @needwork = dd
    /// @endcode
    DtcManager(DtcManager const& other) = default;

    /// @brief Move constructor
    /// @param[in] other DtcManager rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01533
    /// @needwork = dd
    /// @endcode
    DtcManager(DtcManager&& other) = default;

    /// @brief Copy assignment operator
    /// @param[in] right DtcManager reference
    /// @return DtcManager reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01534
    /// @needwork = dd
    /// @endcode
    DtcManager& operator=(DtcManager const& right) = delete;

    /// @brief Move assignment operator
    /// @param[in] right DtcManager rvalue reference
    /// @return DtcManager reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01535
    /// @needwork = dd
    /// @endcode
    DtcManager& operator=(DtcManager&& right) = delete;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01536
    /// @needwork = dd
    /// @endcode
    ~DtcManager() = default;

public:
    /// @brief Implement clear operation initiated by UDS service, clear dtc or dtc group of specified destination
    /// @brief Clear dtc
    /// @param[in] groupOfDtc dtc or dtc group
    /// @param[in] isUserDefined Whether it is a user-defined destination
    /// @param[in] memIdUserDefined User-defined destination marker
    /// @return result
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    int32_t ClearDTC(uint32_t const groupOfDtc, bool const isUserDefined, uint8_t const memIdUserDefined) const;

protected:
    /// @brief Check whether the incoming dtc parameter is valid
    /// @param[in] groupOfDtc dtc or dtc group
    /// @return true or false
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool _CheckCondition(uint32_t const groupOfDtc) const noexcept;

    /// @brief Check whether it is a dtc configured during modeling
    /// @param[in] groupOfDtc dtc or dtc group
    /// @return true or false
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool _IsDTCConfigured(uint32_t const groupOfDtc) const;

    /// @brief Check whether clear operation of dtc or dtc group is restricted
    /// @param[in] groupOfDtc dtc or dtc group
    /// @return true or false
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool _IsForbiddenClear(uint32_t const groupOfDtc) const;

    /// @brief Check whether the specified dtc is affected by the configuration parameter clearDtcLimitation in commonprops
    /// @param[in] groupOfDTCs dtc or dtc group
    /// @return true or false
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool _IsDTCClearAllowedByCommonProperties(uint32_t const groupOfDTCs) const noexcept;

private:
    /// @name context_
    /// dem module runtime context wrapper class, record this to facilitate obtaining various data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01537
    /// @needwork = dd
    /// @endcode
    FaultManagementRuntimeContext& context_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_DTC_MANAGER_H__