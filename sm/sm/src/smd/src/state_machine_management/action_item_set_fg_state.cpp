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
/// @file       action_item_set_fg_state.cpp
/// @brief      Define the data type for setting function group state action
/// @details
/// @date       2024-06-26
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/StateMachineManagement
/// @interface_level=uint
/// @trace_id_sr=SR_SM_09001, SR_SM_09002, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008
/// @unit_name=ActionItemSetFGState
/// @unit_description=Define the data type for setting function group state action
/// @endcode
///
/// ================================================================

#include "state_machine_management/action_item_set_fg_state.h"

#include <ara/exec/exec_error_domain.h>
#include <ara/sm/error_domain_sm.h>

#include <memory>

#include "helper.h"

namespace ara {
namespace sm {
namespace state_machine_management {

using sm::SMErrc;

/// @brief Default constructor
/// @param fgFQN   Function group full name
/// @param fgState Function group state
ActionItemSetFGState::ActionItemSetFGState(core::String fgFQN, core::String fgState) noexcept
    : ActionItemBase{ActionType::kSetFunctionGroupState}
    , fgFQN_{std::move(fgFQN)}
    , fgState_{std::move(fgState)}
    , processContext_{}
{
    GetLog().LogInfo() << "ActionItemSetFGState::ActionItemSetFGState, fgFQN_:" << fgFQN_.c_str()
                       << "fgState_:" << fgState_.c_str();
}

/// @brief Start executing the action
/// @param processID The ID when this action starts execution (also known as processing sequence number)
void ActionItemSetFGState::StartExecute(uint32_t const processID) noexcept
{
    uint32_t const sleepTimeMs{GetSleepTime()};
    GetLog().LogInfo() << "ActionItemSetFGState::StartExecute, fgFQN_:" << fgFQN_.c_str()
                       << "fgState_:" << fgState_.c_str() << "sleepTimeMs_:" << sleepTimeMs
                       << "processID:" << processID;
    assert(changeFgStateHandler_);
    core::Promise< void > promise;
    _SetExcuteFuture(promise.get_future());
    if (0U < sleepTimeMs) {
        isoft::naicpp::EvLoop::TimerPtr pExecTimer{GetExecTimer()};
        std::ignore = isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
            pExecTimer, static_cast< int32_t >(sleepTimeMs),
            [this, innerPromise = std::make_shared< ara::core::Promise< void > >(std::move(promise))]() {
                changeFgStateHandler_(fgFQN_, fgState_, std::move(*innerPromise));
            });
        SetExecTimer(pExecTimer);
    } else {
        changeFgStateHandler_(fgFQN_, fgState_, std::move(promise));
    }

    std::ignore = _GetExcuteFuture().then([this, processID]() {
        assert(processContext_);
        if (processID != processContext_->processID) {  // Ignore callbacks not for the current processing list
            GetLog().LogDebug()
                << "ActionItemSetFGState::futureReadyContinuation(), return immediately because processID is"
                << processID << "but processContext_.processID is" << processContext_->processID
                << "fgFQN_:" << fgFQN_.c_str();
            return;
        }
        OnActionFutureReady(processID);
    });
}

/// @brief Handling after action execution succeeds
void ActionItemSetFGState::OnSuccess() noexcept
{
    GetLog().LogInfo() << "ActionItemSetFGState::OnSuccess(), fgFQN_:" << fgFQN_.c_str()
                       << "fgState_:" << fgState_.c_str();
    if (postExecuteHandlerWithStrParas_) {
        postExecuteHandlerWithStrParas_(fgFQN_, fgState_);
    }
}

/// @brief Handling after action execution fails
void ActionItemSetFGState::OnFailure() noexcept
{
    GetLog().LogInfo() << "ActionItemSetFGState::OnFailure(), fgFQN_:" << fgFQN_.c_str()
                       << "fgState_:" << fgState_.c_str();
}

/// @brief Callback when the Future corresponding to Action is ready
/// @param processID Processing sequence number
void ActionItemSetFGState::OnActionFutureReady(uint32_t const processID) noexcept
{
    GetLog().LogInfo() << "ActionItemSetFGState::OnActionFutureReady(), processID:" << processID
                       << "fgFQN_:" << fgFQN_.c_str();

    assert(_GetExcuteFuture().is_ready());
    assert(processFailureHandler_);
    assert(continueProcessingActionListHandler_);

    // Handle accordingly based on the fut result
    ara::core::Result< void, ara::core::ErrorCode > const res{_GetExcuteFuture().GetResult()};
    bool const bResCheckErrorkAlreadyInState{res.CheckError(exec::ExecErrc::kAlreadyInState)};
    if (res || bResCheckErrorkAlreadyInState) {  // Success
        GetLog().LogDebug() << "ActionItemSetFGState::OnActionFutureReady(), succeed to execute:"
                            << ActionItemToString().data() << "fgFQN_:" << fgFQN_.c_str();
    } else {
        GetLog().LogDebug() << "ActionItemSetFGState::OnActionFutureReady(), failed with error:"
                            << res.Error().Message().data() << "to execute:" << ActionItemToString().c_str()
                            << "fgFQN_:" << fgFQN_.c_str();
    }
    std::shared_ptr< EvNodeTimer > pRetryTimer{GetRetryTimer()};
    // If the Future has a value, i.e., processing succeeded, or the error is that the function group is already in that state (kAlreadyInState)
    bool const bResHasValue{res.HasValue()};
    bool const bResCheckErrorkFailed{res.CheckError(exec::ExecErrc::kFailed)};
    bool const bResCheckErrorkCancelled{res.CheckError(exec::ExecErrc::kCancelled)};
    bool const bResCheckErrorkInTransitionToSameState{res.CheckError(exec::ExecErrc::kInTransitionToSameState)};
    bool const bResCheckErrorkFailedUnexpectedTerminationOnExit{
        res.CheckError(exec::ExecErrc::kFailedUnexpectedTerminationOnExit)};
    bool const bResCheckErrorkFailedUnexpectedTerminationOnEnter{
        res.CheckError(exec::ExecErrc::kFailedUnexpectedTerminationOnEnter)};
    if (bResHasValue || bResCheckErrorkAlreadyInState) {
        OnSuccess();
        processContext_->groupNum--;
        GetLog().LogDebug() << "ActionItemSetFGState::OnActionFutureReady(), groupNum is updated to:"
                            << processContext_->groupNum << "fgFQN_:" << fgFQN_.c_str();
    } else if (bResCheckErrorkCancelled || bResCheckErrorkInTransitionToSameState
               || bResCheckErrorkFailedUnexpectedTerminationOnExit) {
        GetLog().LogDebug()
            << "ActionItemSetFGState::OnActionFutureReady(), try to MakeTimer with kLittleRetryInterval:"
            << kLittleRetryInterval << "fgFQN_:" << fgFQN_.c_str();

        std::ignore = isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
            pRetryTimer, kLittleRetryInterval, [this, processID]() noexcept {
                GetLog().LogDebug() << "ActionItemSetFGState::OnActionFutureReady(), OnRetryActionTimeout, fgFQN_:"
                                    << fgFQN_.c_str();
                StartExecute(processID);
            });
    } else if (bResCheckErrorkFailed || bResCheckErrorkFailedUnexpectedTerminationOnEnter) {
        // If the error is: ara::exec::ExecErrc::kFailed/ara::exec::ExecErrc::kFailedUnexpectedTerminationOnEnter/ara::exec::ExecErrc::kIntegrityOrAuthenticityCheckFailed, then call GetExecutionError,
        assert(getExecutionErrorHandler_);
        GetLog().LogDebug() << "ActionItemSetFGState::OnActionFutureReady(), try to GetExecutionError, fgFQN_:"
                            << fgFQN_.c_str();
        ara::core::Result< ara::exec::ExecutionErrorEvent > const execRes{getExecutionErrorHandler_(fgFQN_)};
        if (execRes.HasValue()
            == false) {  // If it returns error ara::exec::ExecErrc::kFailed/ara::exec::ExecErrc::kCommunicationError (actually any error), then:
            GetLog().LogError()
                << "ActionItemSetFGState::OnActionFutureReady(), failed to GetExecutionError with error:"
                << res.Error().Message().data() << "fgFQN_:" << fgFQN_.c_str();

            // Use EvLoop's MakeTimer interface to add a timer task with retryInterval, and its callback is OnRetryActionTimeout;
            GetLog().LogDebug() << "ActionItemSetFGState::OnActionFutureReady(), try to MakeTimer with kRetryInterval:"
                                << kRetryInterval << "fgFQN_:" << fgFQN_.c_str();
            std::ignore = isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
                pRetryTimer, kRetryInterval, [this, processID]() noexcept {
                    GetLog().LogDebug() << "ActionItemSetFGState::OnActionFutureReady(), OnRetryActionTimeout, fgFQN_:"
                                        << fgFQN_.c_str();
                    StartExecute(processID);
                });
        } else {  // Otherwise trigger error recovery:
            // Get execution error
            assert(errorRecoveryHandler_);
            exec::ExecutionErrorEvent const &executionErrorEvent{execRes.Value()};
            // Use EvLoop's Exec interface to call OnErrorRecoveryEvent;
            std::shared_ptr< ara::core::Promise< void > > promise{
                std::make_shared< ara::core::Promise< void > >()};  // The result is not cared about here
            GetLog().LogDebug() << "ActionItemSetFGState::OnActionFutureReady(), try to Exec OnErrorRecoveryEvent with "
                                   "executionErrorEvent:"
                                << common::ErrorEventToString(executionErrorEvent).c_str()
                                << "fgFQN_:" << fgFQN_.c_str();
            std::ignore = isoft::naicpp::GlobalGeneralEvLoop::Get()->Exec(
                [this, executionErrorEvent, innerPromise = std::move(promise)]() mutable {
                    errorRecoveryHandler_(executionErrorEvent, std::move(*(innerPromise.get())));
                });

            // Update error flag
            processContext_->hasErrorResult = true;
            GetLog().LogDebug() << "ActionItemSetFGState::OnActionFutureReady(), hasErrorResult is set to:"
                                << processContext_->hasErrorResult << "fgFQN_:" << fgFQN_.c_str();
        }
    } else {  // If the error is: ara::exec::ExecErrc::kCommunicationError/ara::exec::ExecErrc::kInvalidTransition/ara::exec::ExecErrc::kGeneralError/others, then cannot retry
        // Although this state machine cannot run normally, it cannot affect the normal operation of other state machines, and this state machine remains as is. Set Error but do not clear ErrorRecoveryOngoing
        processContext_->hasErrorResult = true;
        GetLog().LogWarn() << "ActionItemSetFGState::OnActionFutureReady(), hasErrorResult is set to:"
                           << processContext_->hasErrorResult << "fgFQN_:" << fgFQN_.c_str();
    }
    SetRetryTimer(pRetryTimer);
    // From the start or the last SYNC, wait until:
    //     All processed ActionListItems are executed successfully, or
    //     There is an error that can trigger error recovery, or
    //     There is a non-retryable error, or
    if (processContext_->hasErrorResult) {          // Whether there is an error
        processFailureHandler_();                   // End
    } else if (0UL == processContext_->groupNum) {  // All processed ActionListItems are executed successfully
        GetLog().LogDebug()
            << "ActionItemSetFGState::OnActionFutureReady(), groupNum is 0, so try to ContinueProcessingActionList "
               "with processContext_.nextGroupIndex:"
            << processContext_->nextGroupIndex << "fgFQN_:" << fgFQN_.c_str();
        continueProcessingActionListHandler_(processContext_->nextGroupIndex);
    } else {
    }
}
}  // namespace state_machine_management
}  // namespace sm
}  // namespace ara
