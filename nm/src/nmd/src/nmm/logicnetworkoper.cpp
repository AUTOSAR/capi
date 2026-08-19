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
/// @file       logicnetworkoper.cpp
/// @brief      Logical network management class
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @trace_id_sr=SRS_NM_00004,SRS_NM_00005,SRS_NM_00014,SRS_NM_00015,SRS_NM_00016,SRS_NM_00019,SRS_NM_00020,SRS_NM_00021,SRS_NM_00022,SRS_NM_00023,SRS_NM_00024
/// @unit_name=LogicNetworkOper
/// @unit_description=Logical network management class
/// @module_path=/NetworkManager/nmm
/// @interface_level=/NetworkManager/nmm
/// @endcode
///
/// ================================================================

#include "include/logicnetworkoper.h"

#include "common/common.h"
#include "config/include/configure.h"
#include "utils/include/utils.h"

namespace ara {
namespace nm {
namespace internal {
/// @brief initialize the class.
/// @param instanceSpec Communication descriptor.
/// @param handleIndex Logical network name.
/// @returns true init ok, false fail.
bool LogicNetworkOper::Init(ara::core::String const &instanceSpec, ara::core::String const &handleIndex) noexcept
{
#ifdef ARA_NM_WITH_COM
    if (nmSmi_ != nullptr) {
        return true;
    }
    instanceSpec_ = instanceSpec;
    handleIndex_  = handleIndex;

    ara::core::StringView const specStringView{instanceSpec_.c_str()};
    NmLogger().LogInfo() << "LogicNetworkOper::Init() logicNetworkIndex_=" << handleIndex_.c_str();
    ara::core::InstanceSpecifier const instSpecifier{specStringView};
    ara::core::Result< ara::com::InstanceIdentifierContainer > const iisRes{
        ara::com::runtime::ResolveInstanceIDs(instSpecifier)};
    if (!iisRes.HasValue()) {
        NmLogger().LogInfo() << "ResolveInstanceIDs failed";
        return false;
    }
    ara::com::InstanceIdentifierContainer iis{iisRes.Value()};
    if (iis.empty()) {
        NmLogger().LogInfo() << "InstanceIdentifierContainer empty " << instanceSpec_.c_str();
        return false;
    }
    NmLogger().LogInfo() << "logicNetworkIndex_=" << handleIndex_.c_str() << ",Get InstanceIdentifier(s) done"
                         << (iis.size());
    ara::core::Result< SMInterface > skeletonRes{SMInterface::Create< SMInterface >(
        std::move(iis.front()), ara::com::MethodCallProcessingMode::kEventSingleThread)};
    if (!skeletonRes.HasValue()) {
        NmLogger().LogInfo() << "Create skeleton failed " << instanceSpec_.c_str();
        return false;
    }

    SMIHandlers callHandle;
    SetRequestStateHandler const requestedSetHandler{
        [this](NetworkStateType const requestState) noexcept -> ara::core::Future< NetworkStateType > {
            ara::core::Promise< NetworkStateType > promise;
            NmLogger().LogInfo() << "logicNetworkIndex_=" << handleIndex_.c_str()
                                 << ", SetNetworkRequestedState :" << static_cast< std::uint8_t >(requestState)
                                 << ", logicNetworkRequestedState_"
                                 << static_cast< std::uint8_t >(logicNetworkRequestedState_);
            if (requestState != logicNetworkRequestedState_) {
                SetLogicNetworkRequestedState(requestState);
            }
            promise.set_value(logicNetworkRequestedState_);
            return promise.get_future();
        }};
    callHandle.setLNRequestStateHandler = requestedSetHandler;
    SetEtherHandler const setDefaultEtherIpHandle{
        [this](ara::core::String const &ethIp, ara::core::Promise< void > &promise) noexcept -> void {
            this->SetDefaultEtherIp(ethIp, promise);
        }};
    callHandle.setDefaultEtherIpHandle = setDefaultEtherIpHandle;

    SetEtherHandler const requestDetectNodeHandle{
        [this](ara::core::String const &ethIp, ara::core::Promise< void > &promise) noexcept -> void {
            this->RequestDetectNode(ethIp, promise);
        }};
    callHandle.requestDetectNodeHandle = requestDetectNodeHandle;

    SetEtherHandler const notifyWakeupHandle{
        [this](ara::core::String const &ethIp, ara::core::Promise< void > &promise) noexcept -> void {
            this->NotifyWakeup(ethIp, promise);
        }};
    callHandle.notifyWakeupHandle = notifyWakeupHandle;

    SetEtherSMSHandler const setEtherStateMachineSateHandle{
        [this](ara::core::String const &ethIp, ara::nm::NetworkStateType const &ethState,
               ara::core::Promise< void > &promise) noexcept -> void {
            this->SetEtherStateMachineSate(ethIp, ethState, promise);
        }};
    callHandle.setEtherStateMachineSateHandle = setEtherStateMachineSateHandle;

    GetEtherSMSHandler const getEtherStateMachineStateHandler{
        [this](ara::core::String const &ethIp, ara::nm::NetworkStateType &type) noexcept -> void {
            std::ignore = type;
            this->GetEtherStateMachineState(ethIp, type);
        }};
    callHandle.getEtherStateMachineStateHandler = getEtherStateMachineStateHandler;

    SetNmMsgCtrlTypeHandler const setEtherNmMsgCtrlTypeHandler{
        [this](ara::core::String const &ethIp, ara::nm::MessageCtrlType const &ctrType,
               ara::core::Promise< void > &promise) noexcept -> void {
            this->SetMessageCtrlType(ethIp, ctrType, promise);
        }};
    callHandle.setEtherNmMsgCtrlTypeHandler = setEtherNmMsgCtrlTypeHandler;

    GetNmMsgCtrlTypeHandler const getEtherNmMsgCtrlTypeHandler{
        [this](ara::core::String const &ethIp, ara::nm::MessageCtrlType &etherCtrlType) noexcept -> void {
            std::ignore = etherCtrlType;
            this->GetMessageCtrlType(ethIp, etherCtrlType);
        }};
    callHandle.getEtherNmMsgCtrlTypeHandler = getEtherNmMsgCtrlTypeHandler;

    GetInitStateHandler const getInitStateHandler{[this](std::uint32_t &lnInitCode) noexcept -> void {
        std::ignore = lnInitCode;
        this->GetInitState(lnInitCode);
    }};
    callHandle.getInitStateHandler = getInitStateHandler;

    callHandle.getPassivePnListHandler
        = {[this](ara::core::String const &ethIp, ara::core::String &etherPnList) noexcept -> void {
              this->GetPassivePnRequestList(ethIp, etherPnList);
          }};

    ara::core::StringView const lnHander{handleIndex_.c_str(), handleIndex_.size()};
    nmSmi_ = std::make_unique< SMInterface >(std::move(skeletonRes).Value());
    bool const voidRes{nmSmi_->InitAndOffer(lnHander, callHandle)};
    if (!voidRes) {
        NmLogger().LogInfo() << "InitAndOffer failed";
        nmSmi_ = nullptr;
        return false;
    }
    NmLogger().LogInfo() << "logicNetworkIndex_=" << handleIndex_.c_str() << ",InitAndOffer done ";
#else
    std::ignore = instanceSpec;
    std::ignore = handleIndex;
#endif
    return true;
}

/// @brief initialize the class.
/// @param handleIndex Logical network name.
/// @returns true init ok, false fail.
bool LogicNetworkOper::InitIpc(ara::core::String const &handleIndex) noexcept
{
    handleIndex_ = handleIndex;
    NmLogger().LogInfo() << "logicNetworkIndex_=" << handleIndex_.c_str();

    IpcHandlers callHandle;
    IpcGetRequestStateHandler const requestedGetHandler{[this]() noexcept -> NetworkStateType {
        if (true == this->terminate_) {
            this->logicNetworkRequestedState_ = NetworkStateType::kNoCom;
        }
        NmLogger().LogInfo() << "LogicNetworkOper logicNetworkIndex_=" << handleIndex_.c_str()
                             << ", requestedGetHandler :logicNetworkRequestedState_="
                             << static_cast< std::uint8_t >(this->logicNetworkRequestedState_);
        ;
        return (this->logicNetworkRequestedState_);
    }};
    callHandle.getLNRequestStateHandler = requestedGetHandler;
    IpcSetRequestStateHandler const requestedSetHandler{
        [this](NetworkStateType const requestState) noexcept -> ara::nm::NetworkStateType {
            NmLogger().LogInfo() << "logicNetworkIndex_=" << handleIndex_.c_str()
                                 << ", SetNetworkRequestedState :" << static_cast< std::uint8_t >(requestState)
                                 << ", logicNetworkRequestedState_"
                                 << static_cast< std::uint8_t >(logicNetworkRequestedState_);
            if (requestState != logicNetworkRequestedState_) {
                SetLogicNetworkRequestedState(requestState);
            }
            return logicNetworkRequestedState_;
        }};
    callHandle.setLNRequestStateHandler = requestedSetHandler;

    IpcGetNetworkCurrentStateHandler const networkcurrentGetHandler{[this]() noexcept -> NetworkStateType {
        if (true == this->terminate_) {
            this->logicNetworkCurrentState_ = NetworkStateType::kNoCom;
        }
        NmLogger().LogInfo() << "SMInterface logicNetworkIndex_=" << handleIndex_.c_str()
                             << ", GetNetworkCurrentStateHandler :logicNetworkCurrentState_="
                             << static_cast< std::uint8_t >(this->logicNetworkCurrentState_);
        ;
        return (this->logicNetworkCurrentState_);
    }};
    callHandle.getNetowrkCurrentStateHandler = networkcurrentGetHandler;

    ara::core::StringView const lnHander{handleIndex_.c_str(), handleIndex_.size()};
    NMIpcProc::GetNmIpcProHandler()->RegLnProcHandle(handleIndex_, callHandle);
    NmLogger().LogInfo() << "logicNetworkIndex_=" << handleIndex_.c_str() << ",InitIpc done ";
    bSupportIpc_ = true;
    return true;
}
/// @brief inner process of setting request.
/// @param requestState set value.
void LogicNetworkOper::SetLogicNetworkRequestedState(NetworkStateType const requestState) noexcept
{
    if (requestState != logicNetworkRequestedState_) {
        if (false == terminate_) {
            logicNetworkRequestedState_ = requestState;
            if (bSupportIpc_) {
                NMIpcProc::GetNmIpcProHandler()->UpdateNetworkRequestState(this->handleIndex_,
                                                                           static_cast< std::uint32_t >(requestState));
            }

            NmLogger().LogInfo() << "logicNetworkIndex_=" << handleIndex_.c_str()
                                 << ",SetLogicNetworkRequestedState, update "
                                    "logicNetworkRequestedState_="
                                 << static_cast< std::uint8_t >(requestState);
            static_cast< void >(
                std::for_each(pnPtrVec_.begin(), pnPtrVec_.end(),
                              [requestState](std::shared_ptr< PartialNetworkOper > const &pnPtr) noexcept {
                                  static_cast< void >(pnPtr->SetPartialNetworkRequestedState(requestState));
                              }));
            for (const auto &etherPair : etherSMPtrVec_) {
                bool bRequestNetwork{false};
                if (NetworkStateType::kFullCom == requestState) {
                    bRequestNetwork = true;
                }
                if (bRequestNetwork) {
                    std::ignore = etherPair.second->RequestNetwork(true);
                } else {
                    std::ignore = etherPair.second->ReleaseNetwork(true);
                }
            }
        }
    }
}

/// @brief process of change of ethernet communicator mode.
/// @param currentState ethernet communicator mode.
void LogicNetworkOper::_notifyEtherModeChanged(NetworkStateType const currentState) noexcept
{
    NmLogger().LogInfo() << "logicNetworkIndex_=" << handleIndex_.c_str()
                         << ",LogicNetworkOper::_notifyEtherModeChanged currentState="
                         << static_cast< std::uint8_t >(currentState);
    if (NetworkStateType::kFullCom == currentState) {
        if (etherSMPtrVec_.size() > fullComEther_) {
            fullComEther_++;
            if ((etherSMPtrVec_.size() == fullComEther_) && (pnPtrVec_.size() == fullComPn_)) {
                _notifyNetworkCurrentStateChanged(currentState);
            }
        }
    } else if (NetworkStateType::kNoCom == currentState) {
        std::uint32_t const fullComEtherNow{fullComEther_};
        if (0U < fullComEther_) {
            fullComEther_--;
            if ((etherSMPtrVec_.size() == fullComEtherNow) && (pnPtrVec_.size() == fullComPn_)) {
                _notifyNetworkCurrentStateChanged(currentState);
            }
        }
    } else {
    }
}

/// @brief process of change of partial network communicator mode.
/// @param currentState partial network communicator mode.
void LogicNetworkOper::NotifyPartialNetworkCurrentStateChanged(NetworkStateType const currentState) noexcept
{
    NmLogger().LogInfo() << "logicNetworkIndex_=" << handleIndex_.c_str()
                         << ",LogicNetworkOper::NotifyPartialNetworkCurrentStateChanged "
                            "requestState="
                         << static_cast< std::uint8_t >(currentState);
    if (NetworkStateType::kFullCom == currentState) {
        if (pnPtrVec_.size() > fullComPn_) {
            fullComPn_++;
            if ((etherSMPtrVec_.size() == fullComEther_) && (pnPtrVec_.size() == fullComPn_)) {
                _notifyNetworkCurrentStateChanged(currentState);
            }
        }
    } else if (NetworkStateType::kNoCom == currentState) {
        std::uint32_t const fullComPnNow{fullComPn_};
        if (fullComPn_ > 0U) {
            fullComPn_--;
            if ((etherSMPtrVec_.size() == fullComEther_) && (pnPtrVec_.size() == fullComPnNow)) {
                _notifyNetworkCurrentStateChanged(currentState);
            }
        }
    } else {
    }
}

/// @brief notify the active logic network change to no-com when terminating nm
/// service;
void LogicNetworkOper::NotifyNMStop() noexcept
{
#ifdef ARA_NM_WITH_COM
    if (nullptr != nmSmi_) {
        nmSmi_->NotifyNMStop();
        nmSmi_ = nullptr;
    }
#endif
    pnPtrVec_.clear();
    etherSMPtrVec_.clear();
    terminate_ = true;
}

/// @brief report network current state of logic network to sm.
/// @param currentState network current state of logic network.
void LogicNetworkOper::_notifyNetworkCurrentStateChanged(NetworkStateType const currentState) noexcept
{
    NmLogger().LogInfo() << "logicNetworkIndex_=" << handleIndex_.c_str()
                         << ",_notifyNetworkCurrentStateChanged, update logicNetworkCurrentState_="
                         << static_cast< std::uint8_t >(currentState);
    if (currentState != logicNetworkCurrentState_) {
        if (false == terminate_) {
            logicNetworkCurrentState_ = currentState;
            std::ignore               = isoft::naicpp::GlobalGeneralEvLoop::Get()->Exec([this]() noexcept -> void {
#ifdef ARA_NM_WITH_COM
                if (nullptr != nmSmi_) {
                    static_cast< void >(this->nmSmi_->UpdateNetworkCurrentState(this->logicNetworkCurrentState_));
                }
#endif
                if (bSupportIpc_) {
                    NMIpcProc::GetNmIpcProHandler()->UpdateNetworkCurrentState(
                        this->handleIndex_, static_cast< std::uint32_t >(this->logicNetworkCurrentState_));
                }
            });
        }
    }
}

/// @brief Node detection.
/// @param etherIp NIC IP
/// @param promise promise object reference
void LogicNetworkOper::RequestDetectNode(ara::core::String const &etherIp, ara::core::Promise< void > &promise) noexcept
{
    DetectNodeCallBack const detcnodeListCall{
        [this](ara::core::Vector< std::uint8_t > const &nodeVec) noexcept -> void {
            this->_detctNodesCallbak(nodeVec);
        }};
    ara::core::String const *pStr{&etherIp};
    if (etherIp.empty()) {
        pStr = &defaultEtherIp_;
    }

    ara::core::Result< void > const res;
    EtherIterator const etherPair{etherSMPtrVec_.find(*pStr)};
    if (etherSMPtrVec_.end() != etherPair) {
        etherPair->second->RequestDetectNode(this->handleIndex_, kInvalidPnID, detcnodeListCall);
    } else {
        // Search for PN
        ParttIter const itend{std::end(pnPtrVec_)};
        for (ParttIter it{std::begin(pnPtrVec_)}; itend != it; ++it) {
            if ((*it)->HaveTheEther(*pStr)) {
                (*it)->RequestDetectNode(*pStr, detcnodeListCall);
                break;
            }
        }
    }
    promise.SetResult(res);
}

/// @brief  Set default IP. If the IP passed to the following interfaces is empty, the default IP will be used.
/// @param etherIp NIC IP
/// @param promise promise object reference
void LogicNetworkOper::SetDefaultEtherIp(ara::core::String const &etherIp, ara::core::Promise< void > &promise) noexcept
{
    NmLogger().LogDebug() << "LogicNetworkOper SetDefaultEtherIp " << etherIp.c_str();
    this->defaultEtherIp_ = etherIp;
    ara::core::Result< void > const res;
    promise.SetResult(res);
}

/// @brief Node wakeup.
/// @param etherIp NIC IP
/// @param promise promise object reference
void LogicNetworkOper::NotifyWakeup(ara::core::String const &etherIp, ara::core::Promise< void > &promise) noexcept
{
    ara::core::String const *pStr{&etherIp};
    if (etherIp.empty()) {
        pStr = &defaultEtherIp_;
    }
    ara::core::Result< void > const res;
    EtherIterator const etherPair{etherSMPtrVec_.find(*pStr)};
    if (etherSMPtrVec_.end() != etherPair) {
        etherPair->second->NotifyWakeup();
    } else {
        // Search for PN
        ParttIter const itend{std::end(pnPtrVec_)};
        for (ParttIter it{std::begin(pnPtrVec_)}; itend != it; ++it) {
            if ((*it)->HaveTheEther(*pStr)) {
                (*it)->NotifyWakeup(*pStr);
                break;
            }
        }
    }
    promise.SetResult(res);
}

/// @brief Set NIC state machine state
/// @param etherIp NIC IP
/// @param ethState NIC IP state
/// @param promise promise object reference
void LogicNetworkOper::SetEtherStateMachineSate(ara::core::String const &etherIp,
                                                ara::nm::NetworkStateType const &ethState,
                                                ara::core::Promise< void > &promise) noexcept
{
    ara::core::String const *pStr{&etherIp};

    EthernetStateMachineStateType const ethSMstate{GetInnerSMState(ethState)};
    if (etherIp.empty()) {
        pStr = &defaultEtherIp_;
    }
    ara::core::Result< void > const res;
    EtherIterator const etherPair{etherSMPtrVec_.find(*pStr)};
    if (etherSMPtrVec_.end() != etherPair) {
        etherPair->second->SetEtherStateMachineSate(ethSMstate);
    } else {
        // Search for PN
        ParttIter const itend{std::end(pnPtrVec_)};
        for (ParttIter it{std::begin(pnPtrVec_)}; itend != it; ++it) {
            if ((*it)->HaveTheEther(*pStr)) {
                (*it)->SetEtherStateMachineSate(*pStr, ethSMstate);
                break;
            }
        }
    }
    promise.SetResult(res);
}

/// @brief Get NIC state machine state
/// @param etherIp NIC IP
/// @param innerState promise object reference
void LogicNetworkOper::GetEtherStateMachineState(ara::core::String const &etherIp,
                                                 ara::nm::NetworkStateType &innerState) noexcept
{
    ara::core::String const *pStr{&etherIp};
    if (etherIp.empty()) {
        pStr = &defaultEtherIp_;
    }

    EthernetStateMachineStateType state{EthernetStateMachineStateType::kStateBusSleep};

    EtherIterator const etherPair{etherSMPtrVec_.find(*pStr)};
    if (etherSMPtrVec_.end() != etherPair) {
        state = etherPair->second->GetEtherStateMachineState();
    } else {
        // Search for PN
        ParttIter const itend{std::end(pnPtrVec_)};
        for (ParttIter it{std::begin(pnPtrVec_)}; itend != it; ++it) {
            if ((*it)->HaveTheEther(*pStr)) {
                state = (*it)->GetEtherStateMachineState(*pStr);
                break;
            }
        }
    }
    std::ignore = innerState;
    NmLogger().LogDebug() << "LogicNetworkOper GetEtherStateMachineState, state=" << static_cast< std::uint8_t >(state);
    innerState = GetComSMState(state);
}

/// @brief Node detection result notification
/// @param nodeList Node detection result
void LogicNetworkOper::_detctNodesCallbak(ara::core::Vector< std::uint8_t > const &nodeList) const noexcept
{
#ifdef ARA_NM_WITH_COM
    if (nullptr != nmSmi_) {
        nmSmi_->DetctNodesCallbak(nodeList);
    }
#else
    std::ignore = nodeList;
#endif
}

/// @brief process of change of ethernet communicator mode.
/// @param etherIp connect ip.
/// @param stateType ethernet communicator mode.
void LogicNetworkOper::NotifyEtherStateChanged(ara::core::String const &etherIp,
                                               EthernetStateMachineStateType const &stateType) noexcept
{
    NmLogger().LogDebug() << "LogicNetworkOper NotifyEtherStateChanged etherIp:" << etherIp.c_str()
                          << ", stateType=" << static_cast< std::uint8_t >(stateType);

    /// If LN1's PN1 and PN2 both contain eth1,
    /// then eth1's status will be notified twice, the logical network will filter it, only notifying when status changes
    if (0U == etherStateMap_.count(etherIp)) {
        /// The default state of the NIC is bus_sleep
        etherStateMap_[etherIp] = EthernetStateMachineStateType::kStateBusSleep;
    }
    EthStateIter const etherIt{etherStateMap_.find(etherIp)};
    if (stateType != etherIt->second) {
        etherStateMap_[etherIp] = stateType;
#ifdef ARA_NM_WITH_COM
        ara::nm::NetworkStateType const comStateType{GetComSMState(stateType)};
        if (nullptr != nmSmi_) {
            nmSmi_->SendEtherStateChanged(etherIp, comStateType);
        }
#endif
    }
}

/// @brief Set NM message send/receive control
/// @param etherIp NIC IP
/// @param ctrType Send/receive control
/// @param promise promise object reference
void LogicNetworkOper::SetMessageCtrlType(ara::core::String const &etherIp,
                                          ara::nm::MessageCtrlType const &ctrType,
                                          ara::core::Promise< void > &promise) noexcept
{
    ara::core::String const *pStr{&etherIp};
    if (etherIp.empty()) {
        pStr = &defaultEtherIp_;
    }
    ara::core::Result< void > const res;
    EtherIterator const etherPair{etherSMPtrVec_.find(*pStr)};
    if (etherSMPtrVec_.end() != etherPair) {
        etherPair->second->SetMessageCtrlType(ctrType);
    } else {
        // Search for PN
        ParttIter const itend{std::end(pnPtrVec_)};
        for (ParttIter it{std::begin(pnPtrVec_)}; itend != it; ++it) {
            if ((*it)->HaveTheEther(*pStr)) {
                (*it)->SetMessageCtrlType(*pStr, ctrType);
                break;
            }
        }
    }
    promise.SetResult(res);
}

/// @brief Get NM message send/receive control
/// @param etherIp NIC IP
/// @param etherCtrlType Status return value
void LogicNetworkOper::GetMessageCtrlType(ara::core::String const &etherIp,
                                          ara::nm::MessageCtrlType &etherCtrlType) noexcept
{
    ara::core::String const *pStr{&etherIp};
    if (etherIp.empty()) {
        pStr = &defaultEtherIp_;
    }
    ara::nm::MessageCtrlType ctrType{ara::nm::MessageCtrlType::kEnableAll};
    EtherIterator const etherPair{etherSMPtrVec_.find(*pStr)};
    if (etherSMPtrVec_.end() != etherPair) {
        ctrType = etherPair->second->GetMessageCtrlType();
    } else {
        // Search for PN
        ParttIter const itend{std::end(pnPtrVec_)};
        for (ParttIter it{std::begin(pnPtrVec_)}; itend != it; ++it) {
            if ((*it)->HaveTheEther(*pStr)) {
                ctrType = (*it)->GetMessageCtrlType(*pStr);
                break;
            }
        }
    }
    std::ignore = etherCtrlType;
    NmLogger().LogDebug() << "LogicNetworkOper GetMessageCtrlType,ctrType=" << static_cast< std::uint8_t >(ctrType);
    etherCtrlType = ctrType;
}

/// @brief Get initialization status
/// @param lnInitCode Status return value
void LogicNetworkOper::GetInitState(std::uint32_t &lnInitCode) noexcept
{
    std::uint32_t initCode{0U};
    for (const auto &etherPtr : etherSMPtrVec_) {
        if (true == etherPtr.second->HasDtcError()) {
            NmLogger().LogDebug() << "LogicNetworkOper HasDtcError " << etherPtr.first.c_str();
            initCode = kNmEtherInitErr;
            break;
        }
    }
    if (0U == initCode) {
        ParttIter const itend{std::end(pnPtrVec_)};
        for (ParttIter it{std::begin(pnPtrVec_)}; itend != it; ++it) {
            initCode = (*it)->GetInitState();
            if (0U != initCode) {
                break;
            }
        }
    }
    std::ignore = lnInitCode;
    NmLogger().LogDebug() << "LogicNetworkOper GetInitState,initCode=" << initCode;
    lnInitCode = initCode;
}

/// @brief PN request notification.
/// @param etherIp NIC IP.
/// @param pnList PN set.
void LogicNetworkOper::NotifyPassivePnRequest(ara::core::String const &etherIp,
                                              ara::core::String const &pnList) const noexcept
{
    NmLogger().LogDebug() << "LogicNetworkOper::NotifyPassivePnRequest  etherIp=" << etherIp.c_str()
                          << ", pnList=" << pnList.c_str();
#ifdef ARA_NM_WITH_COM
    if (nullptr != nmSmi_) {
        nmSmi_->PassivePnRequest(etherIp, pnList);
    }
#endif
}

/// @brief PN release notification.
/// @param etherIp NIC IP.
/// @param pnList PN set.
void LogicNetworkOper::NotifyPassivePnRelease(ara::core::String const &etherIp,
                                              ara::core::String const &pnList) const noexcept
{
    NmLogger().LogDebug() << "LogicNetworkOper::NotifyPassivePnRelease  etherIp=" << etherIp.c_str()
                          << ", pnList=" << pnList.c_str();
#ifdef ARA_NM_WITH_COM
    if (nullptr != nmSmi_) {
        nmSmi_->PassivePnRelease(etherIp, pnList);
    }
#endif
}

/// @brief Get passive PN list
/// @param etherIp NIC IP
/// @param etherPnList Passive PN list
void LogicNetworkOper::GetPassivePnRequestList(ara::core::String const &etherIp,
                                               ara::core::String &etherPnList) noexcept
{
    ara::core::String const *pStr{&etherIp};
    ara::core::String pnList;
    if (etherIp.empty()) {
        pStr = &defaultEtherIp_;
    }
    EtherIterator const etherPair{etherSMPtrVec_.find(*pStr)};
    if (etherSMPtrVec_.end() != etherPair) {
        pnList = etherPair->second->GetPassivePnRequestList();
    } else {
        // Search for PN
        for (const auto &pnPtr : pnPtrVec_) {
            if (pnPtr->HaveTheEther(*pStr)) {
                pnList = pnPtr->GetPassivePnRequestList(*pStr);
                break;
            }
        }
    }
    NmLogger().LogDebug() << "LogicNetworkOper GetPassivePnRequestList,pnList=" << pnList.c_str();
    std::ignore = etherPnList;
    etherPnList = pnList;
}

/// @brief destructor of UdpNm.
LogicNetworkOper::~LogicNetworkOper() { NMIpcProc::CloseNmIpcProHandler(); }
}  // namespace internal
}  // namespace nm
}  // namespace ara
