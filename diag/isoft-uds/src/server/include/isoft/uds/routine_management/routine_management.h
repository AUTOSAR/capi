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
/// @file       routine_management.h
/// @brief      This file provides routine_management functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/routine_management
/// @interface_level=software
/// @trace_id_sr=9079d06f-f4b7-43e6-8276-9ffa4ba7cc4a
/// @unit_name=RoutineManagement
/// @unit_description=This file provides routine_management functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICROUTINECONTROL_H_
#define ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICROUTINECONTROL_H_
#include <memory>

#include "generic_routine.h"
#include "isoft/uds/authentication_management/authentication_state.h"
#include "isoft/uds/data_management/environmental_condition_model.h"
#include "isoft/uds/security_access_management/security_level.h"
#include "isoft/uds/session_management/session_model.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Routing configuration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01353
/// @trace_id_dd=DD_UDS_02645
/// @needwork = ad
/// @endcode
struct Routine : public ServiceConfigBase
{
    /// @brief Routing ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01083
    /// @trace_id_dd=DD_UDS_02197
    /// @needwork = ad
    /// @endcode
    std::uint16_t id{0U};
    /// @brief Routing information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01084
    /// @trace_id_dd=DD_UDS_02198
    /// @needwork = ad
    /// @endcode
    std::int16_t routineInfo{-1};
    /// @brief -2代表没有子功能 -1代表有子功能但是没有环境条件检查 >0代表既有子功能又有环境条件
    /// @brief Configure environmental condition permissions for start subfunction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01085
    /// @trace_id_dd=DD_UDS_02199
    /// @needwork = ad
    /// @endcode
    EnvironmentalConditionId startAccessPermissionEnvCondition{-2};
    /// @brief Configure P4 timer value for start subfunction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01086
    /// @trace_id_dd=DD_UDS_02200
    /// @needwork = ad
    /// @endcode
    std::uint32_t startP4ServerMax{0U};
    /// @brief Configure environmental condition permissions for stop subfunction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01087
    /// @trace_id_dd=DD_UDS_02201
    /// @needwork = ad
    /// @endcode
    EnvironmentalConditionId stopAccessPermissionEnvCondition{-2};
    /// @brief Configure P4 timer value for stop subfunction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01088
    /// @trace_id_dd=DD_UDS_02202
    /// @needwork = ad
    /// @endcode
    std::uint32_t stopP4ServerMax{0U};
    /// @brief Configure environmental condition permissions for result subfunction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01089
    /// @trace_id_dd=DD_UDS_02203
    /// @needwork = ad
    /// @endcode
    EnvironmentalConditionId getResultAccessPermissionEnvCondition{-2};
    /// @brief Configure P4 timer value for result subfunction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01090
    /// @trace_id_dd=DD_UDS_02204
    /// @needwork = ad
    /// @endcode
    std::uint32_t resultP4ServerMax{0U};
};

/// @brief Routing control instance configuration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01354
/// @trace_id_dd=DD_UDS_02646
/// @needwork = ad
/// @endcode
struct RoutineControlInstanceConfig
{
    /// @brief Routing configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01091
    /// @trace_id_dd=DD_UDS_02205
    /// @needwork = ad
    /// @endcode
    Routine routine{};
    /// @brief Access authentication permission
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01092
    /// @trace_id_dd=DD_UDS_02206
    /// @needwork = ad
    /// @endcode
    EnableAuth accessPermissionAuth{};
    /// @brief Session permission
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01093
    /// @trace_id_dd=DD_UDS_02207
    /// @needwork = ad
    /// @endcode
    std::set< SessionId > sessionPermission{};
    /// @brief Security level permission
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01094
    /// @trace_id_dd=DD_UDS_02208
    /// @needwork = ad
    /// @endcode
    std::set< SecurityLevelId > securityLevelPermission{};
    /// @brief Comparison operator
    /// @param other Another routing control instance configuration
    /// @return Whether less than
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01095
    /// @trace_id_dd=DD_UDS_02209
    /// @needwork = ad
    /// @endcode
    bool operator<(const RoutineControlInstanceConfig &other) const { return routine.id < other.routine.id; }
};

/// @brief Routing management
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01355
/// @trace_id_dd=DD_UDS_02647
/// @needwork = ad
/// @endcode
struct RoutineManagement
{
    /// @brief Routing control instance table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01096
    /// @trace_id_dd=DD_UDS_02210
    /// @needwork = ad
    /// @endcode
    std::set< RoutineControlInstanceConfig > routineInstanceTable{};
    /// @brief Routing interface pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01097
    /// @trace_id_dd=DD_UDS_02211
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< GenericRoutineInterface > interfacePtr{nullptr};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  ///ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICROUTINECONTROL_H_