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
/// @file       define.h
/// @brief      define some global variables
/// @details
/// @date       2024-06-13
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Utils
/// @unit_name=GlobalVariablesDefine
/// @interface_level=module
/// @unit_description=define some functions for const variables
/// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002,
/// SR_SM_04003, SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
/// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008, SR_SM_10001
/// @endcode
///
/// ================================================================

#ifndef DEFINE_H_
#define DEFINE_H_

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define MAX_WATI_NETWORK_CHANGE_TIME_MS 10000UL

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define DEFAULT_NM_AFTER_RUN_TIME_S 0.5F

#define DEFAULT_SLEEP_TIME_S 0.0F

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define ZERO_UL 0UL

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define ONE_UL 1UL

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define TWO_UL 2UL

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define TEN_UL 10UL

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define EIGHTY_UL 80UL

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define TEN_L 10L

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define SIXTEEN_L 16L

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define ONE_HUNDRED 100

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define FIVE_HUNDRED 500

#include <isoft/e2e/Platform_Types.h>

#include <chrono>

namespace ara {
namespace sm {
namespace common {
/// @brief Define alias to resolve qac2428 item: Direct use of character type.
/// @param Char8_t alias of char
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using Char8_t = char;

/// @brief  Get the path separator
/// @return "/"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_5001, SR_SM_5005
/// @trace_id_sr=SR_SM_05001
/// @trace_id_ad=AD_SM_07001
/// @trace_id_dd=DD_SM_00512
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkPathSeparator() noexcept { return "/"; }

/// @brief A compile-time constant denoting the function group name "MachineFG".
/// @return "MachineFG"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002,
/// SR_SM_04003, SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
/// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_07002
/// @trace_id_dd=DD_SM_00513
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkMachineFunctionGroupName() noexcept { return "MachineFG"; }

/// @brief A compile-time constant denoting the 'Off', state of a function group.
/// @return "Off"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003, SR_SM_04004,
/// SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004,
/// SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_07003
/// @trace_id_dd=DD_SM_00514
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkOffState() noexcept { return "Off"; }

/// @brief A compile-time constant denoting the 'Startup', state of a function group.
/// @return "Startup"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003, SR_SM_04004,
/// SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004,
/// SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_07004
/// @trace_id_dd=DD_SM_00515
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkStartupState() noexcept { return "Startup"; }

/// @brief A compile-time constant denoting the 'Verify', state of a function group.
/// @return "Verify"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_04008
/// @trace_id_ad=AD_SM_07005
/// @trace_id_dd=DD_SM_00516
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkFGVerifyState() noexcept { return "Verify"; }

/// @brief A compile-time constant denoting the 'Restart', state of a function group.
/// @return "Restart"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003, SR_SM_04004,
/// SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004,
/// SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_07006
/// @trace_id_dd=DD_SM_00517
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkRestartState() noexcept { return "Restart"; }

/// @brief A compile-time constant denoting the 'Shutdown', state of a function group.
/// @return "Shutdown"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003, SR_SM_04004,
/// SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004,
/// SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_07007
/// @trace_id_dd=DD_SM_00518
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkShutdownState() noexcept { return "Shutdown"; }

/// @brief A compile-time constant representing the name of the function group
/// @return 'OTAUpdateFG'
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_04008
/// @trace_id_ad=AD_SM_07008
/// @trace_id_dd=DD_SM_00519
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkOTAUpdateFGName() noexcept
{
    return "/FunctionGroups/ota_client/FunctionGroupsSet/OTAUpdateFG";
}

/// @brief A compile-time constant denoting the 'Continue', state of a function group.
/// @return "Continue"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_04008
/// @trace_id_ad=AD_SM_07010
/// @trace_id_dd=DD_SM_00521
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkContinueState() noexcept { return "Continue"; }

/// @brief A compile-time constant denoting the 'functionGroups', field of the software_cluster_manifest.json.
/// @return "functionGroups"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00001
/// @trace_id_ad=AD_SM_07011
/// @trace_id_dd=DD_SM_00522
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkFunctionGroups() noexcept { return "functionGroups"; }

/// @brief A compile-time constant denoting the 'provided_trigger_in_function_group_service_instances', field of the trigger_fg_service_instance_manifest.json.
/// @return "provided_trigger_in_function_group_service_instances"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01004
/// @trace_id_ad=AD_SM_07012
/// @trace_id_dd=DD_SM_00523
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkTriggerInFGServiceInstances() noexcept
{
    return "provided_trigger_in_function_group_service_instances";
}

/// @brief A compile-time constant denoting the 'provided_trigger_out_function_group_service_instances', field of the function group trigger service manifest.
/// @return provided_trigger_out_function_group_service_instances
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01004
/// @trace_id_ad=AD_SM_07013
/// @trace_id_dd=DD_SM_00524
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkTriggerOutFGServiceInstances() noexcept
{
    return "provided_trigger_out_function_group_service_instances";
}

/// @brief A compile-time constant denoting the 'provided_trigger_out_function_group_service_instances', field of the trigger_fg_service_instance_manifest.json.
/// @return provided_trigger_in_out_function_group_service_instances
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01004
/// @trace_id_ad=AD_SM_07014
/// @trace_id_dd=DD_SM_00525
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkTriggerIOFGServiceInstances() noexcept
{
    return "provided_trigger_in_out_function_group_service_instances";
}

/// @brief A compile-time constant denoting the 'function_group_fqn', field of the trigger_fg_service_instance_manifest.json.
/// @return function_group_fqn
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01004
/// @trace_id_ad=AD_SM_07015
/// @trace_id_dd=DD_SM_00526
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkFunctionGroupFQN() noexcept { return "function_group_fqn"; }

/// @brief A compile-time constant denoting the 'instance_id', field of the trigger_fg_service_instance_manifest.json.
/// @return instance_id
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01004
/// @trace_id_ad=AD_SM_07016
/// @trace_id_dd=DD_SM_00527
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkInstanceID() noexcept { return "instance_id"; }

/// @brief A compile-time constant denoting the 'notifications', field of the trigger_fg_service_instance_manifest.json.
/// @return notifications
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09008
/// @trace_id_ad=AD_SM_07017
/// @trace_id_dd=DD_SM_00528
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkNotifications() noexcept { return "notifications"; }

/// @brief A compile-time constant denoting the 'fqn', field of the config json file.
/// @return fqn
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09008
/// @trace_id_ad=AD_SM_07018
/// @trace_id_dd=DD_SM_00529
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkFQN() noexcept { return "fqn"; }

/// @brief A compile-time constant denoting the 'notificationPort', field of the state_machine.json.
/// @return notificationPort
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09008
/// @trace_id_ad=AD_SM_07019
/// @trace_id_dd=DD_SM_00530
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkNotificationPort() noexcept { return "notificationPort"; }

/// @brief A compile-time constant denoting the 'stateMachine', field of the state_machine.json.
/// @return stateMachine
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007, SR_SM_09008
/// @trace_id_ad=AD_SM_07020
/// @trace_id_dd=DD_SM_00531
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkStateMachine() noexcept { return "stateMachine"; }

/// @brief A compile-time constant denoting the 'states', field of the state_machine.json and software_cluster_manifest.json.
/// @return "states"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007, SR_SM_09008
/// @trace_id_ad=AD_SM_08139
/// @trace_id_dd=DD_SM_08499
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkStates() noexcept { return "states"; }

/// @brief A compile-time constant denoting the 'state', field of the state_machine.json and software_cluster_manifest.json.
/// @return "state"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001
/// @trace_id_ad=AD_SM_07021
/// @trace_id_dd=DD_SM_00533
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkState() noexcept { return "state"; }

/// @brief A compile-time constant denoting the 'swcls', field of the state_machine.json and software_cluster_manifest.json.
/// @return "swcls"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00001, SR_SM_03002
/// @trace_id_ad=AD_SM_07022
/// @trace_id_dd=DD_SM_00534
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkSwcls() noexcept { return "swcls"; }

/// @brief A compile-time constant denoting the 'swcl_name', field of the state_machine.json and software_cluster_manifest.json.
/// @return "swcl_name"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00001, SR_SM_03002
/// @trace_id_ad=AD_SM_07023
/// @trace_id_dd=DD_SM_00535
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkSwclName() noexcept { return "swcl_name"; }

/// @brief A compile-time constant denoting the 'swcl_version', field of the state_machine.json and software_cluster_manifest.json.
/// @return "swcl_version"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00001, SR_SM_03002
/// @trace_id_ad=AD_SM_07024
/// @trace_id_dd=DD_SM_00536
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkSwclVersion() noexcept { return "swcl_version"; }

/// @brief A compile-time constant denoting the 'diagnosticAddress', field of the state_machine.json and software_cluster_manifest.json.swcl_version
/// @return "swcl_version"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_03002
/// @trace_id_ad=AD_SM_07025
/// @trace_id_dd=DD_SM_00537
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkDiagnosticAddress() noexcept { return "diagnostic_address"; }

/// @brief A compile-time constant denoting the 'physical', field of the state_machine.json and software_cluster_manifest.json.
/// @return "physical"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_03002
/// @trace_id_ad=AD_SM_07026
/// @trace_id_dd=DD_SM_00538
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkPhysicalDiagnosticAddress() noexcept { return "physical"; }

/// @brief A compile-time constant denoting the 'functional', field of the state_machine.json and software_cluster_manifest.json.
/// @return "functional"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_03002
/// @trace_id_ad=AD_SM_07027
/// @trace_id_dd=DD_SM_00539
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkFunctionalDiagnosticAddress() noexcept { return "functional"; }

/// @brief A compile-time constant denoting the 'notificationPort', field of the state_machine.json.
/// @return "notificationPort"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09001
/// @trace_id_ad=AD_SM_07028
/// @trace_id_dd=DD_SM_00540
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkStateMachineServiceInstanceID() noexcept
{
    return "stateMachineService_instance_id";
}

/// @brief A compile-time constant denoting the 'actionItemLists', field of the state_machine.json.
/// @return "actionItemLists"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07029
/// @trace_id_dd=DD_SM_00541
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkActionItemLists() noexcept { return "actionItemLists"; }

/// @brief A compile-time constant denoting the 'actionItems', field of the state_machine.json.
/// @return "actionItems"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07030
/// @trace_id_dd=DD_SM_00542
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkActionItems() noexcept { return "actionItems"; }

/// @brief A compile-time constant denoting the 'type', field of the state_machine.json.
/// @return "type"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07031
/// @trace_id_dd=DD_SM_00543
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkType() noexcept { return "type"; }

/// @brief Action item behavior separator
/// @return ":"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07032
/// @trace_id_dd=DD_SM_00544
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkActionItemBehaviorSeparator() noexcept { return ":"; }

/// @brief A compile-time constant denoting the 'SetFunctionGroupState', action item type of State Machine
/// @return "SetFunctionGroupState"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07033
/// @trace_id_dd=DD_SM_00545
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkTypeSetFunctionGroupState() noexcept { return "SetFunctionGroupState"; }

/// @brief A compile-time constant denoting the 'Sync', action item type of State Machine
/// @return "Sync"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07034
/// @trace_id_dd=DD_SM_00546
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkTypeSync() noexcept { return "Sync"; }

/// @brief A compile-time constant denoting the 'SetNetworkState', action item type of State Machine
/// @return "SetNetworkState"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07035
/// @trace_id_dd=DD_SM_00547
/// @needwork = no
/// @endcode
inline constexpr Char8_t const *GetkTypeSetNetworkState() noexcept { return "SetNetWorkState"; }

inline constexpr Char8_t const *GetkNetworkHandles() noexcept { return "networkHandles"; }

/// @brief A compile-time constant denoting the 'Sleep', action item type of State Machine
/// @return "Sleep"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07036
/// @trace_id_dd=DD_SM_00548
/// @needwork = no
/// @endcode
inline constexpr Char8_t const *GetkTypeSleep() noexcept { return "Sleep"; }

inline constexpr Char8_t const *GetkTypeCtrlStateMachine() noexcept { return "CtrlAgentStateMachine"; }

/// @brief A compile-time constant denoting the 'StartStateMachine', action item type of State Machine
/// @return "StartStateMachine"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07037
/// @trace_id_dd=DD_SM_00549
/// @needwork = no
/// @endcode
inline constexpr Char8_t const *GetkTypeStartStateMachine() noexcept { return "StartStateMachine"; }

/// @brief A compile-time constant denoting the 'StopStateMachine', action item type of State Machine
/// @return "StopStateMachine"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07038
/// @trace_id_dd=DD_SM_00550
/// @needwork = no
/// @endcode
inline constexpr Char8_t const *GetkTypeStopStateMachine() noexcept { return "StopStateMachine"; }

/// @brief A compile-time constant denoting the 'Unknown', action item type of State Machine
/// @return "Unknown"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07039
/// @trace_id_dd=DD_SM_00551
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkTypeUnknown() noexcept { return "Unknown"; }

/// @brief A compile-time constant denoting the action item type of State Machine
/// @return "setFunctionGroupState"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07040
/// @trace_id_dd=DD_SM_00552
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkSetFunctionGroupState() noexcept { return "setFunctionGroupState"; }

/// @brief A compile-time constant denoting the action item type of State Machine
/// @return "startStateMachine"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07041
/// @trace_id_dd=DD_SM_00553
/// @needwork = no
/// @endcode
inline constexpr Char8_t const *GetkStartStateMachine() noexcept { return "startStateMachine"; }

/// @brief A compile-time constant denoting the action item type of State Machine
/// @return "stopStateMachine"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07042
/// @trace_id_dd=DD_SM_00554
/// @needwork = no
/// @endcode
inline constexpr Char8_t const *GetkStopStateMachine() noexcept { return "stopStateMachine"; }

/// @brief A compile-time constant denoting the action item type of State Machine
/// @return "sync"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07043
/// @trace_id_dd=DD_SM_00555
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkSync() noexcept { return "sync"; }

/// @brief A compile-time constant denoting the 'FunctionGroupFQN', field of the state_machine.json.
/// @return "FunctionGroupFQN"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07044
/// @trace_id_dd=DD_SM_00556
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetFunctionGroupFQN() noexcept { return "FunctionGroupFQN"; }

/// @brief A compile-time constant denoting the 'FunctionGroupFQN', field of the state_machine.json.
/// @return "stateMachineFQN"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07045
/// @trace_id_dd=DD_SM_00557
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkStateMachineFQN() noexcept { return "stateMachineFQN"; }

inline constexpr Char8_t const *GetkStartStateMachineFQN() noexcept { return "startStateMachineFQN"; }

inline constexpr Char8_t const *GetkStopStateMachineFQN() noexcept { return "stopStateMachineFQN"; }

inline constexpr Char8_t const *GetkInitialSMState() noexcept { return "initialState"; }

/// @brief A compile-time constant denoting the 'Unknown', field of the state_machine.json.
/// @return "affectedState"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07046
/// @trace_id_dd=DD_SM_00558
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkAffectedState() noexcept { return "affectedState"; }

/// @brief A compile-time constant denoting the 'Unknown', field of the state_machine.json.
/// @return "requests"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07047
/// @trace_id_dd=DD_SM_00559
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkRequests() noexcept { return "requests"; }

/// @brief A compile-time constant denoting the 'Unknown', field of the state_machine.json.
/// @return "RequestTrigger"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07048
/// @trace_id_dd=DD_SM_00560
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkTypeRequestTrigger() noexcept { return "RequestTrigger"; }

/// @brief A compile-time constant denoting the 'Unknown', field of the state_machine.json.
/// @return "RequestError"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09007
/// @trace_id_ad=AD_SM_07049
/// @trace_id_dd=DD_SM_00561
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkTypeRequestError() noexcept { return "RequestError"; }

/// @brief A compile-time constant denoting the 'Unknown', field of the state_machine.json.
/// @return "rules"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07050
/// @trace_id_dd=DD_SM_00562
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkRules() noexcept { return "rules"; }

/// @brief A compile-time constant denoting the 'Unknown', field of the state_machine.json.
/// @return "contextStateMachine"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07051
/// @trace_id_dd=DD_SM_00563
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkContextStateMachine() noexcept { return "contextStateMachine"; }

/// @brief A compile-time constant denoting the 'Unknown', field of the state_machine.json.
/// @return "assumedCurrentState"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07052
/// @trace_id_dd=DD_SM_00564
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkAssumedCurrentState() noexcept { return "assumedCurrentState"; }

/// @brief A compile-time constant denoting the 'Unknown', field of the state_machine.json.
/// @return "compareValue"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07053
/// @trace_id_dd=DD_SM_00565
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkCompareValue() noexcept { return "compareValue"; }

/// @brief A compile-time constant denoting the 'nextState', field of the state_machine.json.
/// @return "nextState"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_07054
/// @trace_id_dd=DD_SM_00566
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkNextState() noexcept { return "nextState"; }

/// @brief A compile-time constant denoting the 'ANY', value of the RequestError
/// @return "ANY"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09002, SR_SM_09007
/// @trace_id_ad=AD_SM_07055
/// @trace_id_dd=DD_SM_00567
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkANY() noexcept { return "ANY"; }

/// @brief A compile-time constant denoting the 'Initial' state of State Machine
/// @return "Initial"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003, SR_SM_04004,
/// SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004,
/// SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_07056
/// @trace_id_dd=DD_SM_00568
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkInitialState() noexcept { return "Initial"; }

/// @brief A compile-time constant denoting the 'Final', state of State Machine
/// @return "Final"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003, SR_SM_04004,
/// SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004,
/// SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_07058
/// @trace_id_dd=DD_SM_00570
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkFinalState() noexcept { return "Final"; }

/// @brief A compile-time constant denoting the 'inTransition', state of State Machine
/// @return "inTransition"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09008
/// @trace_id_ad=AD_SM_07059
/// @trace_id_dd=DD_SM_00571
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkInTransition() noexcept { return "inTransition"; }

/// @brief A compile-time constant denoting the 'nmAfterRunTimeInfo', the delay time for NM network
/// @return "nmAfterRunTimeInfo"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07060
/// @trace_id_dd=DD_SM_00572
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkNMAfterRunInfo() noexcept { return "nmAfterRunTimeInfo"; }

/// @brief A compile-time constant denoting the 'fgOffTime', the delay time for turning off functional group when the network is turned off
/// @return "functional"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07061
/// @trace_id_dd=DD_SM_00573
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkFgOffTime() noexcept { return "fgOffTime"; }

/// @brief A compile-time constant denoting the 'networkOffTime', the delay time for turning off network when the functional group is turned off
/// @return "networkOffTime"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07062
/// @trace_id_dd=DD_SM_00574
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkNetworkOffTime() noexcept { return "networkOffTime"; }

inline constexpr Char8_t const *GetkSleepTime() noexcept { return "sleepTime"; }

/// @brief The default afterrun time for nm network
/// @return 0.5
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_08106
/// @trace_id_dd=DD_SM_08395
/// @needwork = ad
/// @endcode
inline constexpr float32 GetkDefaultNMAfterRunTime() noexcept
{
    return static_cast< float32 >(DEFAULT_NM_AFTER_RUN_TIME_S);
}

inline constexpr float32 GetkDefaultSleepTime() noexcept { return static_cast< float32 >(DEFAULT_SLEEP_TIME_S); }

/// @brief Health status type for PHM
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_08107
/// @trace_id_dd=DD_SM_08396
/// @needwork = ad
/// @endcode
enum class PHMHealthStatusType : std::uint16_t
{
    /// @brief Tyre pressure type
    kTyrePressure = 1,
    /// @brief voltage type
    kVol = 2,
};

/// @brief A compile-time constant denoting the 'nm.json', config file for NM
/// @return "nmmapping.json"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07063
/// @trace_id_dd=DD_SM_00575
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkNMManifestFileName() noexcept { return "nm_mapping.json"; }

/// @brief A compile-time constant denoting the 'NmhandleInstances', field of the nm.json.
/// @return "nmhandleInstances"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07064
/// @trace_id_dd=DD_SM_00576
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkNMHandleInstances() noexcept { return "nmHandleInstances"; }

/// @brief A compile-time constant denoting the 'NmHandleToFunctionGroupStateMapping', field of the nm.json.
/// @return "nmHandleToFunctionGroupStateMapping"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07065
/// @trace_id_dd=DD_SM_00577
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkNMHandleToFunctionGroupStateMapping() noexcept
{
    return "nmHandleToFunctionGroupStateMapping";
}

/// @brief A compile-time constant denoting the 'nmhandle', field of the nm.json.
/// @return "nmhandle"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07066
/// @trace_id_dd=DD_SM_00578
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkNMHandle() noexcept { return "nmhandle"; }

/// @brief A compile-time constant denoting the 'sm_required_instance_id', field of the nm.json.
/// @return "sm_required_instance_id"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07067
/// @trace_id_dd=DD_SM_00579
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkRequiredNMInstanceID() noexcept { return "sm_required_instance_id"; }

inline constexpr Char8_t const *GetkRequiredPortForIpc() noexcept { return "sm_required_port_for_ipc"; }

/// @brief A compile-time constant denoting the 'mappingDirection', field of the nm.json.
/// @return "mappingDirection"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07068
/// @trace_id_dd=DD_SM_00580
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkMappingDirection() noexcept { return "mappingDirection"; }

/// @brief A compile-time constant denoting the 'functionGroupStateToNmHandle', value of the nm.json.
/// @return "functionGroupStateToNmHandle"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07069
/// @trace_id_dd=DD_SM_00581
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkFunctionGroupStateToNmHandle() noexcept { return "functionGroupStateToNmHandle"; }

/// @brief A compile-time constant denoting the 'nmHandleActiveToFunctionGroupState', value of the nm.json.
/// @return "nmHandleActiveToFunctionGroupState"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07070
/// @trace_id_dd=DD_SM_00582
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkNMHandleActiveToFunctionGroupState() noexcept
{
    return "nmHandleActiveToFunctionGroupState";
}

/// @brief A compile-time constant denoting the 'nmHandleInactiveToFunctionGroupState', value of the nm.json.
/// @return "nmHandleInactiveToFunctionGroupState"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07071
/// @trace_id_dd=DD_SM_00583
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkNMHandleInactiveToFunctionGroupState() noexcept
{
    return "nmHandleInactiveToFunctionGroupState";
}

/// @brief A compile-time constant denoting the 'functionGroupStates', field of the nm.json.
/// @return "name"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005, SR_SM_00001
/// @trace_id_ad=AD_SM_07072
/// @trace_id_dd=DD_SM_00584
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkFunctionGroupName() noexcept { return "name"; }

/// @brief A compile-time constant denoting the 'functionGroupStates', field of the nm.json.
/// @return "functionGroupStates"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07073
/// @trace_id_dd=DD_SM_00585
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkFunctionGroupStates() noexcept { return "functionGroupStates"; }

/// @brief Update session's KV persistent storage identifier
/// @return "smd/state_manager/UpdateSessionStatusPRPort"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_04004
/// @trace_id_ad=AD_SM_07074
/// @trace_id_dd=DD_SM_00586
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkKVModelUpdateSessionIdentifier() noexcept
{
    return "smd/state_manager/UpdateSessionStatusPRPort";
}

/// @brief calibration data's KV persistent storage identifier
/// @return "smd/state_manager/CalibrationDataPRPort"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00002
/// @trace_id_ad=AD_SM_07075
/// @trace_id_dd=DD_SM_00587
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkKVModelCalibrationIdentifier() noexcept
{
    return "smd/state_manager/CalibrationDataPRPort";
}

/// @brief A compile-time constant denoting the Ecu Reset Request Identifier
/// @return "smd/state_manager/DiagnosticEcuResetPPort"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_03002
/// @trace_id_ad=AD_SM_07076
/// @trace_id_dd=DD_SM_00588
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkEcuResetRequestIdentifier() noexcept
{
    return "smd/state_manager/DiagnosticEcuResetPPort";
}

/// @brief A compile-time constant denoting the TriggerInEcuState Identifier
/// @return "smd/state_manager/TriggerIn_State"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_07001, SR_SM_07002
/// @trace_id_ad=AD_SM_07077
/// @trace_id_dd=DD_SM_00589
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkTriggerInEcuStateIdentifier() noexcept
{
    return "smd/state_manager/TriggerIn_State";
}

/// @brief A compile-time constant denoting the TriggerIOEcuState Identifier
/// @return "smd/state_manager/TriggerInOut_State"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_07001, SR_SM_07002
/// @trace_id_ad=AD_SM_07078
/// @trace_id_dd=DD_SM_00590
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkTriggerIOEcuStateIdentifier() noexcept
{
    return "smd/state_manager/TriggerInOut_State";
}

/// @brief A compile-time constant denoting the TriggerOutEcuState Identifier
/// @return "smd/state_manager/TriggerOut_State"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_07001, SR_SM_07002
/// @trace_id_ad=AD_SM_07079
/// @trace_id_dd=DD_SM_00591
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkTriggerOutEcuStateIdentifier() noexcept
{
    return "smd/state_manager/TriggerOut_State";
}

/// @brief A compile-time constant denoting the UpdateRequest Identifier
/// @return "smd/state_manager/UpdateRequest"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
/// @trace_id_ad=AD_SM_07080
/// @trace_id_dd=DD_SM_00592
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkUpdateRequestIdentifier() noexcept { return "smd/state_manager/UpdateRequest"; }

/// @brief A compile-time constant denoting the ShellRequest Identifier
/// @return "smd/state_manager/ShellRequest"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_10001
/// @trace_id_ad=AD_SM_07081
/// @trace_id_dd=DD_SM_00593
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkShellRequestIdentifier() noexcept { return "smd/state_manager/ShellRequest"; }

/// @brief Key for update session status
/// @return "ara/sm"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_07082
/// @trace_id_dd=DD_SM_00594
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkFGStateNotifyServerIdentifier() noexcept { return "ara/sm"; }

/// @brief Key for update session status
/// @return "UpdateSessionStatus"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_04004
/// @trace_id_ad=AD_SM_07083
/// @trace_id_dd=DD_SM_00595
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkUpdateSessionStatusKey() noexcept { return "UpdateSessionStatus"; }

/// @brief Key for update FG set
/// @return "UpdatingFGSet"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_04004
/// @trace_id_ad=AD_SM_07084
/// @trace_id_dd=DD_SM_00596
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkUpdatingFgSetKey() noexcept { return "UpdatingFGSet"; }

/// @brief Key for deactivated FG
/// @return "DeactivatedFGs"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00002
/// @trace_id_ad=AD_SM_07085
/// @trace_id_dd=DD_SM_00597
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkDeactivatedFunctionGroups() noexcept { return "DeactivatedFGs"; }

/// @brief A compile-time constant denoting the EngineRecoveryAction Identifier
/// @return "smd/state_manager/EngineSupervisionRecoveryNotification"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_07086
/// @trace_id_dd=DD_SM_00598
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkEngineRecoveryActionIdentifier() noexcept
{
    return "smd/state_manager/EngineSupervisionRecoveryNotification";
}

/// @brief A compile-time constant denoting the VolHealthChannelAction Identifier
/// @return "smd/state_manager/VoltagePressureHealthChannelRecoveryNotification"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_07087
/// @trace_id_dd=DD_SM_00599
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkVolHealthChannelActionIdentifier() noexcept
{
    return "smd/state_manager/VoltagePressureHealthChannelRecoveryNotification";
}

/// @brief A compile-time constant denoting the HcTestunitHealthChannelAction Identifier
/// @return "smd/state_manager/TestUnitHealthChannelRecoveryNotification"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_07088
/// @trace_id_dd=DD_SM_00600
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkHcTestUnitHealthChannelActionIdentifier() noexcept
{
    return "smd/state_manager/TestUnitHealthChannelRecoveryNotification";
}

/// @brief A compile-time constant denoting the SvTestunitRecoveryAction Identifier
/// @return "smd/state_manager/TestUnitHealthSupervisionRecoveryNotification"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_07089
/// @trace_id_dd=DD_SM_00601
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkSvTestUnitRecoveryActionIdentifier() noexcept
{
    return "smd/state_manager/TestUnitHealthSupervisionRecoveryNotification";
}

/// @brief A compile-time constant denoting the WheelRecoveryAction Identifier
/// @return "smd/state_manager/WheelSupervisionRecoveryNotification"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_07090
/// @trace_id_dd=DD_SM_00602
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkWheelRecoveryActionIdentifier() noexcept
{
    return "smd/state_manager/WheelSupervisionRecoveryNotification";
}

/// @brief A compile-time constant denoting the TyrePressureHealthChannelAction Identifier
/// @return "smd/state_manager/TyrePressureHealthChannelRecoveryNotification"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_07091
/// @trace_id_dd=DD_SM_00603
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkTyrePressureHealthChannelActionIdentifier() noexcept
{
    return "smd/state_manager/TyrePressureHealthChannelRecoveryNotification";
}

/// @brief Key for target address
/// @return "GetkTA()"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_03002
/// @trace_id_ad=AD_SM_07092
/// @trace_id_dd=DD_SM_00604
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkTA() noexcept { return "kTA"; }

/// @brief Key for target address type
/// @return "GetkTAType()"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_03002
/// @trace_id_ad=AD_SM_07093
/// @trace_id_dd=DD_SM_00605
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkTAType() noexcept { return "kTAType"; }

/// @brief A compile-time constant denoting the physical address
/// @return "PHYS"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_03002
/// @trace_id_ad=AD_SM_07094
/// @trace_id_dd=DD_SM_00606
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkPhysicalAddress() noexcept { return "PHYS"; }

/// @brief A compile-time constant denoting the functional address
/// @return "FUNC"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_03002
/// @trace_id_ad=AD_SM_07095
/// @trace_id_dd=DD_SM_00607
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkFunctionalAddress() noexcept { return "FUNC"; }

/// @brief GetkSeperatorSpace
/// @return ", "
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07096
/// @trace_id_dd=DD_SM_00608
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkSeperatorSpace() noexcept { return ", "; }

/// @brief GetkMaxWaitNetworkChangeTime
/// @return Maximum wait time when executing a call
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05004
/// @trace_id_ad=AD_SM_07099
/// @trace_id_dd=DD_SM_00611
/// @needwork = ad
/// @endcode
inline std::chrono::milliseconds GetkMaxWaitNetworkChangeTime() noexcept
{
    return std::chrono::milliseconds(MAX_WATI_NETWORK_CHANGE_TIME_MS);
}

/// @brief GetkSeperator
/// @return ","
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_02001, SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07097
/// @trace_id_dd=DD_SM_00609
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkSeperator() noexcept { return ","; }

/// @brief GetkSeperatorSeperator
/// @return ";; "
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_02001, SR_SM_05001, SR_SM_05005
/// @trace_id_ad=AD_SM_07098
/// @trace_id_dd=DD_SM_00610
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkSeperatorSeperator() noexcept { return ";; "; }

/// @brief GetkSeperatorSeperator
/// @return ";; "
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline constexpr Char8_t const *Getk2DotSeperator() noexcept { return ".. "; }

/// @brief GetkSTO
/// @return "#STO"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00002, SR_SM_04004
/// @trace_id_ad=AD_SM_00477
/// @trace_id_dd=DD_SM_00493
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkSTO() noexcept { return "#STO"; }

/// @brief GetkStorageContext
/// @return "Storage context"
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00002, SR_SM_04004
/// @trace_id_ad=AD_SM_00478
/// @trace_id_dd=DD_SM_00494
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *GetkStorageContext() noexcept { return "Storage context"; }

inline constexpr Char8_t const *GetkNoComStr() noexcept { return "NO-COM"; }
inline constexpr Char8_t const *GetkFullComStr() noexcept { return "FULL-COM"; }

inline constexpr Char8_t const *GetkFgSplitStr() noexcept { return ";"; }

inline constexpr Char8_t const *GetkFgStateSplitStr() noexcept { return "*"; }

}  // namespace common
}  // namespace sm
}  // namespace ara

#endif  // DEFINE_H_
