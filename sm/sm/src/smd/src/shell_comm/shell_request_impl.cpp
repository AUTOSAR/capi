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
/// @file       shell_request_impl.cpp
/// @brief      A implementation of ara::sm::skeleton::ShellRequestSkeleton.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/ShellComm
/// @interface_level=uint
/// @trace_id_sr=SR_SM_10001
/// @unit_name=ShellRequestImpl
/// @unit_description=A implementation of ara::sm::skeleton::ShellRequestSkeleton.
/// @endcode
///
/// ================================================================

#include "shell_request_impl.h"

#include <ara/core/string.h>
#include <ara/sm/impl_type_functiongroupinfo.h>
#include <ara/sm/impl_type_transitionitem.h>

#include "event.h"
#include "helper.h"

namespace ara {
namespace sm {
namespace shell_comm {

/// @brief Destructor
ShellRequestImpl::~ShellRequestImpl() noexcept { log_.LogInfo() << "ShellRequestImpl::~ShellRequestImpl()"; }

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void ShellRequestImpl::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > const &appendEventHandler) noexcept
{
    appendEventHandler_ = appendEventHandler;
}

/// @brief Set state machine state
/// @param smName State machine name
/// @param sMStateRequest State machine state request sequence number
/// @return Future object
core::Future< void > ShellRequestImpl::SetSMState(core::String const &smName,
                                                  std::uint32_t const &sMStateRequest) noexcept
{
    log_.LogInfo() << "ShellRequestImpl::SetSMState(), smName:" << smName.c_str()
                   << "SMStateRequest:" << sMStateRequest;
    core::Promise< void > promise{};
    core::Future< void > future{promise.get_future()};
    if (appendEventHandler_) {
        common::Event request;
        request.type           = common::EventType::kInShellSetSMState;
        request.data           = new common::SMStateRequestInfo{smName, sMStateRequest};
        request.requestPromise = std::move(promise);
        appendEventHandler_(std::move(request));
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogError() << "ShellRequestImpl::SetSMState(), appendEventHandler_ is nullptr";
    }
    return future;
}

/// @brief Set function group state
/// @param fgName Function group name
/// @param fgState Function group state
/// @return Future object
core::Future< void > ShellRequestImpl::SetFGState(core::String const &fgName, core::String const &fgState) noexcept
{
    log_.LogInfo() << "ShellRequestImpl::SetFGState(), fgName:" << fgName.c_str() << "fgState:" << fgState.c_str();
    core::Promise< void > promise;
    core::Future< void > future{promise.get_future()};
    if (appendEventHandler_) {
        common::Event request;
        request.type           = common::EventType::kInShellSetFGState;
        request.data           = new common::FGStateInfo{fgName, fgState};
        request.requestPromise = std::move(promise);
        appendEventHandler_(std::move(request));
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogError() << "ShellRequestImpl::SetFGState(), appendEventHandler_ is nullptr";
    }
    return future;
}

/// @brief Get state machine state
/// @param smName State machine name
/// @return Future object
core::Future< ShellRequestImpl::GetSMStateOutput > ShellRequestImpl::GetSMState(core::String const &smName) noexcept
{
    log_.LogInfo() << "ShellRequestImpl::GetSMState(), smName:" << smName.c_str();
    if (appendEventHandler_) {
        core::Promise< core::String > promiseForEvent;
        std::shared_ptr< core::Future< core::String > > futureForEvent{
            std::make_shared< core::Future< core::String > >(promiseForEvent.get_future())};
        core::Future< ShellRequestImpl::GetSMStateOutput > futureForMethod{
            futureForEvent->then([this, futureForEvent, &smName]() mutable {
                core::Promise< ShellRequestImpl::GetSMStateOutput > promise;
                core::Future< ShellRequestImpl::GetSMStateOutput > future{promise.get_future()};
                core::Result< core::String > const smStateRes{futureForEvent->GetResult()};
                if (smStateRes.HasValue()) {
                    ShellRequestImpl::GetSMStateOutput output;
                    output.SMState = smStateRes.Value();
                    log_.LogInfo() << "ShellRequestImpl::GetSMState(), smName:" << smName.c_str()
                                   << "smState:" << smStateRes.Value().c_str();
                    promise.set_value(std::move(output));
                } else {
                    ara::core::ErrorCode errorCode{smStateRes.Error()};
                    log_.LogError() << "ShellRequestImpl::GetSMState(), smName:" << smName.c_str()
                                    << "error:" << errorCode.Message().data();
                    promise.SetError(std::move(errorCode));
                }
                futureForEvent = nullptr;
                return future;
            })};
        common::Event request;
        request.type                  = common::EventType::kInShellGetSMState;
        request.data                  = new core::String(smName);
        request.requestPromiseWithStr = std::move(promiseForEvent);
        appendEventHandler_(std::move(request));
        return futureForMethod;
    }
    core::Promise< ShellRequestImpl::GetSMStateOutput > promise;
    core::Future< ShellRequestImpl::GetSMStateOutput > future{promise.get_future()};
    promise.SetError(SMErrc::kRejected);
    log_.LogError() << "ShellRequestImpl::GetSMState(), appendEventHandler_ is nullptr";
    return future;
}

/// @brief Get function group state
/// @param fgName Function group name
/// @return Future object
core::Future< ShellRequestImpl::GetFGStateOutput > ShellRequestImpl::GetFGState(core::String const &fgName) noexcept
{
    log_.LogInfo() << "ShellRequestImpl::GetFGState(), fgName:" << fgName.c_str();
    if (appendEventHandler_) {
        core::Promise< core::String > promiseForEvent;
        std::shared_ptr< core::Future< core::String > > futureForEvent{
            std::make_shared< core::Future< core::String > >(promiseForEvent.get_future())};
        core::Future< ShellRequestImpl::GetFGStateOutput > futureForMethod{
            futureForEvent->then([this, futureForEvent, &fgName]() mutable {
                core::Promise< ShellRequestImpl::GetFGStateOutput > promise;
                core::Future< ShellRequestImpl::GetFGStateOutput > future{promise.get_future()};
                core::Result< core::String > const fgStateRes{futureForEvent->GetResult()};
                if (fgStateRes.HasValue()) {
                    ShellRequestImpl::GetFGStateOutput output;
                    output.FGState = fgStateRes.Value();
                    log_.LogInfo() << "ShellRequestImpl::GetFGState(), fgName:" << fgName.c_str()
                                   << "fgState:" << fgStateRes.Value().c_str();
                    promise.set_value(std::move(output));
                } else {
                    ara::core::ErrorCode errorCode{fgStateRes.Error()};
                    log_.LogError() << "ShellRequestImpl::GetFGState(), fgName:" << fgName.c_str()
                                    << "error:" << errorCode.Message().data();
                    promise.SetError(std::move(errorCode));
                }
                futureForEvent = nullptr;
                return future;
            })};
        common::Event request;
        request.type                  = common::EventType::kInShellGetFGState;
        request.data                  = new core::String(fgName);
        request.requestPromiseWithStr = std::move(promiseForEvent);
        appendEventHandler_(std::move(request));
        return futureForMethod;
    }
    core::Promise< ShellRequestImpl::GetFGStateOutput > promise;
    core::Future< ShellRequestImpl::GetFGStateOutput > future{promise.get_future()};
    promise.SetError(SMErrc::kRejected);
    log_.LogError() << "ShellRequestImpl::GetFGState(), appendEventHandler_ is nullptr";
    return future;
}

/// @brief Get function group state
/// @return Future object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_SM_00606
/// @trace_id_dd=DD_SM_00726
/// @needwork = ad
/// @endcode
core::Future< ShellRequestImpl::GetAllFunctionGroupsInfoOutput > ShellRequestImpl::GetAllFunctionGroupsInfo() noexcept
{
    log_.LogInfo() << "ShellRequestImpl::GetAllFunctionGroupsInfo()";
    if (appendEventHandler_) {
        core::Promise< core::String > promiseForEvent;
        std::shared_ptr< core::Future< core::String > > futureForEvent{
            std::make_shared< core::Future< core::String > >(promiseForEvent.get_future())};
        core::Future< ShellRequestImpl::GetAllFunctionGroupsInfoOutput > futureForMethod{
            futureForEvent->then([this, futureForEvent]() mutable {
                core::Promise< ShellRequestImpl::GetAllFunctionGroupsInfoOutput > promise;
                core::Future< ShellRequestImpl::GetAllFunctionGroupsInfoOutput > future{promise.get_future()};
                core::Result< core::String > const fgsInfoRes{futureForEvent->GetResult()};
                if (fgsInfoRes.HasValue()) {
                    ShellRequestImpl::GetAllFunctionGroupsInfoOutput output;
                    core::Vector< common::FunctionGroupsInfoSM > fgsInfoList{
                        common::StrToFunctionGroupsInfo(fgsInfoRes.Value())};
                    for (common::FunctionGroupsInfoSM &info : fgsInfoList) {
                        ara::sm::FunctionGroupsInfo methodFunctionGroupsInfo;
                        methodFunctionGroupsInfo.managedBySm = info.isManagedBySm;
                        methodFunctionGroupsInfo.smFQN       = std::move(info.smFQN);
                        methodFunctionGroupsInfo.smStateList = std::move(info.smStates);
                        for (common::TransitionItemSM &item : info.transitionTable) {
                            ara::sm::TransitionItem methodTransitionItem;
                            methodTransitionItem.requestID      = item.requestID;
                            methodTransitionItem.currentSmState = std::move(item.currentState);
                            methodTransitionItem.nextSmState    = std::move(item.nextState);
                            methodFunctionGroupsInfo.transitionList.push_back(std::move(methodTransitionItem));
                        }
                        for (common::FunctionGroupInfoSM const &functionGroupInfo : info.fgInfos) {
                            ara::sm::FunctionGroupInfo methodFunctionGroupInfo;
                            methodFunctionGroupInfo.fgFQN       = functionGroupInfo.fgFQN;
                            methodFunctionGroupInfo.fgStateList = functionGroupInfo.fgStates;
                            methodFunctionGroupsInfo.fgInfoList.push_back(std::move(methodFunctionGroupInfo));
                        }
                        output.infos.push_back(methodFunctionGroupsInfo);
                    }
                    promise.set_value(std::move(output));
                } else {
                    ara::core::ErrorCode const errorCode{fgsInfoRes.Error()};
                    promise.SetError(errorCode);
                }
                futureForEvent = nullptr;
                return future;
            })};
        common::Event request;
        request.type                  = common::EventType::kInShellGetAllFGInfos;
        request.requestPromiseWithStr = std::move(promiseForEvent);
        appendEventHandler_(std::move(request));
        return futureForMethod;
    }
    core::Promise< ShellRequestImpl::GetAllFunctionGroupsInfoOutput > promise;
    core::Future< ShellRequestImpl::GetAllFunctionGroupsInfoOutput > future{promise.get_future()};
    promise.SetError(SMErrc::kRejected);
    log_.LogError() << "ShellRequestImpl::GetFGState(), appendEventHandler_ is nullptr";
    return future;
}

void ShellRequestImpl::NotifySMStateUpdate(core::String const &smFQN, core::String const &smState) noexcept
{
    std::shared_ptr< ara::sm::StateMachineInfo > const data{
        std::move(NotifyStateMachineStateChanged.Allocate()).Value()};

    data->smFQN   = smFQN;
    data->smState = smState;
    std::ignore   = NotifyStateMachineStateChanged.Send(data);
}
}  // namespace shell_comm
}  // namespace sm
}  // namespace ara