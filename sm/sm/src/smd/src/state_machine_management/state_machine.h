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
/// @file       state_machine.h
/// @brief      Define the state machine class
/// @details
/// @date       2024-06-26
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/StateMachineManagement
/// @unit_name=StateMachine
/// @interface_level=uint
/// @unit_description=State machine class
/// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003, SR_SM_04004,
/// SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004,
/// SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008, SR_SM_10001
/// @endcode
///
/// ================================================================

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include <ara/core/optional.h>
#include <ara/core/promise.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <ara/exec/execution_error_event.h>
#include <ara/log/logger.h>
#include <ara/sm/impl_type_transitionrequesttype.h>
#include <isoft/naicpp/global_evloop.h>

#include <map>
#include <memory>
#include <queue>
#include <set>
#include <vector>

#include "config/global_config.h"
#include "define.h"
#include "event.h"
#include "state_machine_management/action_item_base.h"
#include "state_machine_management/state_machine_common.h"

namespace ara {
namespace sm {
namespace state_machine_management {
/// @brief To solve the magic number problem
constexpr float kDEFAULT_RUN_TIME_FACTOR = 0.5F;
/// @brief Source of the function group transition request
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01002, SR_SM_01003, SR_SM_03002, SR_SM_04005, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_09001
/// @trace_id_ad=AD_SM_08071
/// @trace_id_dd=DD_SM_08214
/// @needwork = ad
/// @endcode
enum class FGStateChangeSource : uint32_t
{
    /// @brief From the debugger
    kSHELL = 0,
    /// @brief From AA
    kAA,
    /// @brief From NM
    kNM,
    /// @brief From UCM
    kUCM,
    /// @brief From DM
    kDM,
    /// @brief From inside SM
    kInternal
};

/// @brief Deferred state machine state request
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_05003
/// @trace_id_ad=AD_SM_08072
/// @trace_id_dd=DD_SM_08215
/// @needwork = ad
/// @endcode
struct PendingSMStateRequest
{
    /// @brief State machine state
    core::String smState{};
    /// @brief Request source
    FGStateChangeSource source{};
    /// @brief Result
    std::shared_ptr< core::Promise< void > > promise;
};

/// @brief Convert the source data to a corresponding string identifier
/// @param source Source of the function group or state machine transition request
/// @return String identifier
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_01002, SR_SM_01003, SR_SM_03002, SR_SM_04005, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_09001
/// @trace_id_ad=AD_SM_00212
/// @trace_id_dd=DD_SM_00220
/// @needwork = ad
/// @endcode
inline core::String ToString(FGStateChangeSource const source) noexcept
{
    core::String strSource{"Unknown"};
    switch (source) {
        case FGStateChangeSource::kAA: {
            strSource = "AA";
        } break;
        case FGStateChangeSource::kSHELL: {
            strSource = "SHELL";
        } break;
        case FGStateChangeSource::kNM: {
            strSource = "NM";
        } break;
        case FGStateChangeSource::kUCM: {
            strSource = "UCM";
        } break;
        case FGStateChangeSource::kDM: {
            strSource = "DM";
        } break;
        case FGStateChangeSource::kInternal: {
            strSource = "Internal";
        } break;
        default: {
            strSource = "Unknown";
        } break;
    }
    return strSource;
}

/// @brief State machine class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003, SR_SM_04004,
/// SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004,
/// SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_08073
/// @trace_id_dd=DD_SM_08216
/// @needwork = ad
/// @endcode
class StateMachine
{
public:
    /// @brief Constructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00213
    /// @trace_id_dd=DD_SM_00221
    /// @needwork = ad
    /// @endcode
    StateMachine() noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00214
    /// @trace_id_dd=DD_SM_00222
    /// @needwork = ad
    /// @endcode
    virtual ~StateMachine() noexcept;

    /// @brief deleted copy constructor function
    /// @param other The StateMachine instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00215
    /// @trace_id_dd=DD_SM_00223
    /// @needwork = ad
    /// @endcode
    StateMachine(StateMachine const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The StateMachine instance to be copyed
    /// @return the assigned StateMachine instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00216
    /// @trace_id_dd=DD_SM_00224
    /// @needwork = ad
    /// @endcode
    StateMachine &operator=(StateMachine const &other) = delete;

    /// @brief Move constructor function
    /// @param other The StateMachine instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00217
    /// @trace_id_dd=DD_SM_00225
    /// @needwork = ad
    /// @endcode
    StateMachine(StateMachine &&other) = default;

    /// @brief Move assignment function
    /// @param other The StateMachine instance to be moved
    /// @return the assigned StateMachine instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00218
    /// @trace_id_dd=DD_SM_00226
    /// @needwork = ad
    /// @endcode
    StateMachine &operator=(StateMachine &&other) = delete;

    /// @brief Initialize the state machine according to the state machine configuration information
    /// @param info State machine configuration information
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00219
    /// @trace_id_dd=DD_SM_00227
    /// @needwork = ad
    /// @endcode
    bool Init(sm::config::SMConfigInfo const &info) noexcept;

    /// @brief Find the action item that sets the function group state in the action list. If its function group state is related to the network state, add the corresponding FullCom action item before it.
    /// @param fgFQN Function group name
    /// @param fgState2FullComNmHandlesMap Mapping of function group states to FullCom network list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_sr=SR_SM_05004
    /// @trace_id_ad=AD_SM_00220
    /// @trace_id_dd=DD_SM_00228
    /// @needwork = ad
    /// @endcode
    void ModifyActionListBasedOnFullComNMConfig(
        core::String const &fgFQN,
        core::Map< core::String, core::Vector< core::String > > const &fgState2FullComNmHandlesMap) noexcept;

    /// @brief Find the action item that sets the function group state in the action list. If its function group state is related to the network state, add the corresponding NoCom action item after it.
    /// @param fgFQN Function group name
    /// @param noComNMHandles Networks that need to be switched to NoCom when the function group is in Off state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_sr=SR_SM_05004
    /// @trace_id_ad=AD_SM_00221
    /// @trace_id_dd=DD_SM_00229
    /// @needwork = ad
    /// @endcode
    void ModifyActionListBasedOnNoComNMConfig(core::String const &fgFQN,
                                              core::Vector< core::String > const &noComNMHandles) noexcept;

    /// @brief Insert an action list at the beginning of the state machine action list
    /// @param state State machine state
    /// @param actionItemList The action list to be inserted
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00222
    /// @trace_id_dd=DD_SM_00230
    /// @needwork = ad
    /// @endcode
    bool InsertActionListToBegin(core::String const &state,
                                 std::vector< std::shared_ptr< ActionItemBase > > const &actionItemList) noexcept;

    /// @brief Insert an action list at the end of the state machine action list
    /// @param state State machine state
    /// @param actionItemList The action list to be inserted
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00223
    /// @trace_id_dd=DD_SM_00231
    /// @needwork = no
    /// @endcode
    bool InsertActionListToEnd(core::String const &state,
                               std::vector< std::shared_ptr< ActionItemBase > > const &actionItemList) noexcept;

    /// @brief Handling of RequestSMState event
    /// @param transitionRequest Transition request
    /// @param source Source of the state machine transition request
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001, SR_SM_10001
    /// @trace_id_ad=AD_SM_00224
    /// @trace_id_dd=DD_SM_00232
    /// @needwork = ad
    /// @endcode
    void RequestSMState(TransitionRequestType const transitionRequest,
                        FGStateChangeSource const source,
                        core::Promise< void > &&promise) noexcept;

    void RequestSMStateFromNM(core::String const nmTransitionRequest) noexcept;

    /// @brief Handling of error recovery event, result is not cared about
    /// @param executionErrorEvent Execution error
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_06004, SR_SM_08001, SR_SM_09002
    /// @trace_id_ad=AD_SM_00225
    /// @trace_id_dd=DD_SM_00233
    /// @needwork = ad
    /// @endcode
    void ErrorRecovery(ara::exec::ExecutionErrorEvent const &executionErrorEvent,
                       core::Promise< void > &&promise) noexcept;

    /// @brief Get the current externally broadcast state of the state machine
    /// @return The current externally broadcast state of the state machine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00226
    /// @trace_id_dd=DD_SM_00234
    /// @needwork = ad
    /// @endcode
    core::String GetCurrentNotifierSMState() const noexcept;

    /// @brief Get the current state of the function group
    /// @param fgFQN Function group name
    /// @return Current state of the function group
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_02001, SR_SM_10001
    /// @trace_id_ad=AD_SM_00227
    /// @trace_id_dd=DD_SM_00235
    /// @needwork = ad
    /// @endcode
    core::String const &GetCurrentFGState(core::String const &fgFQN) noexcept;

    /// @brief Get the function groups managed by this state machine
    /// @return The function groups managed by this state machine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00228
    /// @trace_id_dd=DD_SM_00236
    /// @needwork = ad
    /// @endcode
    core::Vector< core::String > GetManagedFGs() noexcept;

    /// @brief  Output the current state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00229
    /// @trace_id_dd=DD_SM_00237
    /// @needwork = no
    /// @endcode
    void Print() const noexcept;

    /// @brief Start the state machine
    /// @param source Source of the state machine start request
    /// @param smState State machine state
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00230
    /// @trace_id_dd=DD_SM_00238
    /// @needwork = ad
    /// @endcode
    core::Future< void > Start(FGStateChangeSource const source,
                               core::String smState,
                               uint64_t const &nmHandleId = 0) noexcept;

    /// @brief Overloaded function to start the state machine
    /// @param promise Used to asynchronously set the execution result
    /// @param source Source of the state machine start request
    /// @param smState State machine state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_sr=SR_SM_01002, SR_SM_03002, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_10001
    /// @trace_id_ad=AD_SM_00231
    /// @trace_id_dd=DD_SM_00239
    /// @needwork = ad
    /// @endcode
    void Start(std::shared_ptr< core::Promise< void > > const &promise,
               FGStateChangeSource const source,
               core::String const &smState,
               uint64_t const &nmHandleId = 0) noexcept;

    /// @brief Stop the state machine
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00232
    /// @trace_id_dd=DD_SM_00240
    /// @needwork = ad
    /// @endcode
    core::Future< void > Stop() noexcept;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001, SR_SM_01003, SR_SM_02002, SR_SM_05004, SR_SM_06001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00233
    /// @trace_id_dd=DD_SM_00241
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > const &appendEventHandler) noexcept;

    /// @brief Register the callback function for obtaining execution errors
    /// @param getExecutionErrorHandler Callback function for obtaining execution errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_06004
    /// @trace_id_ad=AD_SM_00234
    /// @trace_id_dd=DD_SM_00242
    /// @needwork = ad
    /// @endcode
    void RegisterGetExecutionErrorHandler(
        std::function< core::Result< exec::ExecutionErrorEvent >(core::String const &) > const
            &getExecutionErrorHandler) noexcept;

    /// @brief Handling for when the action list processing fails
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00235
    /// @trace_id_dd=DD_SM_00243
    /// @needwork = ad
    /// @endcode
    void OnProcessFailure() noexcept;

    /// @brief Continue processing the action list until the end or encountering SYNC
    /// @param startIndex The index of the action list to start execution next time
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00236
    /// @trace_id_dd=DD_SM_00244
    /// @needwork = ad
    /// @endcode
    void ContinueProcessingActionList(ActionItemList::size_type const startIndex) noexcept;

    /// @brief Determine whether this state machine is the master state machine (i.e., the state machine managing MachineFG)
    /// @return true - is the master state machine
    /// @return false - is not the master state machine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00237
    /// @trace_id_dd=DD_SM_00245
    /// @needwork = ad
    /// @endcode
    bool IsController() const noexcept;

    /// @brief Handling of RequestFGState event
    /// @param fgFQN Function group name
    /// @param fgState Function group state
    /// @param source Source of the function group transition request
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01002, SR_SM_01003, SR_SM_10001
    /// @trace_id_ad=AD_SM_00238
    /// @trace_id_dd=DD_SM_00246
    /// @needwork = ad
    /// @endcode
    void RequestFGState(core::String const &fgFQN,
                        core::String const &fgState,
                        FGStateChangeSource const source,
                        core::Promise< void > &&promise,
                        uint64_t const &nmHandleId) noexcept;

    /// @brief Switch state machine state
    /// @param startFrom State machine working status when starting the transition
    /// @param nextState Target state
    /// @param source Source of the state machine start request
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00239
    /// @trace_id_dd=DD_SM_00247
    /// @needwork = ad
    /// @endcode
    void SwitchToState(StateMachineWorkStatusType const startFrom,
                       core::StringView const nextState,
                       FGStateChangeSource const source,
                       core::Promise< void > &&promise) noexcept;

    /// @brief Initialize the state machine according to function group information
    /// @param smFQN State machine full name
    /// @param info Function group information
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00240
    /// @trace_id_dd=DD_SM_00248
    /// @needwork = ad
    /// @endcode
    bool Init(core::String const &smFQN, sm::config::FGInfo const &info) noexcept;

    /// @brief Initialize the action list
    /// @param info Function group information
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00241
    /// @trace_id_dd=DD_SM_00249
    /// @needwork = ad
    /// @endcode
    bool InitActionItemList(config::FGInfo const &info) noexcept;

    /// @brief Set the Check Shutdown Handler
    /// @param handler The handler which can be used to check whether the machine can be shutdown
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01002, SR_SM_01003, SR_SM_04003, SR_SM_10001
    /// @trace_id_ad=AD_SM_00242
    /// @trace_id_dd=DD_SM_00250
    /// @needwork = ad
    /// @endcode
    void RegisterCheckShutdownHandler(
        std::function< core::Result< void >(core::StringView const, FGStateChangeSource const) > const
            &handler) noexcept;

    /// @brief Set the delay information related to network management
    /// @param info Delay information related to network management
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005
    /// @trace_id_ad=AD_SM_00243
    /// @trace_id_dd=DD_SM_00251
    /// @needwork = ad
    /// @endcode
    void SetNMAfterRunTime(config::NMAfterRunTimeInfo const &info) noexcept;

    /// @brief Set the function handle for checking whether a function group is deactivated
    /// @param handler Function handle for checking whether a function group is deactivated
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003
    /// @trace_id_ad=AD_SM_00244
    /// @trace_id_dd=DD_SM_00252
    /// @needwork = ad
    /// @endcode
    void SetCheckFGDeactivatedHandler(std::function< bool(core::String const &) > const &handler) noexcept;

    /// @brief Called by the action item that switches the function group to notify that the function group state has changed
    /// @param fgFQN Function group full name
    /// @param fgState Function group state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_02002, SM_SR_01003
    /// @trace_id_ad=AD_SM_00245
    /// @trace_id_dd=DD_SM_00253
    /// @needwork = ad
    /// @endcode
    void NotifyFGStateChange(core::String const &fgFQN, core::String const &fgState) const noexcept;

    /// @brief Update the current state of the function group
    /// @param fgFQN Function group full name
    /// @param fgState Function group state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_02002, SM_SR_01003
    /// @trace_id_ad=AD_SM_00246
    /// @trace_id_dd=DD_SM_00254
    /// @needwork = ad
    /// @endcode
    void UpdateFGState(core::String const &fgFQN, core::String const &fgState) noexcept;

    /// @brief Set the function group state and its associated network state
    /// @param fgFQN Function group full name
    /// @param fgState Function group state
    /// @param processID Processing ID, used to filter whether to perform network switching
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01002, SR_SM_03002, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_10001
    /// @trace_id_ad=AD_SM_00247
    /// @trace_id_dd=DD_SM_00255
    /// @needwork = ad
    /// @endcode
    void ChangeFGAndRelatedNetworkStates(core::String const &fgFQN,
                                         core::String const &fgState,
                                         uint32_t const &processID,
                                         std::shared_ptr< core::Promise< void > > const &promise,
                                         uint64_t const &nmHandleId) noexcept;

    /// @brief Publish a function group switching request to EventManager
    /// @param fgFQN Function group full name
    /// @param fgState Function group state
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01002, SR_SM_01003, SR_SM_03002, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_10001
    /// @trace_id_ad=AD_SM_00248
    /// @trace_id_dd=DD_SM_00256
    /// @needwork = ad
    /// @endcode
    void ChangeFGStateOnly(core::String const &fgFQN,
                           core::String const &fgState,
                           core::Promise< core::Future< void > > &&promise) noexcept;

    /// @brief Publish a function group switching request to EventManager
    /// @param fgFQN Function group full name
    /// @param fgState Function group state
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01002, SR_SM_01003, SR_SM_03002, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_10001
    /// @trace_id_ad=AD_SM_00249
    /// @trace_id_dd=DD_SM_00257
    /// @needwork = ad
    /// @endcode
    void ChangeFGStateOnly(core::String const &fgFQN,
                           core::String const &fgState,
                           core::Promise< void > &&promise) noexcept;

    /// @brief Publish a network state switching request to EventManager
    /// @param nmHandle Network name
    /// @param nmState Network state
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05004
    /// @trace_id_ad=AD_SM_00250
    /// @trace_id_dd=DD_SM_00258
    /// @needwork = ad
    /// @endcode
    void ChangeNetworkStateOnly(core::String const &nmHandle,
                                common::NetworkStateInternalType const &nmState,
                                core::Promise< void > &&promise,
                                uint64_t const &nmHandleId) noexcept;

    /// @brief Asynchronously switch a group of networks to the same state
    /// @param nmList List of networks
    /// @param nmState Network state
    /// @param promise Carries the switching result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05004
    /// @trace_id_ad=AD_SM_00251
    /// @trace_id_dd=DD_SM_00259
    /// @needwork = ad
    /// @endcode
    void AsyncChangeNetworkStates(core::Vector< core::String > const &nmList,
                                  common::NetworkStateInternalType const &nmState,
                                  core::Promise< void > &&promise,
                                  uint64_t const &nmHandleId) noexcept;

    /// @brief Register the callback function for starting the state machine to the start state machine action item
    /// @param startStateMachineHandler Callback function for starting the state machine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00252
    /// @trace_id_dd=DD_SM_00260
    /// @needwork = no
    /// @endcode
    void RegisterStartStateMachineHandler(
        std::function< core::Future< void >(core::String const &, core::String const &) > const
            &startStateMachineHandler) noexcept;

    /// @brief Register the callback function for stopping the state machine to the stop state machine action item
    /// @param stopStateMachineHandler Callback function for stopping the state machine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00253
    /// @trace_id_dd=DD_SM_00261
    /// @needwork = no
    /// @endcode
    void RegisterStopStateMachineHandler(
        std::function< core::Future< void >(core::String const &) > const &stopStateMachineHandler) noexcept;

    /// @brief Set the required function handles for all action items in the action list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01002, SR_SM_01002, SR_SM_03002, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_10001
    /// @trace_id_ad=AD_SM_00254
    /// @trace_id_dd=DD_SM_00262
    /// @needwork = ad
    /// @endcode
    void RegisterHandles2Actions() noexcept;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    bool IsUserDefined() const noexcept;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    core::Vector< core::String > const &GetSmStates() const noexcept;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    core::Vector< common::TransitionItemSM > GetTransitionTable() const noexcept;

    void RegisterGetNmHandlerNameFun(std::function< core::String(uint64_t const &) > getNmHandlerNameFun);

private:
    /// @brief Initialize the action list
    /// @param state2ActionList Mapping of state machine states to action lists
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08217
    /// @needwork = dda
    /// @endcode
    bool _initActionItemList(
        core::Map< core::String, core::Vector< config::SMAcionItemInfo > > const &state2ActionList) noexcept;

    /// @brief State machine start event
    /// @param smState State machine state
    /// @param source Source of the state machine start request
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08218
    /// @needwork = dda
    /// @endcode
    void _onStartEvent(core::String const &smState,
                       FGStateChangeSource const source,
                       std::shared_ptr< core::Promise< void > > const &promise,
                       uint64_t const &nmHandleId) noexcept;

    /// @brief State machine stop event
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @interface_level=none
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08498
    /// @needwork = dda
    /// @endcode
    void _onStopEvent(std::shared_ptr< core::Promise< void > > const &promise) noexcept;

    /// @brief Check whether it is possible to transition to the next state machine state
    /// @param nextState Next state
    /// @return Whether the transition is allowed
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08219
    /// @needwork = dda
    /// @endcode
    core::Result< void > _canTransitionTo(core::String const &nextState) const noexcept;

    /// @brief Start executing the action list
    /// @param startFrom State machine working status when starting to process the action list
    /// @param actionItemList Mapping of state machine states to action lists
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08220
    /// @needwork = dda
    /// @endcode
    void _startProcessActionList(StateMachineWorkStatusType const startFrom,
                                 ActionItemList const *const actionItemList,
                                 core::Promise< void > &&promise) noexcept;

    /// @brief Stop the ongoing processing
    /// @param stopFrom State machine working status when finishing processing the action list
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08221
    /// @needwork = dda
    /// @endcode
    void _stopProcessing(StateMachineWorkStatusType const
                             stopFrom) noexcept;  // Because ContinueProcessingActionList is placed in GlobalGeneralEvLoop for processing,
    // So it is possible that it has already been executed and succeeded (but there will be no error), but that's not a big problem.
    // Now when StopProcessing is called, the processing is inevitably unsuccessful, which brings this certainty.

    /// @brief The action list processing ended successfully
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08222
    /// @needwork = dda
    /// @endcode
    void _onProcessSuccess() noexcept;

    /// @brief Get the position of the action item of type type and matching the description actionItemDescriptionStr in the action list of SMState
    /// @param smState State machine state
    /// @param type Action type
    /// @param actionItemDescriptionStr Action description string
    /// @return Position of the action item
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08223
    /// @needwork = dda
    /// @endcode
    uint32_t _getActionItemPos(core::String const &smState,
                               ActionType const type,
                               core::String const &actionItemDescriptionStr) noexcept;

    /// @brief Insert an action item before the index position in the action list of SMState
    /// @param smState State machine state
    /// @param luIndex Index of the action item in the action list
    /// @param actionItem The action item to be inserted
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08224
    /// @needwork = dda
    /// @endcode
    void _insertActionItem(core::String const &smState,
                           ActionItemList::size_type const luIndex,
                           std::shared_ptr< ActionItemBase > const &actionItem) noexcept;

    /// @brief Publish a state machine state change event to EventManager
    /// @param smFQN State machine full name
    /// @param smNotifierState Externally published state machine state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08225
    /// @needwork = dda
    /// @endcode
    void _notifySMStateChange(core::String const &smFQN, core::String const &smNotifierState) const noexcept;

    /// @brief Handle postponed state transition requests
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08226
    /// @needwork = dda
    /// @endcode
    void _handlePendingRequest() noexcept;

    /// @brief Whether it has started
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08227
    /// @needwork = dda
    /// @endcode
    bool started_{false};

    /// @brief State machine FQN
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08228
    /// @needwork = dda
    /// @endcode
    core::String smFQN_;

    /// @brief Set of states
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09004, SR_SM_09006
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08229
    /// @needwork = dda
    /// @endcode
    core::Vector< core::String > states_;

    /// @brief Mapping of state machine states to action lists
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09004, SR_SM_09006
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08230
    /// @needwork = dda
    /// @endcode
    std::map< core::String, ActionItemList > state2ActionListMap_;

    /// @brief List of transition requests
    /// Table which defines next StateMachine State, depending on current StateMachine State and on value passed via
    /// StateMachineService interface. TransitionRequestTable should be 2 maps:
    /// map1, from TransitionRequest (TransitionRequestType) to map2; map2, from Current State to Next State.
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09004, SR_SM_09006
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08231
    /// @needwork = dda
    /// @endcode
    std::map< TransitionRequestType, std::map< core::String, core::String > > transitionRequestTable_;

    std::map< core::String, std::map< core::String, core::String > > extendTransitionRequestTable_;

    /// @brief Error recovery list
    /// Table which defines next StateMachine State, depending on ErrorEvent value passed in StateMachine error
    /// notification. Each ErrorRecoveryTable should be a map from ExecutionError to Next State.
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_06004, SR_SM_08001, SR_SM_09002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08232
    /// @needwork = dda
    /// @endcode
    std::map< ara::exec::ExecutionError, core::String > errorRecoveryTable_;

    /// @brief Default error recovery state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_06004, SR_SM_08001, SR_SM_09002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08233
    /// @needwork = dda
    /// @endcode
    core::String anyErrRecoveryState_{""};

    /// @brief Current state machine state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08234
    /// @needwork = dda
    /// @endcode
    core::String currentState_{common::GetkInitialState()};

    /// @brief Current notification state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08235
    /// @needwork = dda
    /// @endcode
    core::String currentNotifierState_{common::GetkFinalState()};

    /// @brief Whether it is a Controller state machine
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08236
    /// @needwork = dda
    /// @endcode
    bool isController_{false};

    /// @brief Working status
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09004, SR_SM_09006
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08237
    /// @needwork = dda
    /// @endcode
    StateMachineWorkStatusType workStatus_{StateMachineWorkStatusType::kOff};

    /// @brief Processing status
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08238
    /// @needwork = dda
    /// @endcode
    ProcessStatusType processStatus_{ProcessStatusType::kOff};

    /// @brief The last ExecutionError handled during error recovery
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_06004, SR_SM_08001, SR_SM_09002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08239
    /// @needwork = dda
    /// @endcode
    core::Optional< exec::ExecutionError > lasExecutionError_{core::nullopt};

    /// @brief Context of action list processing
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08240
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ProcessActionListContext > processContext_;

    /// @brief Function handle for checking whether shutdown is allowed
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01002, SR_SM_01003, SR_SM_04003, SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08241
    /// @needwork = dda
    /// @endcode
    std::function< core::Result< void >(core::StringView const, FGStateChangeSource const) > checkShutdownHandler_;

    /// @brief Mapping of function group states to state machine states, used for self-built state machines
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08242
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, core::String > fg2Sm_;

    /// @brief Mapping of function group FQN to their current states
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08243
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, core::String > fg2State_;

    /// @brief Mapping of function group FQN to their processing IDs, mainly used for canceling delayed events
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05004, SR_SM_05005
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08244
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, uint32_t > fg2ProcessID_;

    /// @brief Mapping of function group FQN+ProcessID to timers, mainly used for storing timers
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05005
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08245
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, std::shared_ptr< EvNodeTimer > > fgProcessID2Timer_;

    /// @brief Network management delay information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05005
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08246
    /// @needwork = dda
    /// @endcode
    config::NMAfterRunTimeInfo nmAfterRunTime_{kDEFAULT_RUN_TIME_FACTOR, kDEFAULT_RUN_TIME_FACTOR};

    /// @brief Function handle for publishing events to EventManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01003, SR_SM_02002, SR_SM_05004, SR_SM_06001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08247
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_{nullptr};

    /// @brief Function handle for obtaining execution errors
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_06004, SR_SM_08001, SR_SM_09002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08248
    /// @needwork = dda
    /// @endcode
    std::function< core::Result< exec::ExecutionErrorEvent >(core::String const &) > getExecutionErrorHandler_;

    /// @brief Function handle for checking whether a function group is deactivated
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08249
    /// @needwork = dda
    /// @endcode
    std::function< bool(core::String const &) > checkFGDeactivatedHandler_;

    /// @brief Callback function for starting the state machine
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08250
    /// @needwork = no
    /// @endcode
    std::function< core::Future< void >(core::String const &, core::String const &) > startStateMachineHandler_;

    /// @brief Callback function for stopping the state machine
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08251
    /// @needwork = no
    /// @endcode
    std::function< core::Future< void >(core::String const &) > stopStateMachineHandler_;

    std::function< core::String(uint64_t const &) > getNmHandlerNameFun_;

    /// @brief Whether this state machine is generated by user definition
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08252
    /// @needwork = dda
    /// @endcode
    bool isUserDefined_{false};

    /// @brief Mapping of function group states to FullCom network list for the function groups managed by this state machine, core::Map<Function group FQN, core::Map<Function group state, core::Vector<Network>>>
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08253
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, core::Map< core::String, core::Vector< core::String > > > fgState2FullComNmHandles_;

    /// @brief Mapping of Off state to NoCom network list for the function groups managed by this state machine, core::Map<Function group FQN, core::Vector<Network>>
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08254
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, core::Vector< core::String > > offFgState2NoComNmHandles_;

    /// @brief State machine transition requests caused by network state changes need to be queued, and processed after the current state machine state transition is completed.
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05005
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08255
    /// @needwork = dda
    /// @endcode
    std::queue< PendingSMStateRequest > pendingSmStateRequests_;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08256
    /// @needwork = dda
    /// @endcode
    log::Logger &log_{log::CreateLogger((core::StringView{"#SMM"}), (core::StringView{"State Machine Management"}))};

    core::String initialStartState_;
};

}  // namespace state_machine_management
}  // namespace sm
}  // namespace ara

#endif  // STATE_MACHINE_H_