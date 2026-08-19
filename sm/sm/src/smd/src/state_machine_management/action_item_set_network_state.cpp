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
/// @file       action_item_set_network_state.cpp
/// @brief      Define the class for setting logical network state
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
/// @unit_name=ActionItemSetNetworkState
/// @unit_description=Define the class for setting logical network state
/// @endcode
///
/// ================================================================

#include "state_machine_management/action_item_set_network_state.h"

#include <ara/sm/error_domain_sm.h>

#include <memory>

#include "helper.h"

namespace ara {
namespace sm {
namespace state_machine_management {

using ara::sm::SMErrc;

/// @brief Default constructor
/// @param networkHandle Network state handle
/// @param networkState Network state internal type
ActionItemSetNetworkState::ActionItemSetNetworkState(core::String networkHandle,
                                                     common::NetworkStateInternalType networkState) noexcept
    : ActionItemBase{ActionType::kSetNetworkState}
    , networkHandle_{std::move(networkHandle)}
    , networkState_{std::move(networkState)}
    , processContext_{}
{
    GetLog().LogInfo() << "ActionItemSetNetworkState::ActionItemSetNetworkState(), networkHandle_:"
                       << networkHandle_.c_str()
                       << "networkState_:" << common::NetworkStateTypeToString(networkState_).c_str();
}

/// @brief Start executing the action
/// @param processID Processing sequence number
void ActionItemSetNetworkState::StartExecute(uint32_t const processID) noexcept
{
    uint32_t const sleepTimeMs{GetSleepTime()};
    GetLog().LogInfo() << "ActionItemSetNetworkState::StartExecute(), networkHandle_:" << networkHandle_.c_str()
                       << "networkState_:" << common::NetworkStateTypeToString(networkState_).c_str()
                       << "sleepTimeMs_:" << sleepTimeMs << "nmHandleId_:" << nmHandleId_;
    assert(changeNetworkStateHandler_);
    core::Promise< void > promise;
    _SetExcuteFuture(promise.get_future());
    if (0U < sleepTimeMs) {
        std::shared_ptr< EvNodeTimer > execTimer{GetExecTimer()};
        std::ignore = isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
            execTimer, static_cast< int32_t >(sleepTimeMs),
            [this, innerPromise = std::make_shared< ara::core::Promise< void > >(std::move(promise))]() {
                GetLog().LogDebug() << "ActionItemSetNetworkState::StartExecute(), networkHandle_:"
                                    << networkHandle_.c_str()
                                    << "networkState_:" << common::NetworkStateTypeToString(networkState_).c_str()
                                    << "changeNetworkStateHandler_ is really executed";
                changeNetworkStateHandler_(networkHandle_, networkState_, std::move(*innerPromise), nmHandleId_);
            });
        SetExecTimer(execTimer);
    } else {
        changeNetworkStateHandler_(networkHandle_, networkState_, std::move(promise), nmHandleId_);
    }
    std::ignore = _GetExcuteFuture().then([this, processID]() {
        assert(processContext_);
        if (processID != processContext_->processID) {  // Ignore callbacks not for the current processing list
            GetLog().LogDebug()
                << "ActionItemSetFGState::futureReadyContinuation(), return immediately because processID is"
                << processID << "but processContext_.processID is" << processContext_->processID
                << "networkHandle_:" << networkHandle_.c_str();
            return;
        }
        OnActionFutureReady(processID);
    });
}

/// @brief Handling after action execution succeeds
void ActionItemSetNetworkState::OnSuccess() noexcept
{
    GetLog().LogInfo() << "ActionItemSetNetworkState::OnSuccess(), networkHandle_:" << networkHandle_.c_str()
                       << "networkState_:" << common::NetworkStateTypeToString(networkState_).c_str();
}

/// @brief Handling after action execution fails
void ActionItemSetNetworkState::OnFailure() noexcept
{
    GetLog().LogInfo() << "ActionItemSetNetworkState::OnFailure(), networkHandle_:" << networkHandle_.c_str()
                       << "networkState_:" << common::NetworkStateTypeToString(networkState_).c_str();
}

/// @brief Callback when the Future corresponding to Action is ready
/// @param processID Processing sequence number
void ActionItemSetNetworkState::OnActionFutureReady(uint32_t const processID) noexcept
{
    GetLog().LogInfo() << "ActionItemSetNetworkState::OnActionFutureReady(), networkHandle_:" << networkHandle_.c_str()
                       << "networkState_:" << common::NetworkStateTypeToString(networkState_).c_str();
    std::ignore = processID;

    assert(_GetExcuteFuture().is_ready());
    assert(processFailureHandler_);
    assert(continueProcessingActionListHandler_);

    // Handle accordingly based on the fut result
    ara::core::Result< void, ara::core::ErrorCode > const res{_GetExcuteFuture().GetResult()};
    if (res) {  // Success
        GetLog().LogDebug() << "ActionItemSetNetworkState::OnActionFutureReady(), succeed to execute:"
                            << ActionItemToString().c_str();
        OnSuccess();
        processContext_->groupNum--;
        GetLog().LogDebug() << "ActionItemSetNetworkState::OnActionFutureReady(), groupNum is updated to:"
                            << processContext_->groupNum << "networkHandle_:" << networkHandle_.c_str();
    } else {
        processContext_->hasErrorResult = true;
        GetLog().LogDebug() << "ActionItemSetNetworkState::OnActionFutureReady(), failed with error:"
                            << res.Error().Message().data() << "to execute:" << ActionItemToString().c_str()
                            << "hasErrorResult is set to:" << processContext_->hasErrorResult;
    }
    // From the start or the last SYNC, wait until:
    //     All processed ActionListItems are executed successfully, or
    //     There is an error that can trigger error recovery, or
    //     There is a non-retryable error, or
    if (processContext_->hasErrorResult) {          // Whether there is an error
        processFailureHandler_();                   // End
    } else if (0UL == processContext_->groupNum) {  // All processed ActionListItems are executed successfully
        GetLog().LogDebug() << "ActionItemSetNetworkState::OnActionFutureReady(), groupNum is 0, so try to "
                               "ContinueProcessingActionList with processContext_.nextGroupIndex:"
                            << processContext_->nextGroupIndex << "networkHandle_:" << networkHandle_.c_str();
        continueProcessingActionListHandler_(processContext_->nextGroupIndex);
    } else {
    }
}

}  // namespace state_machine_management
}  // namespace sm
}  // namespace ara
