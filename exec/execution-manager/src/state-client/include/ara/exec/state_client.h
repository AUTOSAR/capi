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
/// @file       state_client.h
/// @brief      State client
/// @details
/// @date       2022-01-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/StateClient
/// @unit_name=StateClient
/// @unit_description=Used to manage the state of function groups in the execution management system.
/// @interface_level=software
/// @endcode
///
/// ================================================================

#ifndef ARA_EXEC_STATE_CLIENT_H_
#define ARA_EXEC_STATE_CLIENT_H_

#include <ara/core/future.h>
#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <functional>
#include <memory>

#include "ara/exec/execution_error_event.h"
#include "ara/exec/function_group.h"
#include "ara/exec/function_group_state.h"

namespace ara {
namespace exec {

/// @brief Class implements State Client.
/// Class representing connection to Execution Management that is used
/// to request Function Group state transitions (or other operations).
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_02275, 772d4909ec06eb7d91877cda9ad93cd95477a2b2
/// @trace_id_sr=SR_EM_10006
/// @trace_id_ad=AD_EM_00035
/// @trace_id_dd=DD_EM_00760
/// @needwork = ad
/// @endcode
class StateClient
{
public:
#ifdef R20_11
    /// @brief Constructor that creates State Client instance.
 /// @param undefinedStateCallback Abnormal callback function, called when the function group falls into an undefined state.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_02276
    /// @endcode
    explicit StateClient(std::function< void(ara::exec::FunctionGroup&) > undefinedStateCallback) noexcept;
#endif
    /// @brief Constructor in version 2211
 /// @param undefinedStateCallback Abnormal callback function, called when the function group falls into an undefined state.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00035
    /// @trace_id_dd=DD_EM_00761
    /// @needwork = dda
    /// @endcode
    explicit StateClient(std::function< void(ara::exec::ExecutionErrorEvent const&) > undefinedStateCallback) noexcept;

    /// @brief Destructor of the State Client instance.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_02277, a8b3ec5e8efde18905a4efa25c74d35991aa66b5
    /// @trace_id_ad=AD_EM_00035
    /// @trace_id_dd=DD_EM_00762
    /// @needwork = dda
    /// @endcode
    ~StateClient() noexcept;

 /// @brief Prohibit use of copy constructor
 /// @param other Other state client
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00035
    /// @trace_id_dd=DD_EM_00763
    /// @needwork = dda
    /// @endcode
    StateClient(StateClient const& other) = delete;

 /// @brief Prohibit use of move constructor
 /// @param other Other state client
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00035
    /// @trace_id_dd=DD_EM_00764
    /// @needwork = dda
    /// @endcode
    StateClient(StateClient&& other) = delete;

 /// @brief Prohibit use of copy assignment
 /// @param other Other state client
 /// @return New state client instance
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00035
    /// @trace_id_dd=DD_EM_00765
    /// @needwork = dda
    /// @endcode
    StateClient& operator=(StateClient const& other) = delete;

 /// @brief Prohibit use of move assignment
 /// @param other Other state client
 /// @return New state client instance
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00035
    /// @trace_id_dd=DD_EM_00766
    /// @needwork = dda
    /// @endcode
    StateClient& operator=(StateClient&& other) = delete;

    /// @brief Request state transition for a single Function Group.
    ///        This method will request Execution Management to perform state transition and return
    ///        immediately. Returned ara::core::Future can be used to determine result of requested transition.
    /// @param fgState representing meta-model definition of a state inside a specific
    ///              Ref{Function Group}. Execution Management will perform state transition
    ///              from the current state to the state identified by this parameter.
    /// @return void if requested transition is successful, otherwise it returns ExecErrorDomain error.
    ///         ara::exec::ExecErrc::kCancelled
    ///             if transition to the requested Function Group statewas cancelled by a newer requestar
    ///         ara::exec::ExecErrc::kFailed
    ///             if transition to the requested Function Group state failed.
    ///         ara::exec::ExecErrc::kFailedUnexpectedTerminationOnExit
    ///             if Unexpected Termination in Process of previousFunction Group State happened.
    ///         ara::exec::ExecErrc::kFailedUnexpectedTerminationOnEnter
    ///             if Unexpected Termination in Process of targetFunction Group State happened.
    ///         ara::exec::ExecErrc::kInvalidArguments
    ///             if arguments passed doesn’t appear to be valid (e.g.after a software update,
    ///             given functionGroup doesn’texist anymore).
    ///         ara::exec::ExecErrc::kCommunicationError
    ///             if StateClient can’t communicate with ExecutionManagement (e.g. IPC link is down).
    ///         ara::exec::ExecErrc::kAlreadyInState
    ///             if the FunctionGroup is already in the requestedstate.
    ///         ara::exec::ExecErrc::kInTransitionToSameState
    ///             if a transition to the requested state is already ongoing.
    ///         ara::exec::ExecErrc::kGeneralError
    ///             if any other error occurs.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_02278
    /// @trace_id_ad=AD_EM_00035
    /// @trace_id_dd=DD_EM_00767
    /// @needwork = dda
    /// @endcode
    ara::core::Future< void > SetState(FunctionGroupState const& fgState) const noexcept;  // PRQA S 2024

    /// @brief Method to retrieve result of Machine State initial transition to Startup state.
    /// @return void if requested transition is successful, otherwise it returns ExecErrorDomain error.
    ///         ExecErrc::kCancelled
    ///             if transition to the requested Function Group state was cancelled by a newer request.
    ///         ExecErrc::kFailed if transition to the requested Function Group state failed.
    ///         ExecErrc::kCommunicationError
    ///             if StateClient can't communicate with Execution Management (e.g. IPC link is down).
    ///         ExecErrc::kGeneralError if any other error occurs.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_02279
    /// @trace_id_ad=AD_EM_00035
    /// @trace_id_dd=DD_EM_00768
    /// @needwork = dda
    /// @endcode
    ara::core::Future< void > GetInitialMachineStateTransitionResult() const noexcept;

    /// @brief Returns the execution error whiech changed the given Function Group to an Undefined Function Group State.
    ///        This function will return whith error and will not return an ExecutionErrorEvent object, if the given
    ///        Function Group is in a defined FunctionGroup state again.
    /// @param functionGroup Function Group of interest.
    /// @return ExecErrc::kFailed Given Function Group is not in an Undefined Functoin Group State.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_02542
    /// @trace_id_ad=AD_EM_00035
    /// @trace_id_dd=DD_EM_00769
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::exec::ExecutionErrorEvent > GetExecutionError(  // PRQA S 2024
        ara::exec::FunctionGroup const& functionGroup) const noexcept;

private:
 /// @brief Private class
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    class PrivateImpl;

 /// @brief Private implementation
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00035
    /// @trace_id_dd=DD_EM_00770
    /// @needwork = dda
    /// @endcode
 /// NOTE: Originally should be defined as unique_ptr, but low-version compilers have imperfect support for forward declarations causing compilation errors, so changed to shared_ptr.
    std::shared_ptr< PrivateImpl > privateImpl_;
};

}  // namespace exec
}  // namespace ara

#endif  ///< ARA_EXEC_STATE_CLIENT_H_
