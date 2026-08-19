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
/// @file       em_comm_manager.cpp
/// @brief      Management class for communication with EM, for switching function group states and obtaining execution errors
/// @details
/// @date       2024-05-03
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/EMComm
/// @interface_level=module
/// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
/// @unit_name=EMCommManager
/// @unit_description=Management class for communication with EM, for switching function group states and obtaining execution errors
/// @endcode
///
/// ================================================================

#include "em_comm_manager.h"

#include <ara/exec/exec_error_domain.h>
#include <ara/sm/error_domain_sm.h>

#include "helper.h"
namespace ara {
namespace sm {
namespace em_comm {

/// @brief Constructor function
/// @param name CommManager name
EMCommManager::EMCommManager(core::String const &name) noexcept
    : ICommunicationManager{name}
    , stateClient_{}
    , executionClient_{}
    , appendEventHandler_{}
    , log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"Communication Manager context"}))}
{
    log_.LogInfo() << "EMCommManager::EMCommManager()";
}

/// @brief Destructor function
EMCommManager::~EMCommManager() noexcept { log_.LogInfo() << "EMCommManager::~EMCommManager()"; }

/// @brief Initialize
/// @return true - success
/// @return false - failure
bool EMCommManager::Init() noexcept
{
    log_.LogInfo() << "EMCommManager::Init()";
    if (stateClient_ == nullptr) {
        stateClient_ = std::make_unique< exec::StateClient >(
            [this](ara::exec::ExecutionErrorEvent const &executionErrorEvent) noexcept {
                _deliverEMUndefinedStateRequest(executionErrorEvent);
            });
    }
    if (executionClient_ == nullptr) {
        executionClient_ = std::make_unique< exec::ExecutionClient >();
    }
    return true;
}

/// @brief Start accepting requests
/// @return true - success
/// @return false - failure
bool EMCommManager::Start() noexcept
{
    log_.LogInfo() << "EMCommManager::Start()";
    return true;
}

/// @brief Stop accepting requests
/// @return
void EMCommManager::Stop() noexcept { log_.LogInfo() << "EMCommManager::Stop()"; }

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void EMCommManager::RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept
{
    log_.LogInfo() << "EMCommManager::RegisterAppendEventHandler()";
    appendEventHandler_ = appendEventHandler;
}

/// @brief Asynchronously switch function group state
/// @param fgFQN  Function group name
/// @param fgState Function group state
/// @return Future object
core::Future< void > EMCommManager::AsyncSetFGState(core::String const &fgFQN,
                                                    core::String const &fgState) const noexcept
{
    log_.LogInfo() << "EMCommManager::AsyncSetFGState(), fgFQN:" << fgFQN.c_str() << "fgState:" << fgState.c_str();
    ara::core::Promise< void > promise;
    ara::core::Future< void > future{promise.get_future()};
    if (stateClient_) {
        ara::core::Result< exec::FunctionGroup::CtorToken > const groupTokenResult{
            exec::FunctionGroup::Preconstruct((ara::core::StringView{fgFQN}))};

        // Check if function group is valid
        if (groupTokenResult.HasValue() == false) {
            log_.LogError() << "EMCommManager::AsyncSetFGState(), failed to call Preconstruct for fgFQN:"
                            << fgFQN.c_str();
            promise.SetError(SMErrc::kFGNameIllegal);
            return future;
        }

        exec::FunctionGroup::CtorToken groupToken{groupTokenResult.Value()};
        exec::FunctionGroup const group{std::move(groupToken)};

        ara::core::Result< exec::FunctionGroupState::CtorToken > const stateTokenResult{
            exec::FunctionGroupState::Preconstruct(group, (ara::core::StringView{fgState}))};

        // Check if function group state is valid
        if (stateTokenResult.HasValue() == false) {
            log_.LogError() << "EMCommManager::AsyncSetFGState(), failed to call Preconstruct for fgState:"
                            << fgState.c_str();
            promise.SetError(SMErrc::kFGStateIllegal);
            return future;
        }

        exec::FunctionGroupState::CtorToken stateToken{stateTokenResult.Value()};
        exec::FunctionGroupState const state{std::move(stateToken)};

        return stateClient_->SetState(state);
    }
    log_.LogError() << "EMCommManager::AsyncSetFGState(), stateClient_ is nullptr";
    promise.SetError(SMErrc::kRejected);
    return future;
}

/// @brief Get the execution error related to the specified function group
/// @param fgFQN Function group name
/// @return Set result
core::Result< exec::ExecutionErrorEvent > EMCommManager::GetExecutionError(core::String const &fgFQN) const noexcept
{
    log_.LogInfo() << "EMCommManager::GetExecutionError(), fgFQN:" << fgFQN.c_str();

    ara::core::Result< exec::FunctionGroup::CtorToken > const groupTokenResult{
        exec::FunctionGroup::Preconstruct((ara::core::StringView{fgFQN}))};

    // Check if function group is valid
    if (groupTokenResult.HasValue() == false) {
        log_.LogError() << "EMCommManager::GetExecutionError(), failed to call Preconstruct for fgFQN:"
                        << fgFQN.c_str();
        return ara::core::Result< exec::ExecutionErrorEvent >::FromError(SMErrc::kFGNameIllegal);
    }
    exec::FunctionGroup::CtorToken groupToken{groupTokenResult.Value()};
    exec::FunctionGroup const group{std::move(groupToken)};

    // It is not easy to construct core::Result<exec::ExecutionErrorEvent> here, so we do not check if stateClient_ is empty.
    ara::core::Result< ara::exec::ExecutionErrorEvent > ret{stateClient_->GetExecutionError(group)};
    if (ret) {  // Success
        log_.LogDebug() << "EMCommManager::GetExecutionError(), got ExecutionErrorEvent:"
                        << common::ErrorEventToString(ret.Value()).c_str() << "for fgFQN:" << fgFQN.c_str();
    } else {
        log_.LogError() << "EMCommManager::GetExecutionError(), failed with error:" << ret.Error().Message().data()
                        << "for fgFQN:" << fgFQN.c_str();
    }
    return ret;
}

/// @brief Get the initial machine state transition result
/// @return Future object
core::Future< void > EMCommManager::GetInitialMachineStateTransitionResult() const noexcept
{
    log_.LogInfo() << "EMCommManager::GetInitialMachineStateTransitionResult()";
    if (stateClient_) {
        return stateClient_->GetInitialMachineStateTransitionResult();
    }
    ara::core::Promise< void > promise;
    ara::core::Future< void > future{promise.get_future()};
    log_.LogError() << "EMCommManager::GetInitialMachineStateTransitionResult(), stateClient_ is nullptr";
    promise.SetError(SMErrc::kRejected);
    return future;
}

/// @brief Report the execution state of the process
/// @param state Execution state enumeration value of the process
/// @return Status result
core::Result< void > EMCommManager::ReportExecutionState(exec::ExecutionState const state) const noexcept
{
    if (executionClient_) {
        return executionClient_->ReportExecutionState(state);
    }
    log_.LogError() << "EMCommManager::ReportExecutionState(), executionClient_ is nullptr";
    return ara::core::Result< void >::FromError(SMErrc::kRejected);
}

/// @brief Forward EMUndefinedState request
/// @param executionErrorEvent  Execution error event information for entering the undefined state
void EMCommManager::_deliverEMUndefinedStateRequest(exec::ExecutionErrorEvent const &executionErrorEvent) const noexcept
{
    log_.LogInfo() << "EMCommManager::_deliverEMUndefinedStateRequest(), executionErrorEvent:"
                   << common::ErrorEventToString(executionErrorEvent).data();

    ara::core::Promise< void > promise;
    if (appendEventHandler_) {
        common::Event request;
        request.type           = common::EventType::kInEMUndefinedStateCallback;
        request.data           = new exec::ExecutionErrorEvent(executionErrorEvent);
        request.requestPromise = std::move(promise);
        appendEventHandler_(std::move(request));
    } else {
        log_.LogError() << "EMCommManager::_deliverEMUndefinedStateRequest(), appendEventHandler_ is nullptr";
    }
}

}  // namespace em_comm
}  // namespace sm
}  // namespace ara
