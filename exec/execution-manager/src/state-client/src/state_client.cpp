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
/// @file       state_client.cpp
/// @brief      Function group class implementation
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

#include "ara/exec/state_client.h"

#include <ara/core/future.h>
#include <ara/core/promise.h>
#include <ara/core/string.h>
#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/utils/string.h>

#include <memory>

#include "ara/exec/exec_error_domain.h"
#include "ara/exec/execution_error_event.h"
#include "ara/exec/internal/config/execution_manifest.h"
#include "ara/exec/internal/config/function_group_set.h"
#include "ara/exec/internal/config/software_cluster_manifest.h"
#include "ara/exec/internal/config/sysconfig.h"
#include "ara/exec/internal/log/log.h"
#include "ara/exec/internal/sms/client.h"

#ifdef LOG
    #undef LOG
#endif

/// @brief State client log macro definition
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define LOG() ara::exec::internal::log1::Log< ara::exec::StateClient::PrivateImpl >()

namespace ara {
namespace exec {

namespace {

/// @brief Global static function group set, ensures static initialization
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
ara::core::Vector< internal::config::FunctionGroup > g_LoadedFunctionGroups;  // NOLINT

/// @brief Load platform function group set
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void LoadPlatformFunctionGroups() noexcept
{
    isoft::ara_fsh::Platform pf;
    ara::core::String const swclPath{pf.GetPlatformSwclsManifest()};
    std::shared_ptr< internal::config::SoftwareClusterManifest > const spSwclManifest{
        internal::config::SoftwareClusterManifest::CreateInstance(swclPath)};
    if (spSwclManifest == nullptr) {
        return;
    }

    for (auto const &swcl : spSwclManifest->GetSoftwareClusters()) {  // PRQA S 2961
        if (swcl.HasConfigError()) {
            continue;
        }

        ara::core::String const fgsPath{pf.GetSwclFuncGrpSet(swcl.GetName().c_str(), swcl.GetVersion().c_str())};
        if (0 != access(fgsPath.c_str(), F_OK | R_OK)) {
            continue;
        }

        std::shared_ptr< internal::config::FunctionGroupSet > const fgSet{
            internal::config::FunctionGroupSet::CreateInstance(fgsPath)};
        if (fgSet == nullptr) {
            continue;
        }

        for (auto const &fg : fgSet->GetFunctionGroups()) {  // PRQA S 2961
            if (fg.HasConfigError()) {
                continue;
            }

            g_LoadedFunctionGroups.emplace_back(fg);
        }
    }
}

}  // namespace

/// @brief State client private class implementation
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
class StateClient::PrivateImpl
{
public:
 /// @brief char type redefinition
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using Char8_t = char;

 /// @brief Get log context ID
 /// @return Log context ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetLogCtxId() noexcept { return "SCLT"; }

 /// @brief Get log context description
 /// @return Log context description
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetLogCtxDesc() noexcept { return "The Client of State Management Service"; }

 /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    PrivateImpl() noexcept;

 /// @brief Is valid
 /// @return Valid value
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    bool IsValid() const noexcept { return isValid_; }

 /// @brief Open client
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    int32_t Open() noexcept { return smsClient_.Open(); }

 /// @brief Close client
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    int32_t Close() noexcept { return smsClient_.Close(); }

 /// @brief Register undefined state notification
 /// @param cb Callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void OnUndefineStateNotify(internal::sms::Client::ResponseHandler const &cb) noexcept
    {
        return smsClient_.OnUndefineStateNotify(cb);
    }

 /// @brief Find execution error event
 /// @param fgName Function group name
 /// @return nullptr not found; !nullptr event object pointer
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ara::exec::ExecutionErrorEvent const *FindExecutionErrorEvent(ara::core::StringView const &fgName) const noexcept
    {
        for (ara::exec::ExecutionErrorEvent const &ee : errorEvents_) {  // PRQA S 2961
            if (ee.functionGroup.GetMetaModelIdentifier() == fgName) {
                return &ee;
            }
        }
        return nullptr;
    }

 /// @brief Add execution error event
 /// @param ee Execution error event object
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void AddExecutionErrorEvent(ara::exec::ExecutionErrorEvent &&ee) noexcept
    {
        ara::core::Vector< ara::exec::ExecutionErrorEvent >::iterator it{errorEvents_.begin()};
        while (it != errorEvents_.end()) {
            if (it->functionGroup == ee.functionGroup) {
                it->executionError = ee.executionError;
                return;
            }
            it++;
        }
        errorEvents_.emplace_back(std::move(ee));
    }

 /// @brief Add execution error event
 /// @param ee Execution error event object
    void AddExecutionErrorEvent(ara::exec::ExecutionErrorEvent const &ee) noexcept
    {
        AddExecutionErrorEvent(ara::exec::ExecutionErrorEvent{ee});
    }

 /// @brief Delete all error events under the specified function group
 /// @param fgName Function group name
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void DelExecutionErrorEvent(ara::core::StringView const &fgName) noexcept
    {
        ara::core::Vector< ara::exec::ExecutionErrorEvent >::const_iterator it{errorEvents_.cbegin()};
        while (it != errorEvents_.end()) {
            if (it->functionGroup.GetMetaModelIdentifier() == fgName) {
                break;
            }
            it++;
        }
        if (it != errorEvents_.end()) {
            std::ignore = errorEvents_.erase(it);
        }
    }

 /// @brief Get machine function group startup state
 /// @param pri Class pointer
 /// @param cb Callback function
 /// @return Execution error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static ara::exec::ExecErrc DoGetInitialMachineStateTransitionResult(
        StateClient::PrivateImpl *const pri, internal::sms::Client::ResponseHandler const &cb) noexcept;

 /// @brief Set function group state
 /// @param fgFqn Function group FQN
 /// @param state Function group state
 /// @param pri Object pointer
 /// @param cb Callback function
 /// @return Execution error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static ara::exec::ExecErrc DoSetState(ara::core::String const &fgFqn,
                                          ara::core::String const &state,
                                          StateClient::PrivateImpl *const pri,
                                          internal::sms::Client::ResponseHandler const &cb) noexcept;

private:
 /// @brief Mark whether the current state client is valid
    bool isValid_{false};

 /// @brief State client communication protocol Client object
    internal::sms::Client smsClient_{};

 /// @brief Save problematic function groups so that users can obtain information via GetExecutionError()
    ara::core::Vector< ara::exec::ExecutionErrorEvent > errorEvents_{};
};

/// @brief Constructor
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
StateClient::PrivateImpl::PrivateImpl() noexcept
{
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_02297
    /// @endcode
 /// If functionClusterAffiliation is not configured as STATE_MANAGEMENT in the execution manifest, any call should return kCommunicationError error
    isoft::ara_fsh::Process const fsh;
    ara::core::String const execManifestPath{fsh.GetExecutionManifest()};
    if (true == execManifestPath.empty()) {
        isValid_ = false;
        return;
    }

    internal::config::ExecutionManifest execManifest;
    if (0 != execManifest.Load(execManifestPath)) {
        isValid_ = false;
        return;
    }

    if (true != execManifest.IsStateManager()) {
        isValid_ = false;
        return;
    }

    isValid_ = true;
}

/// @brief Get machine function group startup state
/// @param pri Class pointer
/// @param cb Callback function
/// @return Execution error code
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
ara::exec::ExecErrc StateClient::PrivateImpl::DoGetInitialMachineStateTransitionResult(
    StateClient::PrivateImpl *const pri, ara::exec::internal::sms::Client::ResponseHandler const &cb) noexcept
{
    if (nullptr == pri) {
        return ara::exec::ExecErrc::kGeneralError;
    }

    ara::core::String machineFg;
    for (auto &fg : g_LoadedFunctionGroups) {
        std::string const fgShortName{isoft::utils::GetShortName(fg.GetName().c_str())};
        if (fgShortName.compare(internal::config::GetMachineFunctionGroupName()) == 0) {
            machineFg = fg.GetName();
            break;
        }
    }

    int32_t const r{pri->smsClient_.GetFunctionGroupState(machineFg, cb)};
    LOGD() << "smsClient_.GetFunctionGroupState(" << machineFg << ")";
    if (r != 0) {
        return ara::exec::ExecErrc::kGeneralError;
    }

    return ara::exec::ExecErrc::kNoError;
}

/// @brief Set function group state
/// @param fgFqn Function group FQN
/// @param state Function group state
/// @param pri Object pointer
/// @param cb Callback function
/// @return Execution error code
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
ara::exec::ExecErrc StateClient::PrivateImpl::DoSetState(ara::core::String const &fgFqn,
                                                         ara::core::String const &state,
                                                         StateClient::PrivateImpl *const pri,
                                                         internal::sms::Client::ResponseHandler const &cb) noexcept
{
    if (nullptr == pri) {
        LOGE() << "StateClient::PrivateImpl::DoSetState(): nullptr == pri";
        return ara::exec::ExecErrc::kGeneralError;
    }

    ara::core::Result< FunctionGroup::CtorToken > fgToken{FunctionGroup::Preconstruct(fgFqn)};
    if (!fgToken.HasValue()) {
        return ara::exec::ExecErrc::kInvalidArguments;
    }

    ara::core::Result< FunctionGroupState::CtorToken > const fgStateToken{
        FunctionGroupState::Preconstruct(FunctionGroup(std::move(fgToken).Value()), state)};
    if (!fgStateToken.HasValue()) {
        return ara::exec::ExecErrc::kInvalidArguments;
    }

 /// Actually passed to execution manager is the function group long path + function group state short name
    ara::core::String stateName;
    std::size_t const pos{state.find_last_of("/")};
    if (pos == ara::core::String::npos) {
        stateName = state;
    } else {
        stateName = state.substr(pos + 1U);
    }

    LOGD() << "smsClient_.SetFunctionGroupState(" << fgFqn << "," << stateName << ")";
    int32_t const r{pri->smsClient_.SetFunctionGroupState(fgFqn, stateName, cb)};
    if (r != 0) {
        LOGE() << "smsClient_.SetFunctionGroupState(" << fgFqn << "," << stateName << "): kGeneralError: " << r;
        return ara::exec::ExecErrc::kGeneralError;
    }
    LOGV() << "smsClient_.SetFunctionGroupState(" << fgFqn << "," << stateName << "): kNoError: " << r;
    return ara::exec::ExecErrc::kNoError;
}

#ifdef R20_11
/// @brief Constructor that creates State Client instance.
/// @param undefinedStateCallback Abnormal callback function, called when the function group falls into an undefined state.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_02276
/// @endcode
StateClient::StateClient(std::function< void(ara::exec::FunctionGroup &) > undefinedStateCallback) noexcept
    : privateImpl_{std::make_shared< PrivateImpl >()}
{
    if (nullptr == privateImpl_) {
        return;
    }

 /// If identity verification fails, exit directly
    if (true != privateImpl_->IsValid()) {
        LOGE() << "you are configured NOT a STATE_MANAGER process, so you can`t using StateClient.";
        return;
    }

 /// Open communication channel
    if (0 != privateImpl_->Open()) {
        LOGE() << "privateImpl_->Open() failed !!!";
        return;
    }

    std::function< void(ara::exec::FunctionGroup &) > undefinedStateCb{std::move(undefinedStateCallback)};
    privateImpl_->OnUndefineStateNotify(
        [this, undefinedStateCb](ara::core::StringView const &fgName, ara::core::StringView const &,
                                 ara::exec::ExecErrc const, uint32_t const execErrorCode) {
            FunctionGroup fg{FunctionGroup::CtorToken(fgName)};

 /// Add to error list
            this->privateImpl_->AddExecutionErrorEvent(ara::exec::ExecutionErrorEvent{execErrorCode, fg});

 /// Call callback function
            if (nullptr != undefinedStateCb) {
                undefinedStateCb(fg);
            } else {
                LOGE() << "StateClient::StateClient(): nullptr == undefinedStateCallback";
            }
            return;
        });

    LoadPlatformFunctionGroups();

    return;
}
#endif

/// @brief Constructor in version 2211
/// @param undefinedStateCallback Abnormal callback function, called when the function group falls into an undefined state.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00035
/// @trace_id_dd=DD_EM_00761
/// @needwork = dda
/// @endcode
StateClient::StateClient(std::function< void(ara::exec::ExecutionErrorEvent const &) > undefinedStateCallback) noexcept
    : privateImpl_{std::make_shared< PrivateImpl >()}
{
    std::function< void(ara::exec::ExecutionErrorEvent const &) > undefinedStateCb{std::move(undefinedStateCallback)};
    if (undefinedStateCb == nullptr) {
        LOGE() << "StateClient::StateClient(): undefinedStateCallback is empty";
        return;
    }

    if (nullptr == privateImpl_) {
        return;
    }

 /// If identity verification fails, exit directly
    if (!privateImpl_->IsValid()) {
        LOGE() << "you are configured NOT a STATE_MANAGER process, so you can`t using StateClient.";
        return;
    }

 /// Open communication channel
    if (0 != privateImpl_->Open()) {
        LOGE() << "privateImpl_->Open() failed !!!";
        return;
    }

    privateImpl_->OnUndefineStateNotify(
        [this, undefinedStateCb](ara::core::StringView const &fgName, ara::core::StringView const &,
                                 ara::exec::ExecErrc const, uint32_t const execErrorCode) {
            FunctionGroup const fg{FunctionGroup::CtorToken(fgName)};

 /// Add to error list
            ara::exec::ExecutionErrorEvent const ee{execErrorCode, fg};
            this->privateImpl_->AddExecutionErrorEvent(ee);
            undefinedStateCb(ee);

            return;
        });

    LoadPlatformFunctionGroups();

    return;
}

/// @brief Destructor of the State Client instance.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_02277, a8b3ec5e8efde18905a4efa25c74d35991aa66b5
/// @trace_id_ad=AD_EM_00035
/// @trace_id_dd=DD_EM_00762
/// @needwork = dda
/// @endcode
StateClient::~StateClient() noexcept
{
    if (nullptr == privateImpl_) {
        return;
    }

    if (true != privateImpl_->IsValid()) {
        return;
    }

    if (0 != privateImpl_->Close()) {
        LOGE() << "privateImpl_->Close() failed !!!";
        return;
    }

    return;
}

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
ara::core::Future< void > StateClient::SetState(FunctionGroupState const &fgState) const noexcept
{
    std::shared_ptr< ara::core::Promise< void > > promise{std::make_shared< ara::core::Promise< void > >()};
    ara::core::Future< void > future{promise->get_future()};

    if (nullptr == privateImpl_) {
        LOGE() << "SetState(): nullptr == privateImpl_";
        promise->SetError(ara::exec::ExecErrc::kGeneralError);
        return future;
    }

    if (true != privateImpl_->IsValid()) {
        LOGE() << "false == privateImpl_->IsValid()";
        promise->SetError(ara::exec::ExecErrc::kCommunicationError);
        return future;
    }

    ara::core::String const fg{fgState.GetGroupName()};
    ara::core::String const state{fgState.GetStateName()};

    PrivateImpl *const p{privateImpl_.get()};
    ara::exec::ExecErrc const ee{PrivateImpl::DoSetState(
        fg, state, p,
        [promise, pri = privateImpl_.get(), state](
            ara::core::StringView const &fgName, ara::core::StringView const &stateName,
            ara::exec::ExecErrc const errorCode, uint32_t const execErrorCode) -> void {
            if ((ara::exec::ExecErrc::kNoError == errorCode) || (ara::exec::ExecErrc::kAlreadyInState == errorCode)) {
 /// Function group switch successful, remove from error list
                pri->DelExecutionErrorEvent(fgName);

                if (ara::exec::ExecErrc::kNoError == errorCode) {  // PRQA S 2992
                    LOGV() << "DoSetState(): Transform {" << fgName << "} to {" << stateName
                           << "} successfully !";  // PRQA S 2880
                    promise->set_value();
                } else {
                    LOGW() << "DoSetState(): " << fgName << " is already in state (" << state << ") !";
                    promise->SetError(errorCode);
                }
            } else {
 /// In subsequent Undefined events, the error code will also be added to the error list, but the two errors are the same, and only one copy will exist per function group
                ara::core::Result< FunctionGroup::CtorToken > r{FunctionGroup::Preconstruct(fgName)};
                if (r.HasValue()) {
                    FunctionGroup const ifg{std::move(r).Value()};
                    pri->AddExecutionErrorEvent(ara::exec::ExecutionErrorEvent{execErrorCode, ifg});
                } else {
                    LOGE() << "Preconsturct FunctionGroup {" << fgName << "} error !!!";
                }

                LOGE() << "DoSetState()::UserCallback(" << fgName << "," << stateName << "): SetError("
                       << static_cast< int32_t >(errorCode) << ")";
                promise->SetError(errorCode);
            }
        })};

    if (p->IsValid()) {
    }

    if (ara::exec::ExecErrc::kNoError != ee) {
        LOGE() << "StateClient::SetState(" << fg << "," << state << "): " << static_cast< int32_t >(ee);
        promise->SetError(ee);
    }

    return future;
}

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
ara::core::Future< void > StateClient::GetInitialMachineStateTransitionResult() const noexcept
{
    std::shared_ptr< ara::core::Promise< void > > promise{std::make_shared< ara::core::Promise< void > >()};
    ara::core::Future< void > future{promise->get_future()};

    if (nullptr == privateImpl_) {
        promise->SetError(ara::exec::ExecErrc::kGeneralError);
        return future;
    }

    if (true != privateImpl_->IsValid()) {
        promise->SetError(ara::exec::ExecErrc::kCommunicationError);
        return future;
    }

    PrivateImpl *const p{privateImpl_.get()};
    ara::exec::ExecErrc const ee{PrivateImpl::DoGetInitialMachineStateTransitionResult(
        p,
        [this, promise](ara::core::StringView const &fgName, ara::core::StringView const &stateName,
                        ara::exec::ExecErrc const errorCode, uint32_t const execErrorCode) -> void {
            if (internal::config::GetFunctionGroupStateStartup() == stateName) {
                promise->set_value();
            } else {
                LOGW() << "StateClient::GetInitialMachineStateTransitionResult():" << fgName << "," << stateName
                       << ", errorCode(" << static_cast< int32_t >(execErrorCode) << "), execErrorCode("
                       << execErrorCode << ") !!!";
                ara::core::Result< FunctionGroup::CtorToken > r{FunctionGroup::Preconstruct(fgName)};  // PRQA S 4117
                if (!r.HasValue()) {
                    LOGE() << "Preconstruct FunctionGroup {" << fgName << "}";
                    promise->SetError(errorCode);
                    return;
                }

                FunctionGroup const fg{std::move(r).Value()};
                ara::exec::ExecutionErrorEvent const execErrEvt{execErrorCode, fg};
                this->privateImpl_->AddExecutionErrorEvent(execErrEvt);

                promise->SetError(errorCode);
            }
        })};

    if (p->IsValid()) {
    }

    if (ara::exec::ExecErrc::kNoError != ee) {
        promise->SetError(ee);
    }
    return future;
}

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
ara::core::Result< ara::exec::ExecutionErrorEvent > StateClient::GetExecutionError(
    ara::exec::FunctionGroup const &functionGroup) const noexcept
{
    ara::exec::ExecutionErrorEvent const *const ee{
        privateImpl_->FindExecutionErrorEvent(functionGroup.GetMetaModelIdentifier())};
    if (nullptr == ee) {
        return ara::core::Result< ara::exec::ExecutionErrorEvent >::FromError(ara::exec::ExecErrc::kFailed);
    }
    return ara::core::Result< ara::exec::ExecutionErrorEvent >::FromValue(*ee);
}

}  // namespace exec
}  // namespace ara
