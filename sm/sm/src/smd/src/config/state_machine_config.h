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
/// @file       state_machine_config.h
/// @brief      Read configuration files related to state machines
/// @details
/// @date       2024-05-02
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Config
/// @unit_name=StateMachineConfig
/// @interface_level=module
/// @unit_description=Read configuration files related to state machines
/// @trace_id_sr=SR_SM_00107
/// @endcode
///
/// ================================================================

#ifndef STATE_MACHINE_CONFIG_H_
#define STATE_MACHINE_CONFIG_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <ara/exec/execution_error_event.h>
#include <ara/log/logger.h>
#include <ara/sm/impl_type_transitionrequesttype.h>
#include <isoft/manifestreader/manifest_reader.h>

namespace ara {
namespace sm {
namespace config {

/// State machine service information
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00107
/// @trace_id_ad=AD_SM_08030
/// @trace_id_dd=DD_SM_08071
/// @needwork = ad
/// @endcode
struct SMServiceInfo
{
    /// @brief State machine FQN
    core::String smFQN;

    /// @brief TriggerOutSMState channel
    core::String notificationPort;

    /// @brief SM service InstanceID
    core::String instanceId;
};

/// @brief Action item information
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00107
/// @trace_id_ad=AD_SM_08031
/// @trace_id_dd=DD_SM_08072
/// @needwork = ad
/// @endcode
struct SMAcionItemInfo
{
    /// @brief Action type: SetFunctionGroupState, Sync, StartClientSM, StopClientSM, ControlNMNetwork, Sleep
    core::String type;

    /// @brief Specific behavior, different types correspond to different behaviors
    /// setFunctionGroupState: "MachineFG:Verify"
    /// sync: ""
    /// startClientSM: "ClientFG:Verify" or "ClientFG"
    /// stoptClientSM: "ClientFG"
    /// controlNMNetwork: "nmhandle1:FullCom"
    /// sleep: "500"
    core::String behavior;
};

/// @brief State machine configuration information
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00107
/// @trace_id_ad=AD_SM_08032
/// @trace_id_dd=DD_SM_08073
/// @needwork = ad
/// @endcode
struct SMConfigInfo
{
    /// @brief State machine FQN
    core::String smFQN;

    /// @brief Default start state when the state machine is started
    core::String initialStartState;

    /// @brief Mapping of state machine states to their action lists
    core::Map< core::String, core::Vector< SMAcionItemInfo > > state2ActionList;

    /// @brief List of transition requests
    /// Table which defines next StateMachine State, depending on current StateMachine State and on value passed via
    /// StateMachineService interface. TransitionRequestTable should be 2 maps:
    /// map1, from TransitionRequest (TransitionRequestType) to map2; map2, from Current State to Next State.
    core::Map< TransitionRequestType, core::Map< core::String, core::String > > transitionRequestTable;

    /// @brief Extended transition request list, used for state machine state switching caused by network state changes in the 23-11 standard
    /// key is netHandle.FULL-COM, e.g., /ISOFT/Development/Machine1/nm1_machine1/ln1.FULL-COM or
    /// key is netHandle.NO-COM, e.g., /ISOFT/Development/Machine1/nm1_machine1/ln1.NO-COM
    /// value is the same as transitionRequestTable
    core::Map< core::String, core::Map< core::String, core::String > > extendTransitionRequestTable;

    /// @brief Error recovery list
    /// Table which defines next StateMachine State, depending on ErrorEvent value passed in StateMachine error
    /// notification. Each ErrorRecoveryTable should be a map from ExecutionError to Next State.
    core::Map< ara::exec::ExecutionError, core::String > errorRecoveryTable;

    /// @brief State to switch to when an unknown error occurs in this state machine
    core::String anyErrRecoveryState;
};

/// @brief The class for parsing the config files and getting state machine info
/// @code{.isoft}
/// @interface_level=uint
/// @trace_id_sr=SR_SM_00107
/// @trace_id_sr=SR_SM_00107
/// @interface_level=unit
/// @trace_id_sr=SR_SM_00107
/// @trace_id_ad=AD_SM_08033
/// @trace_id_dd=DD_SM_08074
/// @needwork = ad
/// @endcode
class StateMachineConfig
{
public:
    /// @brief Constructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00035
    /// @trace_id_dd=DD_SM_00035
    /// @needwork = ad
    /// @endcode
    StateMachineConfig() noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00036
    /// @trace_id_dd=DD_SM_00036
    /// @needwork = ad
    /// @endcode
    ~StateMachineConfig() noexcept;

    /// @brief deleted copy constructor function
    /// @param other The StateMachineConfig instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00037
    /// @trace_id_dd=DD_SM_00037
    /// @needwork = ad
    /// @endcode
    StateMachineConfig(StateMachineConfig const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The StateMachineConfig instance to be copyed
    /// @return the assigned StateMachineConfig instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00038
    /// @trace_id_dd=DD_SM_00038
    /// @needwork = ad
    /// @endcode
    StateMachineConfig& operator=(StateMachineConfig const& other) = delete;

    /// @brief Move constructor function
    /// @param other The StateMachineConfig instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00039
    /// @trace_id_dd=DD_SM_00039
    /// @needwork = ad
    /// @endcode
    StateMachineConfig(StateMachineConfig&& other) = default;

    /// @brief Move assignment function
    /// @param other The StateMachineConfig instance to be moved
    /// @return the assigned StateMachineConfig instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00040
    /// @trace_id_dd=DD_SM_00040
    /// @needwork = ad
    /// @endcode
    StateMachineConfig& operator=(StateMachineConfig&& other) = delete;

    /// @brief Get the list of all state machine service information
    /// @return List of state machine service information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00041
    /// @trace_id_dd=DD_SM_00041
    /// @needwork = ad
    /// @endcode
    core::Vector< SMServiceInfo > const& GetAllSMServiceInfoList() const noexcept;

    /// @brief Get the list of all state machine configuration information
    /// @return List of state machine configuration information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00042
    /// @trace_id_dd=DD_SM_00042
    /// @needwork = ad
    /// @endcode
    core::Vector< SMConfigInfo > const& GetAllSMConfigInfoList() const noexcept;

    /// @brief Load all state machine' info list
    /// @param manifestPath config file path
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00043
    /// @trace_id_dd=DD_SM_00043
    /// @needwork = ad
    /// @endcode
    bool Load(core::String const& manifestPath) noexcept;

private:
    /// @brief Parse state machine services and state machine states
    /// @param stateMachineManifest State machine configuration node
    /// @return true Parsing successful
    /// @return false Parsing failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08075
    /// @needwork = dda
    /// @endcode
    bool _iterateNotifications(std::unique_ptr< isoft::manifestreader::Manifest > const& stateMachineManifest) noexcept;

    /// @brief Parse the action list of state machine states
    /// @param stateMachineManifest State machine configuration node
    /// @return true Parsing successful
    /// @return false Parsing failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08076
    /// @needwork = dda
    /// @endcode
    bool _iterateActionItemLists(
        std::unique_ptr< isoft::manifestreader::Manifest > const& stateMachineManifest) noexcept;

    /// @brief Parse the state transition table and error recovery table of state machine states
    /// @param stateMachineManifest State machine configuration node
    /// @return true Parsing successful
    /// @return false Parsing failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08077
    /// @needwork = dda
    /// @endcode
    bool _iterateRequests(std::unique_ptr< isoft::manifestreader::Manifest > const& stateMachineManifest) noexcept;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08078
    /// @needwork = dda
    /// @endcode
    log::Logger& log_;

    /// @brief List of state machine configuration information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08079
    /// @needwork = dda
    /// @endcode
    core::Vector< SMConfigInfo > smConfigInfoList_;

    /// @brief List of state machine service information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08080
    /// @needwork = dda
    /// @endcode
    core::Vector< SMServiceInfo > smServiceInfoList_;

    /// @brief Mapping of function group FQN to state machine FQN
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00107
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08081
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, core::String > fg2StateMachineMap_;
};
}  // namespace config
}  // namespace sm
}  // namespace ara

#endif  //
