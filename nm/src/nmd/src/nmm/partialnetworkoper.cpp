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
/// @file       partialnetworkoper.cpp
/// @brief      Partial network management class
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @trace_id_sr=SRS_NM_00003
/// @unit_name=PartialNetworkOper
/// @unit_description=Partial network management class
/// @module_path=/NetworkManager/nmm
/// @interface_level=/NetworkManager/nmm
/// @endcode
///
/// ================================================================

#include "include/partialnetworkoper.h"

#include <algorithm>

#include "common/common.h"
#include "config/include/configure.h"
#include "utils/include/utils.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief Destructor of PartialNetworkOper.
PartialNetworkOper::~PartialNetworkOper() noexcept
{
    etherSMPtrVec_.clear();
    mainLoop_                = nullptr;
    pnResetTimerRef_         = nullptr;
    pncPrepareSleepTimerRef_ = nullptr;
    pnModObservGroup_.clear();
    lnWatchEtherStateGroup_.clear();
}

/// @brief set request state of partial network.
/// @param requestState network request state of partial network.
void PartialNetworkOper::SetPartialNetworkRequestedState(NetworkStateType const requestState) noexcept
{
    NmLogger().LogDebug() << "pnHandleIndex=" << handleIndex_.c_str()
                          << ",PartialNetworkOper::SetPartialNetworkRequestedState requestState"
                          << static_cast< std::uint8_t >(requestState);
    if (NetworkStateType::kFullCom == requestState) {
        networkRequestedCount_++;
        if (1U == networkRequestedCount_) {
            _otherStateToPncRequested();
            for (const auto &etherPair : etherSMPtrVec_) {
                std::ignore = etherPair.second->RequestNetwork(false, this->pnId_);
            }
            if (etherSMPtrVec_.size() == fullComEther_) {
                _notifyPartialNetworkCurrentStateChanged(requestState);
            }
        }
    } else if (NetworkStateType::kNoCom == requestState) {
        if (0U < networkRequestedCount_) {
            networkRequestedCount_--;
            if (0U == networkRequestedCount_) {
                _pncRequestedToPncReadySleep();
                for (const auto &etherPair : etherSMPtrVec_) {
                    std::ignore = etherPair.second->ReleaseNetwork(false, this->pnId_);
                }
            }
        }
    } else {
    }
}

/// @brief process of change of ethernet communicator mode.
/// @param currentState ethernet communicator mode.
void PartialNetworkOper::_notifyEtherModeChanged(NetworkStateType const currentState) noexcept
{
    NmLogger().LogDebug() << "pnHandleIndex=" << handleIndex_.c_str()
                          << ",PartialNetworkOper::_notifyEtherModeChanged currentState"
                          << static_cast< std::uint8_t >(currentState);
    if (NetworkStateType::kFullCom == currentState) {
        if (etherSMPtrVec_.size() > fullComEther_) {
            fullComEther_++;
            // AAAAA
            if (_hasBeenRequested()) {
                _notifyPartialNetworkCurrentStateChanged(currentState);
            }
        }
    } else if (NetworkStateType::kNoCom == currentState) {
        std::uint32_t const fullComEtherNow{fullComEther_};
        if (fullComEther_ > 0U) {
            fullComEther_--;
            if ((etherSMPtrVec_.size() == fullComEtherNow)
                && (PNCSMStateType::kNmPncNoCommunication != pnSMStateType_)) {
                _notifyPartialNetworkCurrentStateChanged(currentState);
            }
        }
    } else {
    }
}

/// @brief report current state of to logic network.
/// @param currentState network request state.
void PartialNetworkOper::_notifyPartialNetworkCurrentStateChanged(NetworkStateType const currentState) noexcept
{
    NmLogger().LogDebug() << "pnHandleIndex=" << handleIndex_.c_str()
                          << ",PartialNetworkOper::_notifyPartialNetworkCurrentStateChanged "
                             "currentState"
                          << static_cast< std::uint8_t >(currentState);
    if (currentState != pnComState_) {
        pnComState_ = currentState;

        for (ara::core::Vector< PnComModeHandler >::iterator it{pnModObservGroup_.begin()};
             pnModObservGroup_.end() != it; ++it) {
            (*it)(currentState);
        }
    }
}

/// @brief update pn is active.
void PartialNetworkOper::_updatePnMsg() noexcept
{
    NmLogger().LogDebug() << "pnHandleIndex=" << handleIndex_.c_str() << ",PartialNetworkOper::_updatePnMsg";
    pnMsgCount_++;
    if ((etherSMPtrVec_.size() == pnMsgCount_) && (etherSMPtrVec_.size() == fullComEther_)) {
        if ((PNCSMStateType::kNmPncRequest == pnSMStateType_) || (PNCSMStateType::kNmPncReadySleep == pnSMStateType_)) {
            if (0 != pnResetTimerRef_->UpdateTime(TimeS2MS(pnResetTime_))) {
                NmLogger().LogError() << "pnHandleIndex=" << handleIndex_.c_str() << ", UpdateTime error";
            }
        } else {
            _switchToPncReadySleep();
        }
        pnMsgCount_ = 0U;
        _notifyPartialNetworkCurrentStateChanged(NetworkStateType::kFullCom);
    }
}

/// @brief content initialization.
/// @param pncId      Partial network ID
/// @param handleIndex handle index of partial network.
/// @param pnResetTime PncReadySleep state timeout duration.
/// @param pncPrepareSleepTime pncPrepareSleep state timeout duration.
/// @returns               0 ok
std::int32_t PartialNetworkOper::Init(std::uint16_t const pncId,
                                      ara::core::String const &handleIndex,
                                      double const pnResetTime,
                                      double const pncPrepareSleepTime) noexcept
{
    pnId_                = pncId;
    handleIndex_         = handleIndex;
    pnResetTime_         = pnResetTime;
    pncPrepareSleepTime_ = pncPrepareSleepTime;
    mainLoop_            = isoft::naicpp::GlobalGeneralEvLoop::Get();
    return 0;
}

/// @brief reset timer process
void PartialNetworkOper::_pnResetTimerCalled() noexcept
{
    NmLogger().LogDebug() << "handleIndex_=" << handleIndex_.c_str() << ",PartialNetworkOper::PnResetTimer expire";
    if (PNCSMStateType::kNmPncRequest == pnSMStateType_) {
        if (0 != pnResetTimerRef_->UpdateTime(TimeS2MS(pnResetTime_))) {
            NmLogger().LogError() << "_pnResetTimerCalled pnHandleIndex=" << handleIndex_.c_str()
                                  << ", UpdateTime error";
        }
    } else if (PNCSMStateType::kNmPncReadySleep == pnSMStateType_) {
        if (pnMsgCount_ >= etherSMPtrVec_.size()) {
            pnMsgCount_ = 0U;
            if (0 != pnResetTimerRef_->UpdateTime(TimeS2MS(pnResetTime_))) {
                NmLogger().LogError() << "_pnResetTimerCalled pnHandleIndex=" << handleIndex_.c_str()
                                      << ", UpdateTime error";
            }
        } else {
            _pncReadySleepToPncPrepareSleep();
        }
    } else {
    }
}

/// @brief pn prepare-sleep timer process
void PartialNetworkOper::_pnPrepareSleepTimerCalled() noexcept
{
    NmLogger().LogDebug() << "handleIndex_=" << handleIndex_.c_str()
                          << ",PartialNetworkOper::PnPrepareSleepTimer expire";
    if (PNCSMStateType::kNmPncPrepareSleep == pnSMStateType_) {
        _pncParepareSleepToNoCommunication();
    }
}

/// @brief pnc state machine from other state To PncRequested
void PartialNetworkOper::_otherStateToPncRequested() noexcept
{
    NmLogger().LogDebug() << "handleIndex_=" << handleIndex_.c_str()
                          << ",PartialNetworkOper::_otherStateToPncRequested enter";
    if (PNCSMStateType::kNmPncRequest != pnSMStateType_) {
        if (pnResetTimerRef_) {
            pnResetTimerRef_.reset();
        }
        if (pncPrepareSleepTimerRef_) {
            pncPrepareSleepTimerRef_.reset();
        }

        std::int32_t const defaultNmPnResetTime{TimeS2MS(pnResetTime_)};
        if (0 != NmMakeTimer(mainLoop_, pnResetTimerRef_, defaultNmPnResetTime, [this]() noexcept -> void {
                this->_pnResetTimerCalled();
            })) {
            NmLogger().LogError() << "_otherStateToPncRequested handleIndex_=" << handleIndex_.c_str()
                                  << ", MakeTimer error";
        }
        pnMsgCount_    = 0U;
        pnSMStateType_ = PNCSMStateType::kNmPncRequest;
    }
}

/// @brief pnc state machine from PncRequested To PncReadySleep
void PartialNetworkOper::_pncRequestedToPncReadySleep() noexcept
{
    NmLogger().LogDebug() << "handleIndex_=" << handleIndex_.c_str()
                          << ",PartialNetworkOper::_pncRequestedToPncReadySleep";
    if (PNCSMStateType::kNmPncRequest == pnSMStateType_) {
        pnMsgCount_    = 0U;
        pnSMStateType_ = PNCSMStateType::kNmPncReadySleep;
    }
}

/// @brief pnc state machine from PncReadySleep To PncPrepareSleep
void PartialNetworkOper::_pncReadySleepToPncPrepareSleep() noexcept
{
    NmLogger().LogDebug() << "handleIndex_=" << handleIndex_.c_str()
                          << ",PartialNetworkOper::_pncReadySleepToPncPrepareSleep";
    if (PNCSMStateType::kNmPncReadySleep == pnSMStateType_) {
        pnSMStateType_ = PNCSMStateType::kNmPncPrepareSleep;
        pnResetTimerRef_.reset();
        pncPrepareSleepTimerRef_.reset();
        std::int32_t const defaultPncPrepareSleepTime{TimeS2MS(pncPrepareSleepTime_)};
        if (0
            != NmMakeTimer(mainLoop_, pncPrepareSleepTimerRef_, defaultPncPrepareSleepTime,
                           [this]() noexcept -> void { this->_pnPrepareSleepTimerCalled(); })) {
            NmLogger().LogError() << "_pncReadySleepToPncPrepareSleep handleIndex_=" << handleIndex_.c_str()
                                  << ", MakeTimer error";
        }
    }
}

/// @brief prepare sleep timer process to No Communication
void PartialNetworkOper::_pncParepareSleepToNoCommunication() noexcept
{
    NmLogger().LogDebug() << "handleIndex_=" << handleIndex_.c_str()
                          << ",PartialNetworkOper::_pncParepareSleepToNoCommunication enter";
    if (PNCSMStateType::kNmPncPrepareSleep == pnSMStateType_) {
        pncPrepareSleepTimerRef_.reset();
        pnSMStateType_ = PNCSMStateType::kNmPncNoCommunication;
        _notifyPartialNetworkCurrentStateChanged(NetworkStateType::kNoCom);
    }
}

/// @brief switch to pnc ready sleep, from no-communication or prepare-sleep
void PartialNetworkOper::_switchToPncReadySleep() noexcept
{
    NmLogger().LogDebug() << "handleIndex_=" << handleIndex_.c_str()
                          << ",PartialNetworkOper::_switchToPncReadySleep enter";
    pnResetTimerRef_.reset();
    std::int32_t const defaultNmPnResetTime{TimeS2MS(pnResetTime_)};
    if (0 != NmMakeTimer(mainLoop_, pnResetTimerRef_, defaultNmPnResetTime, [this]() noexcept -> void {
            this->_pnResetTimerCalled();
        })) {
        NmLogger().LogError() << "_switchToPncReadySleep handleIndex_=" << handleIndex_.c_str() << ", MakeTimer error";
    }
    pncPrepareSleepTimerRef_.reset();
    pnSMStateType_ = PNCSMStateType::kNmPncReadySleep;
}

/// @brief Node detection.
/// @param etherIp NIC IP
/// @param callBack Callback method
void PartialNetworkOper::RequestDetectNode(ara::core::String const &etherIp,
                                           DetectNodeCallBack const &callBack) noexcept
{
    EtherIterator const etherPair{etherSMPtrVec_.find(etherIp)};
    if (etherSMPtrVec_.end() != etherPair) {
        etherPair->second->RequestDetectNode("", pnId_, callBack);
    }
}

/// @brief Passive wakeup.
/// @param etherIp NIC IP
void PartialNetworkOper::NotifyWakeup(ara::core::String const &etherIp) noexcept
{
    EtherIterator const etherPair{etherSMPtrVec_.find(etherIp)};
    if (etherSMPtrVec_.end() != etherPair) {
        etherPair->second->NotifyWakeup();
    }
}

/// @brief Set NIC state machine state.
/// @param etherIp NIC IP
/// @param newState New state machine state
void PartialNetworkOper::SetEtherStateMachineSate(ara::core::String const &etherIp,
                                                  EthernetStateMachineStateType const newState) noexcept
{
    EtherIterator const etherPair{etherSMPtrVec_.find(etherIp)};
    if (etherSMPtrVec_.end() != etherPair) {
        etherPair->second->SetEtherStateMachineSate(newState);
    }
}

/// @brief Get NIC state machine state.
/// @param etherIp NIC IP
/// @returns State machine state
EthernetStateMachineStateType PartialNetworkOper::GetEtherStateMachineState(
    ara::core::String const &etherIp) const noexcept
{
    EthernetStateMachineStateType state{EthernetStateMachineStateType::kStateBusSleep};
    ConstEthIterator const etherPair{etherSMPtrVec_.find(etherIp)};
    if (etherSMPtrVec_.end() != etherPair) {
        state = etherPair->second->GetEtherStateMachineState();
    }
    return state;
}

/// @brief Set NM message send/receive control
/// @param etherIp NIC IP
/// @param ctrType Send/receive control
void PartialNetworkOper::SetMessageCtrlType(ara::core::String const &etherIp,
                                            ara::nm::MessageCtrlType const &ctrType) noexcept
{
    EtherIterator const etherPair{etherSMPtrVec_.find(etherIp)};
    if (etherSMPtrVec_.end() != etherPair) {
        etherPair->second->SetMessageCtrlType(ctrType);
    }
}

/// @brief Get NM message send/receive control status
/// @param etherIp NIC IP
/// @returns  Send/receive control status
ara::nm::MessageCtrlType PartialNetworkOper::GetMessageCtrlType(ara::core::String const &etherIp) noexcept
{
    ara::nm::MessageCtrlType ctrType{ara::nm::MessageCtrlType::kEnableAll};
    EtherIterator const etherPair{etherSMPtrVec_.find(etherIp)};
    if (etherSMPtrVec_.end() != etherPair) {
        ctrType = etherPair->second->GetMessageCtrlType();
    }
    return ctrType;
}

/// @brief Get initialization status
/// @returns Initialization status
std::uint32_t PartialNetworkOper::GetInitState() const noexcept
{
    std::uint32_t initCode{0U};
    for (const auto &etherPtr : etherSMPtrVec_) {
        if (true == etherPtr.second->HasDtcError()) {
            NmLogger().LogDebug() << "PartialNetworkOper HasDtcError " << etherPtr.first.c_str();
            initCode = kNmEtherInitErr;
            break;
        }
    }
    return initCode;
}

/// @brief add the pointer of ethernet state machine object to partial network.
/// @param ethernetPtr pointer of ethernet state machine.
void PartialNetworkOper::AddNMEtherStateMachinePtr(std::shared_ptr< NMEtherStateMachine > const &ethernetPtr) noexcept
{
    if (nullptr != ethernetPtr) {
        etherSMPtrVec_[ethernetPtr->GetIpAddress()] = (ethernetPtr);
        ethernetPtr->RegistModeObserverHandler(
            [this](NetworkStateType const currentMode) noexcept -> void { _notifyEtherModeChanged(currentMode); });
        ethernetPtr->RegistPnObserverHandler(pnId_, [this]() noexcept -> void { _updatePnMsg(); });

        /// Currently, PN is first added to LN, and then PN adds ether. PN passes LN's NotifyEtherStateChanged to ether.
        /// If this order is adjusted later, modifications need to be considered here!!!!
        for (ara::core::Vector< EtherStateChangeHandler >::iterator it{lnWatchEtherStateGroup_.begin()};
             lnWatchEtherStateGroup_.end() != it; ++it) {
            ethernetPtr->RegistStateObserverHandler(*it);
        }

        ethernetPtr->RegistPnRequestObserverHandler(
            [this](ara::core::String const &etherIp, ara::core::String const &pnList) noexcept -> void {
                NotifyPassivePnRequest(etherIp, pnList);
            });

        ethernetPtr->RegistPnReleaseObserverHandler(
            [this](ara::core::String const &etherIp, ara::core::String const &pnList) noexcept -> void {
                NotifyPassivePnRelease(etherIp, pnList);
            });
    }
}

/// @brief PN request notification.
/// @param etherIp NIC IP.
/// @param pnList PN set.
void PartialNetworkOper::NotifyPassivePnRequest(ara::core::String const &etherIp,
                                                ara::core::String const &pnList) noexcept
{
    for (ara::core::Vector< PnInformHandler >::iterator it{pnRequestObservGroup_.begin()};
         pnRequestObservGroup_.end() != it; ++it) {
        (*it)(etherIp, pnList);
    }
}

/// @brief PN release notification.
/// @param etherIp NIC IP.
/// @param pnList PN set.
void PartialNetworkOper::NotifyPassivePnRelease(ara::core::String const &etherIp,
                                                ara::core::String const &pnList) noexcept
{
    for (ara::core::Vector< PnInformHandler >::iterator it{pnReleaseObservGroup_.begin()};
         pnReleaseObservGroup_.end() != it; ++it) {
        (*it)(etherIp, pnList);
    }
}

/// @brief Get passive PN list
/// @param etherIp NIC IP
/// @returns Passive PN list
ara::core::String PartialNetworkOper::GetPassivePnRequestList(ara::core::String const &etherIp) noexcept
{
    EtherIterator const etherPair{etherSMPtrVec_.find(etherIp)};
    ara::core::String pnList("");
    if (etherSMPtrVec_.end() != etherPair) {
        pnList = etherPair->second->GetPassivePnRequestList();
    }
    return pnList;
}
}  // namespace internal
}  // namespace nm
}  // namespace ara
