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
/// @file       nmetherstatemachine.cpp
/// @brief      Ethernet state machine management class
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @trace_id_sr=SRS_NM_00001,SRS_NM_00002,SRS_NM_00006,SRS_NM_00007,SRS_NM_00008,SRS_NM_00009,SRS_NM_00010,SRS_NM_00011,SRS_NM_00012,SRS_NM_00013,SRS_NM_00015,SRS_NM_00018
/// @unit_name=NMEtherStateMachine
/// @unit_description=Ethernet state machine management class
/// @module_path=/NetworkManager/nmm
/// @interface_level=/NetworkManager/nmm
/// @endcode
///
/// ================================================================

#include "include/nmetherstatemachine.h"

#include "common/common.h"
#include "include/ethoper.h"
#include "smi/include/nmipcproc.h"
#ifdef ARA_NM_WITH_COM
    #include "smi/include/sminterface.h"
#endif
#include "utils/include/netcard.h"
#include "utils/include/utils.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief destructor of NMEtherStateMachine.
NMEtherStateMachine::~NMEtherStateMachine() noexcept
{
    pEtherUdpNmconfig_                = nullptr;
    mainLoop_                         = nullptr;
    networkTimerRef_                  = nullptr;
    nmMsgCycleOffsetTimerRef_         = nullptr;
    msgCycleTimerRef_                 = nullptr;
    repeateMessageTimerRef_           = nullptr;
    immediateNmTransmissionsTimerRef_ = nullptr;
    waitBusSleepTimerRef_             = nullptr;
    etherModObservGroup_.clear();
    pnObservGroup_.clear();
    if (nullptr != pNmMsg_) {
        pNmMsg_->DeInit();
        pNmMsg_ = nullptr;
    }
    pConfigRef_ = nullptr;
    NMIpcProc::CloseNmIpcProHandler();
}
/// @brief initialize the class.
/// @param connectorName  ethernet communicator name
/// @param pConfigRefTmp      json configure reference
/// @returns               0 ok
std::int32_t NMEtherStateMachine::Init(ara::core::String const &connectorName,
                                       std::shared_ptr< Configure > const &pConfigRefTmp) noexcept
{
    std::int32_t errcode{-1};

    if (!initConfig_) {
        connectorName_     = connectorName;
        pEtherUdpNmconfig_ = pConfigRefTmp->GetEtherConfig(connectorName);
        mainLoop_          = isoft::naicpp::GlobalGeneralEvLoop::Get();
        pConfigRef_        = pConfigRefTmp;
        static_cast< void >(pConfigRefTmp->GetPnsOfEthernet(connectorName, configPnGroup_));
        /// If there is a need to check and record DTC, check the NIC; otherwise, follow the original handling
        bool const bHaveMonitor{false == pConfigRefTmp->GetDiagMonitorId().empty()};
        bool const bHaveDid{false == pConfigRefTmp->GetDidInstance().empty()};
        bool const bHaveIfName{false == pEtherUdpNmconfig_->GetIfName().empty()};
        if ((true == bHaveMonitor) && (true == bHaveDid) && (true == bHaveIfName)) {
            NmLogger().LogDebug() << "NMEtherStateMachine, InitCheckNetCard ifName="
                                  << pEtherUdpNmconfig_->GetIfName().c_str();
            if (true == InitCheckNetCard(pEtherUdpNmconfig_->GetIfName())) {
                errcode = 0;
                static_cast< void >(_checkCardAndInitSocket());
            } else {
                dtcError_ = true;
            }
        } else {
            errcode = _checkCardAndInitSocket();
        }
        supportSleepMode_ = GetSupportBusSleepMode(pEtherUdpNmconfig_->GetiPv4Address());
        std::ignore       = _initIpcRegister();
    }
    return errcode;
}

/// @brief Check if the NIC exists and initialize the socket,
/// If the NIC is link-down, try to initialize the socket again on the next active request.
/// @returns  kNmOperOK ok
std::int32_t NMEtherStateMachine::_checkCardAndInitSocket() noexcept
{
    std::int32_t errcode{-1};
    if (true == CheckNetCard(pEtherUdpNmconfig_->GetiPv4Address())) {
        pNmMsg_ = std::make_shared< NmMsg >();
        if (nullptr != pNmMsg_) {
            std::int32_t const retCode{pNmMsg_->Init(connectorName_, pConfigRef_)};
            if (0 == retCode) {
                pNmMsg_->RegistMsgProceHandler(
                    [this](std::uint8_t const nodeId, bool const repeatMessageBitIndication,
                           ara::core::Vector< std::uint16_t > const &passiveValidPnGroup) noexcept -> void {
                        static_cast< void >(RecvNmMessage(nodeId, repeatMessageBitIndication, passiveValidPnGroup));
                    });
                errcode     = 0;
                initConfig_ = true;
            } else {
                NmLogger().LogError() << "NMEtherStateMachine, _checkCardAndInitSocket "
                                         "OpenSocket fail, connector_ip"
                                      << pEtherUdpNmconfig_->GetiPv4Address().c_str();
                pNmMsg_ = nullptr;
            }
        }
    } else {
        NmLogger().LogError() << "NMEtherStateMachine, _checkCardAndInitSocket fail, connector_ip"
                              << pEtherUdpNmconfig_->GetiPv4Address().c_str()
                              << ", it may not exist or its not link-up";
    }
    return errcode;
}
/// @brief request network.
/// @param requestEther true logic network request network, otherwise false
/// @param pnRequestInex partial network request network
/// @returns kNmOperOK ok
NmOperCode NMEtherStateMachine::RequestNetwork(bool const requestEther, std::uint16_t const pnRequestInex) noexcept
{
    NmOperCode errcode{NmOperCode::kNmOperError};
    NmLogger().LogDebug() << "NMEtherStateMachine::RequestNetwork, connectorName=" << connectorName_.c_str()
                          << ",requestEther=" << requestEther << ", pnRequestInex=" << pnRequestInex;
    if (!initConfig_) {
        std::int32_t const ret{_checkCardAndInitSocket()};
        if (0 != ret) {
            return errcode;
        }
    }

    bool bValid{false};
    if (requestEther) {
        requestEtherCount_ += 1U;
        bValid = true;
    }
    if (0U < pnRequestInex) {
        if (_hasConfigPn(pnRequestInex)) {
            if (_hasQuestPn(pnRequestInex)) {
                NmLogger().LogWarn() << "NMEtherStateMachine::RequestNetwork, already";
            } else {
                requestPnGroup_.emplace_back(pnRequestInex);
                bValid = true;
            }
        }
    }
    if (!bValid) {
        return NmOperCode::kNmOperError;
    }

    if (ESMModeType::kModeBusSleep == modeType_) {
        activeWakeUpNet_ = true;
        _busSleepToRepeateMessage();
    } else if (ESMModeType::kModePrepareBusSleep == modeType_) {
        activeWakeUpNet_ = true;
        _prepareBusSleepToRepeateMessage();
    } else if (ESMModeType::kModeNetwork == modeType_) {
        if (pEtherUdpNmconfig_->GetNmPnHandleMultipleNetworkRequests()) {
            _multipleNetworkRequestsForceToRepeateMessage();
        } else if (EthernetStateMachineStateType::kStateReadySleep == stateType_) {
            _readySleepToNormalOperation();
        } else {
        }
    } else {
    }
    errcode = NmOperCode::kNmOperOK;
    return errcode;
}

/// @brief release network.
/// @param requestEther true logic network request network, otherwise false
/// @param pnRequestInex partial network request network
/// @returns kNmOperOK ok
NmOperCode NMEtherStateMachine::ReleaseNetwork(bool const requestEther, std::uint16_t const pnRequestInex) noexcept
{
    NmLogger().LogDebug() << "NMEtherStateMachine::ReleaseNetwork enter, requestEther=" << requestEther
                          << ", pnRequestInex=" << pnRequestInex;
    NmOperCode errcode{NmOperCode::kNmOperError};

    if ((!requestEther) && (0U >= pnRequestInex)) {
        return errcode;
    }

    if (requestEther && (requestEtherCount_ > 0U)) {
        requestEtherCount_ -= 1U;
    }
    if (0U < pnRequestInex) {
        _eraseQuestPn(pnRequestInex);
    }

    if (!_checkActiveRequested()) {
        if ((ESMModeType::kModeNetwork == modeType_)
            && (EthernetStateMachineStateType::kStateNormalOperation == stateType_)) {
            _normalOperationToReadySleep();
        }
    }
    errcode = NmOperCode::kNmOperOK;
    return errcode;
}

/// @brief check the partial network has request network or not.
/// @param pnId partial network id
/// @returns true requested
bool NMEtherStateMachine::_hasQuestPn(std::uint16_t const pnId) noexcept
{
    bool bQuest{false};
    ara::core::Vector< std::uint16_t >::iterator const it{
        std::find(requestPnGroup_.begin(), requestPnGroup_.end(), pnId)};
    if (it != requestPnGroup_.end()) {
        bQuest = true;
    }
    return bQuest;
}
/// @brief check the partial network has request network or not.
/// @param pnId partial network id
/// @returns true requested
void NMEtherStateMachine::_eraseQuestPn(std::uint16_t const pnId) noexcept
{
    ara::core::Vector< std::uint16_t >::const_iterator const it{
        std::find(requestPnGroup_.cbegin(), requestPnGroup_.cend(), pnId)};
    if (it != requestPnGroup_.cend()) {
        std::ignore = requestPnGroup_.erase(it);
    }
}

/// @brief check the partial network has configed the ethernet communicator or
/// not.
/// @param pnId partial network
/// @returns true configed
bool NMEtherStateMachine::_hasConfigPn(std::uint16_t const pnId) noexcept
{
    for (ara::core::Vector< std::uint16_t >::iterator it{configPnGroup_.begin()}; configPnGroup_.end() != it; ++it) {
        if (pnId == *it) {
            return true;
        }
    }
    return false;
}

/// @brief from bus-sleep mode to repeate message state in network mode
void NMEtherStateMachine::_busSleepToRepeateMessage() noexcept
{
    ESMModeType const desMode{ESMModeType::kModeNetwork};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStateRepeatMessage};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);

    /// In repeat message state, NM messages must be sent. For passive nodes, PNI is empty
    /// Some customers require fast transmission in repeat message state

    if (_checkActiveRequested() || pEtherUdpNmconfig_->GetNmSendImmediateInRepeatMessage()) {
        if (0U < pEtherUdpNmconfig_->GetNmImmediateNmTransmissions()) {
            static_cast< void >(_sendNmMessage());
            nmImmediateNmTransmissionsLeft_ = pEtherUdpNmconfig_->GetNmImmediateNmTransmissions();
            if (0U < nmImmediateNmTransmissionsLeft_) {
                nmImmediateNmTransmissionsLeft_--;
            }
            if (0U < nmImmediateNmTransmissionsLeft_) {
                msgCycleTimerRef_.reset();
                _makeImmediateNmTransmissionsTimer();
            } else {
                _makeMsgCycleTimer();
            }
        } else if (kTimerMillSecond <= pEtherUdpNmconfig_->GetNmMsgCycleOffset()) {
            msgCycleTimerRef_.reset();
            _makeMsgCycleOffsetTimer();
        } else {
            static_cast< void >(_sendNmMessage());
            _makeMsgCycleTimer();
        }
    } else {
        if (kTimerMillSecond <= pEtherUdpNmconfig_->GetNmMsgCycleOffset()) {
            msgCycleTimerRef_.reset();
            _makeMsgCycleOffsetTimer();
        } else {
            static_cast< void >(_sendNmMessage());
            _makeMsgCycleTimer();
        }
    }

    _makeRepeateMessageTimer();
    _makeNetworkTimer();
}

/// @brief multi-network requests, force to enter repeate message in order to
/// send nm message immediately
void NMEtherStateMachine::_multipleNetworkRequestsForceToRepeateMessage() noexcept
{
    ESMModeType const desMode{ESMModeType::kModeNetwork};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStateRepeatMessage};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);

    if (_checkActiveRequested()) {
        if (0U < pEtherUdpNmconfig_->GetNmImmediateNmTransmissions()) {
            static_cast< void >(_sendNmMessage());
            nmImmediateNmTransmissionsLeft_ = pEtherUdpNmconfig_->GetNmImmediateNmTransmissions();
            if (0U < nmImmediateNmTransmissionsLeft_) {
                nmImmediateNmTransmissionsLeft_--;
            }
            if (0U < nmImmediateNmTransmissionsLeft_) {
                msgCycleTimerRef_.reset();
                _makeImmediateNmTransmissionsTimer();
            } else {
                _makeMsgCycleTimer();
            }
        } else if (kTimerMillSecond <= pEtherUdpNmconfig_->GetNmMsgCycleOffset()) {
            msgCycleTimerRef_.reset();
            _makeMsgCycleOffsetTimer();
        } else {
            static_cast< void >(_sendNmMessage());
            _makeMsgCycleTimer();
        }
    }

    _makeRepeateMessageTimer();
    _makeNetworkTimer();
}

/// @brief from prepare-bus-sleep mode to repeate message state in network mode
void NMEtherStateMachine::_prepareBusSleepToRepeateMessage() noexcept
{
    waitBusSleepTimerRef_.reset();
    ESMModeType const desMode{ESMModeType::kModeNetwork};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStateRepeatMessage};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);

    /// Some customers require fast transmission in all repeat message states; passive nodes do not have fast transmission
    if (_checkActiveRequested() || pEtherUdpNmconfig_->GetNmSendImmediateInRepeatMessage()) {
        if (0U < pEtherUdpNmconfig_->GetNmImmediateNmTransmissions()) {
            static_cast< void >(_sendNmMessage());
            nmImmediateNmTransmissionsLeft_ = pEtherUdpNmconfig_->GetNmImmediateNmTransmissions();
            if (0U < nmImmediateNmTransmissionsLeft_) {
                nmImmediateNmTransmissionsLeft_--;
            }
            if (0U < nmImmediateNmTransmissionsLeft_) {
                msgCycleTimerRef_.reset();
                _makeImmediateNmTransmissionsTimer();
            } else {
                _makeMsgCycleTimer();
            }
        } else if (kTimerMillSecond <= pEtherUdpNmconfig_->GetNmMsgCycleOffset()) {
            msgCycleTimerRef_.reset();
            _makeMsgCycleOffsetTimer();
        } else {
            static_cast< void >(_sendNmMessage());
            _makeMsgCycleTimer();
        }
    } else {
        if (kTimerMillSecond <= pEtherUdpNmconfig_->GetNmMsgCycleOffset()) {
            msgCycleTimerRef_.reset();
            _makeMsgCycleOffsetTimer();
        } else {
            static_cast< void >(_sendNmMessage());
            _makeMsgCycleTimer();
        }
    }

    _makeRepeateMessageTimer();
    _makeNetworkTimer();
}

/// @brief request network: from ready-sleep state in network mode to normal
/// operation state in network mode
void NMEtherStateMachine::_readySleepToNormalOperation() noexcept
{
    ESMModeType const desMode{ESMModeType::kModeNetwork};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStateNormalOperation};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);
    static_cast< void >(_sendNmMessage());

    _makeMsgCycleTimer();
    _makeNetworkTimer();
}

/// @brief request release: from normal operation in network mode to ready-sleep
/// state in network mode
void NMEtherStateMachine::_normalOperationToReadySleep() noexcept
{
    ESMModeType const desMode{ESMModeType::kModeNetwork};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStateReadySleep};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);
    msgCycleTimerRef_.reset();
}

/// @brief from prepare bus sleep mode to bus sleep mode
void NMEtherStateMachine::_prepareBusSleepToBusSleep() noexcept
{
    ESMModeType const desMode{ESMModeType::kModeBusSleep};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStateBusSleep};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);
    if (supportSleepMode_) {
        std::ignore = EnterBusSleepMode(pEtherUdpNmconfig_->GetiPv4Address());
    }
}

/// @brief process of repeate message timer
void NMEtherStateMachine::_repeateMessageTimerCalled() noexcept
{
    NmLogger().LogDebug() << " NMEtherStateMachine::RepeateMessageTimer expire," << connectorName_.c_str();
    if ((ESMModeType::kModeNetwork == modeType_)
        && (EthernetStateMachineStateType::kStateRepeatMessage == stateType_)) {
        if (_checkActiveRequested()) {
            _repeateMessageToNormalOperation();
        } else {
            _repeateMessageToReadySleep();
        }
    }
    _notifyDetectNodeList();
    repeateMessageTimerRef_.reset();
}

/// @brief repeate message timer expires, if request network, from repeate
/// message state in network mode to normal operation state in network mode
void NMEtherStateMachine::_repeateMessageToNormalOperation() noexcept
{
    ESMModeType const desMode{ESMModeType::kModeNetwork};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStateNormalOperation};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);
}

/// @brief repeate message timer expires, if request release, from repeate
/// message state in network mode to normal operation state in network mode
void NMEtherStateMachine::_repeateMessageToReadySleep() noexcept
{
    ESMModeType const desMode{ESMModeType::kModeNetwork};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStateReadySleep};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);
}

/// @brief process of wait bus sleep timer in prepare-bus-sleep mode
void NMEtherStateMachine::_waitBusSleepTimerCalled() noexcept
{
    NmLogger().LogDebug() << " NMEtherStateMachine::WaitBusSleepTimer expire," << connectorName_.c_str();
    if (ESMModeType::kModePrepareBusSleep == modeType_) {
        _prepareBusSleepToBusSleep();
    }
    waitBusSleepTimerRef_.reset();
}

/// @brief process of immediate nm transmissions timer in repeate message state
/// in network mode
void NMEtherStateMachine::_immediateNmTransmissionsTimerCalled() noexcept
{
    NmLogger().LogDebug() << " NMEtherStateMachine::ImmediateNmTransmissionsTimer "
                             "expire,nmImmediateNmTransmissionsLeft_="
                          << nmImmediateNmTransmissionsLeft_ << " " << connectorName_.c_str();
    /// In repeat message state, NM messages must be sent. For passive nodes, PNI is empty
    if (_checkActiveRequested() || pEtherUdpNmconfig_->GetNmSendImmediateInRepeatMessage()) {
        if ((EthernetStateMachineStateType::kStateRepeatMessage == stateType_)
            && (0U < nmImmediateNmTransmissionsLeft_)) {
            if (NmOperCode::kNmOperOK == _sendNmMessage()) {
                if (0 != networkTimerRef_->UpdateTime(TimeS2MS(pEtherUdpNmconfig_->GetNmNetworkTimeout()))) {
                    NmLogger().LogError()
                        << "connectorName_=" << connectorName_.c_str() << ", networkTimer UpdateTime error";
                }
            }
            nmImmediateNmTransmissionsLeft_--;
            if (0U < nmImmediateNmTransmissionsLeft_) {
                if (0
                    != immediateNmTransmissionsTimerRef_->UpdateTime(
                        TimeS2MS(pEtherUdpNmconfig_->GetNmImmediateNmCycleTime()))) {
                    NmLogger().LogError() << "connectorName_=" << connectorName_.c_str()
                                          << ", immediateNmTransmissionsTimer UpdateTime error";
                }
            } else {
                _makeMsgCycleTimer();
            }
        }
    }
}

/// @brief process of perodic nm-message transmission
///
/// @trace_id_sws=
void NMEtherStateMachine::_msgCycleOffsetTimerCalled() noexcept
{
    NmLogger().LogDebug() << " NMEtherStateMachine::MsgCycleOffsetTimer expire," << connectorName_.c_str();
    /// In repeat message state, NM messages must be sent. For passive nodes, PNI is empty
    {
        if (EthernetStateMachineStateType::kStateRepeatMessage == stateType_) {
            static_cast< void >(_sendNmMessage());
            _makeMsgCycleTimer();
        }
    }
}

/// @brief process of perodic nm-message transmission
///
/// @trace_id_sws=
void NMEtherStateMachine::_msgCycleTimerCalled() noexcept
{
    NmLogger().LogDebug() << " NMEtherStateMachine::MsgCycleTimer expire," << connectorName_.c_str();
    /// In repeat message state, NM messages must be sent. For passive nodes, PNI is empty
    {
        if ((EthernetStateMachineStateType::kStateNormalOperation == stateType_)
            || (EthernetStateMachineStateType::kStateRepeatMessage == stateType_)) {
            static_cast< void >(_sendNmMessage());
            if (0 != msgCycleTimerRef_->UpdateTime(TimeS2MS(pEtherUdpNmconfig_->GetNmMsgCycleTime()))) {
                NmLogger().LogError() << "connectorName_=" << connectorName_.c_str()
                                      << ", msgCycleTimer UpdateTime error";
            }
        }
    }
}

/// @brief process of nm-message timeout
void NMEtherStateMachine::_networkTimerCalled() noexcept
{
    NmLogger().LogDebug() << "NMEtherStateMachine::NetworkTimer expire, " << connectorName_.c_str();
    if (ESMModeType::kModeNetwork == modeType_) {
        if (EthernetStateMachineStateType::kStateRepeatMessage == stateType_) {
            if (0 != networkTimerRef_->UpdateTime(TimeS2MS(pEtherUdpNmconfig_->GetNmNetworkTimeout()))) {
                NmLogger().LogError() << "connectorName_=" << connectorName_.c_str()
                                      << ", networkTimer UpdateTime error";
            }
        } else if (EthernetStateMachineStateType::kStateNormalOperation == stateType_) {
            /// According to SWS, actively request the network. Only after repeat_message timeout will it enter normal_operation.
            /// Continue sending messages to keep the network alive. Restart the timer upon nmNetworkTimeout timeout
            /// Force set state to normal_operation. If the network has already been requested, it is consistent with SWS;
            /// otherwise, it is ReadySleep. If no packet is received before nmNetworkTimeout timeout, enter prepare_bus_sleep
            if (_checkActiveRequested()) {
                if (0 != networkTimerRef_->UpdateTime(TimeS2MS(pEtherUdpNmconfig_->GetNmNetworkTimeout()))) {
                    NmLogger().LogError()
                        << "connectorName_=" << connectorName_.c_str() << ", networkTimer UpdateTime error";
                }
            } else {
                _readySleepToPrepareBusSleep();
            }
        } else if (EthernetStateMachineStateType::kStateReadySleep == stateType_) {
            _readySleepToPrepareBusSleep();
        } else {
        }
    }
}

/// @brief nm-message timeout, from ready sleep state in network mode to prepare
/// bus sleep mode
void NMEtherStateMachine::_readySleepToPrepareBusSleep() noexcept
{
    ESMModeType const desMode{ESMModeType::kModePrepareBusSleep};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStatePrepareBusSleep};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);
    requestPnGroup_.clear();
    requestEtherCount_ = 0U;
    activeWakeUpNet_   = false;
    _makeWaitBusSleepTimer();
}

/// @brief from normal operation or ready sleep to repeate message
/// state(repeatMessageRequest)
void NMEtherStateMachine::_reptMSgReqToRepeateMessage() noexcept
{
    ESMModeType const desMode{ESMModeType::kModeNetwork};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStateRepeatMessage};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);

    /// In repeat message state, NM messages must be sent. For passive nodes, PNI is empty
    /// Some customers require fast transmission in repeat message state

    if (pEtherUdpNmconfig_->GetNmSendImmediateInRepeatMessage()) {
        if (0U < pEtherUdpNmconfig_->GetNmImmediateNmTransmissions()) {
            static_cast< void >(_sendNmMessage());
            nmImmediateNmTransmissionsLeft_ = pEtherUdpNmconfig_->GetNmImmediateNmTransmissions();
            if (0U < nmImmediateNmTransmissionsLeft_) {
                nmImmediateNmTransmissionsLeft_--;
            }
            if (0U < nmImmediateNmTransmissionsLeft_) {
                msgCycleTimerRef_.reset();
                _makeImmediateNmTransmissionsTimer();
            } else {
                _makeMsgCycleTimer();
            }
        } else if (kTimerMillSecond <= pEtherUdpNmconfig_->GetNmMsgCycleOffset()) {
            msgCycleTimerRef_.reset();
            _makeMsgCycleOffsetTimer();
        } else {
            static_cast< void >(_sendNmMessage());
            _makeMsgCycleTimer();
        }
    } else {
        if (kTimerMillSecond <= pEtherUdpNmconfig_->GetNmMsgCycleOffset()) {
            msgCycleTimerRef_.reset();
            _makeMsgCycleOffsetTimer();
        } else {
            static_cast< void >(_sendNmMessage());
            _makeMsgCycleTimer();
        }
    }
    _makeRepeateMessageTimer();
    _makeNetworkTimer();
}

/// @brief process of received nm message.
/// @param nodeId message buffer
/// @param repeatMessageBitIndication buffer length
/// @param passiveValidPnGroup buffer length
/// @returns kNmOperOK ok
NmOperCode NMEtherStateMachine::RecvNmMessage(std::uint8_t const nodeId,
                                              bool const repeatMessageBitIndication,
                                              ara::core::Vector< std::uint16_t > const &passiveValidPnGroup) noexcept
{
    NmLogger().LogDebug() << "RecvNmMessage, " << connectorName_.c_str() << " " << _getCurrentModeStateInfo().c_str()
                          << ",repeatMessageBitIndication=" << repeatMessageBitIndication;
    if (!initConfig_) {
        NmLogger().LogError() << "RecvNmMessage error not initconfig =" << connectorName_.c_str();
        return NmOperCode::kNmOperError;
    }
    if ((ara::nm::MessageCtrlType::kEnableAll != ctrType_) && (ara::nm::MessageCtrlType::kEnableReceive != ctrType_)) {
        NmLogger().LogWarn() << "RecvNmMessage,ctrType_=" << static_cast< std::uint8_t >(ctrType_)
                             << ", discard NM Message";
        return NmOperCode::kNmOperError;
    }
    if ((true == detectNode_) && (0 <= pEtherUdpNmconfig_->GetNmNidPosition())) {
        ara::core::Map< std::uint8_t, bool >::iterator const nodeIt{nodeList_.find(nodeId)};
        if (nodeList_.end() == nodeIt) {
            nodeList_[nodeId] = true;
        }
    }
    if (repeatMessageBitIndication) {
        if ((EthernetStateMachineStateType::kStateNormalOperation == stateType_)
            || (EthernetStateMachineStateType::kStateReadySleep == stateType_)) {
            /// Standard has no fast transmission at this time; some customers have fast transmission
            _reptMSgReqToRepeateMessage();
        } else {
        }
    } else {
        if (ESMModeType::kModeBusSleep == modeType_) {
            if (pEtherUdpNmconfig_->GetDropMsgInBusSleepMode()) {
                NmLogger().LogWarn() << "RecvNmMessage,kModeBusSleep GetDropMsgInBusSleepMode true, "
                                        "discard NM Message";
                return NmOperCode::kNmOperError;
            }
            _busSleepToRepeateMessage();
        } else if (ESMModeType::kModePrepareBusSleep == modeType_) {
            _prepareBusSleepToRepeateMessage();
        } else if (ESMModeType::kModeNetwork == modeType_) {
            if (0 != networkTimerRef_->UpdateTime(TimeS2MS(pEtherUdpNmconfig_->GetNmNetworkTimeout()))) {
                NmLogger().LogError() << "connectorName_=" << connectorName_.c_str()
                                      << ", networkTimer UpdateTime error";
            }
        } else {
        }
    }
    _updatePnMsg(passiveValidPnGroup);
    if (0 < pEtherUdpNmconfig_->GetPassivePnTimeout()) {
        _checkPassivePnRequest(passiveValidPnGroup);
        if (nullptr == passivePnCheckRef_) {
            static_cast< void >(NmMakeTimer(mainLoop_, passivePnCheckRef_,
                                            TimeS2MS(pEtherUdpNmconfig_->GetNmMsgCycleTime()),
                                            [this]() noexcept -> void { this->_passivePnCheckTimerCalled(); }));
        }
    }

    return NmOperCode::kNmOperOK;
}

/// @brief send nm message.
/// @returns kNmOperOK ok
NmOperCode NMEtherStateMachine::_sendNmMessage() noexcept
{
    if ((ara::nm::MessageCtrlType::kEnableAll != ctrType_) && (ara::nm::MessageCtrlType::kEnableSend != ctrType_)) {
        NmLogger().LogWarn() << "_sendNmMessage,ctrType_=" << static_cast< std::uint8_t >(ctrType_) << ", cannot send";
        return NmOperCode::kNmOperError;
    }
    std::int32_t ret{0};
    if (nullptr != pNmMsg_) {
        ret = pNmMsg_->SendNmMessage(detectNode_, requestPnGroup_, userDataNMState_, activeWakeUpNet_);
    }
    NmLogger().LogDebug() << "_sendNmMessage return ret=" << ret;
    NmOperCode retCode{NmOperCode::kNmOperError};
    if ((ret > 0) && (pEtherUdpNmconfig_->GetPduLength() == static_cast< std::uint32_t >(ret))) {
        _updatePnMsg(requestPnGroup_);
        _makeNetworkTimer();
        retCode = NmOperCode::kNmOperOK;
    } else {
        NmLogger().LogError() << "NMEtherStateMachine::_sendNmMessage fail";
        retCode = NmOperCode::kNmOperError;
    }
    return retCode;
}

/// @brief print state information for debug.
/// @param destMode mode type
/// @param destStateType state type
void NMEtherStateMachine::_printDebugInfo(ESMModeType const destMode,
                                          EthernetStateMachineStateType const destStateType) const noexcept
{
    ara::core::Map< ESMModeType, ara::core::String > mapMode;
    mapMode[ESMModeType::kModeBusSleep]        = ara::core::String("kModeBusSleep");
    mapMode[ESMModeType::kModePrepareBusSleep] = ara::core::String("kModePrepareBusSleep");
    mapMode[ESMModeType::kModeNetwork]         = ara::core::String("kModeNetwork");
    ara::core::Map< EthernetStateMachineStateType, ara::core::String > stateTypeMap;
    stateTypeMap[EthernetStateMachineStateType::kStateReadySleep]      = ara::core::String("kStateReadySleep");
    stateTypeMap[EthernetStateMachineStateType::kStateNormalOperation] = ara::core::String("kStateNormalOperation");
    stateTypeMap[EthernetStateMachineStateType::kStateRepeatMessage]   = ara::core::String("kStateRepeatMessage");
    ara::log::LogStream localLogDebug{NmLogger().LogDebug()};
    localLogDebug << connectorName_.c_str() << " change from " << mapMode[modeType_].c_str();
    if (ESMModeType::kModeNetwork == modeType_) {
        localLogDebug << ":" << stateTypeMap[stateType_].c_str();
    }
    localLogDebug << " to " << mapMode[destMode].c_str();
    if (ESMModeType::kModeNetwork == destMode) {
        localLogDebug << ":" << stateTypeMap[destStateType].c_str();
    }
}

/// @brief state information for debug.
/// @returns debug information
ara::core::String NMEtherStateMachine::_getCurrentModeStateInfo() const noexcept
{
    ara::core::Map< ESMModeType, ara::core::String > mapMode;
    mapMode[ESMModeType::kModeBusSleep]        = ara::core::String("kModeBusSleep");
    mapMode[ESMModeType::kModePrepareBusSleep] = ara::core::String("kModePrepareBusSleep");
    mapMode[ESMModeType::kModeNetwork]         = ara::core::String("kModeNetwork");
    ara::core::Map< EthernetStateMachineStateType, ara::core::String > stateTypeMap;
    stateTypeMap[EthernetStateMachineStateType::kStateReadySleep]      = ara::core::String("kStateReadySleep");
    stateTypeMap[EthernetStateMachineStateType::kStateNormalOperation] = ara::core::String("kStateNormalOperation");
    stateTypeMap[EthernetStateMachineStateType::kStateRepeatMessage]   = ara::core::String("kStateRepeatMessage");
    ara::core::String currentInfo{mapMode[modeType_]};
    if (ESMModeType::kModeNetwork == modeType_) {
        currentInfo += ":" + stateTypeMap[stateType_];
    }
    return currentInfo;
}

/// @brief in order to notify pn or ln the change of ethernet communication type
/// full-com no-com.
/// @param modeType mode type
void NMEtherStateMachine::_setMode(ESMModeType const modeType) noexcept
{
    if (modeType != modeType_) {
        modeType_ = modeType;
        NetworkStateType currentState{NetworkStateType::kNoCom};
        if (ESMModeType::kModeNetwork == modeType_) {
            currentState = NetworkStateType::kFullCom;
        }
        if (currentState != comState_) {
            comState_ = currentState;
            ara::core::Vector< EtherComModeHandler >::iterator const observerEnd{std::end(etherModObservGroup_)};
            for (ara::core::Vector< EtherComModeHandler >::iterator observerIt{std::begin(etherModObservGroup_)};
                 observerEnd != observerIt; ++observerIt) {
                (*observerIt)(currentState);
            }
        }
    }
}

/// @brief set state machine state type.
/// @param stateType state type
void NMEtherStateMachine::_setNetworkStateType(EthernetStateMachineStateType const stateType) noexcept
{
    if (stateType != stateType_) {
        _setuserDataNMState(stateType);
        stateType_ = stateType;
        for (ara::core::Vector< EtherStateChangeHandler >::iterator it{etherStateObservGroup_.begin()};
             etherStateObservGroup_.end() != it; ++it) {
            (*it)(pEtherUdpNmconfig_->GetiPv4Address(), stateType_);
        }
    }
}

/// @brief notify pn is active.
/// @param validPns pn group.
void NMEtherStateMachine::_updatePnMsg(ara::core::Vector< std::uint16_t > const &validPns) noexcept
{
    ara::core::Vector< std::uint16_t >::const_iterator const itEnd{std::end(validPns)};
    ara::core::Vector< std::uint16_t >::const_iterator const itBegin{std::begin(validPns)};
    static_cast< void >(std::for_each(itBegin, itEnd, [this](std::uint16_t const pnId) noexcept {
        ara::core::Map< std::uint16_t, PnActiveHandler >::iterator const pnPtr{this->pnObservGroup_.find(pnId)};
        if (pnPtr != this->pnObservGroup_.end()) {
            pnPtr->second();
        }
    }));
}

/// @brief Node detection. Node detection returns to repeat message, after detection, returns to original state
/// @param lnName LN name
/// @param pnId partial network id
/// @param callBack Result callback
void NMEtherStateMachine::RequestDetectNode(ara::core::String const &lnName,
                                            std::uint16_t const pnId,
                                            DetectNodeCallBack const &callBack) noexcept
{
    NmLogger().LogDebug() << "NMEtherStateMachine::RequestDetectNode, currentMode:"
                          << _getCurrentModeStateInfo().c_str();
    if (false == detectNode_) {
        nodeList_.clear();
        if (nullptr != callBack) {
            detctNodCall_.clear();
            detctNodCall_.emplace_back(callBack);
        }
        if (ESMModeType::kModeNetwork == modeType_) {
            detectNode_   = true;
            detectNodePn_ = pnId;
            detectNodeLn_ = lnName;
            _busSleepToRepeateMessage();
        } else {
            NmLogger().LogError() << "NMEtherStateMachine::DetectNode, wrong current mode";
        }
    }
}

/// @brief Low-level wakeup callback, passive wakeup
/// Need to accelerate sending the first frame, i.e., enter repeat message state upon packet reception
void NMEtherStateMachine::NotifyWakeup() noexcept
{
    NmLogger().LogDebug() << "NMEtherStateMachine::NotifyWakeup, wakeUp_:" << wakeUp_;
    if (!initConfig_) {
        std::int32_t const ret{_checkCardAndInitSocket()};
        if (0 != ret) {
            return;
        }
    }
    /// A wakeup may be received in any state of the state machine. Do not clear the network request flag.
    /// Just clear the timer, which is equivalent to restarting from repeat_message
    nmMsgCycleOffsetTimerRef_         = nullptr;
    msgCycleTimerRef_                 = nullptr;
    repeateMessageTimerRef_           = nullptr;
    immediateNmTransmissionsTimerRef_ = nullptr;
    waitBusSleepTimerRef_             = nullptr;

    _wakeupCallToRepeateMessage();
}

/// @brief Low-level wakeup callback, need to accelerate sending the first frame, i.e., enter repeat message state upon packet reception,
/// but do not clear the request list,
///   If the network is requested after repeat message state ends, enter normal_operation,
///   otherwise enter ready_sleep
void NMEtherStateMachine::_wakeupCallToRepeateMessage() noexcept
{
    ESMModeType const desMode{ESMModeType::kModeNetwork};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStateRepeatMessage};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);

    /// In repeat message state, NM messages must be sent. For passive nodes, PNI is empty
    /// Some customers require fast transmission in repeat message state

    if (_checkActiveRequested() || pEtherUdpNmconfig_->GetNmSendImmediateInRepeatMessage()) {
        if (0U < pEtherUdpNmconfig_->GetNmImmediateNmTransmissions()) {
            static_cast< void >(_sendNmMessage());
            nmImmediateNmTransmissionsLeft_ = pEtherUdpNmconfig_->GetNmImmediateNmTransmissions();
            if (0U < nmImmediateNmTransmissionsLeft_) {
                nmImmediateNmTransmissionsLeft_--;
            }
            if (0U < nmImmediateNmTransmissionsLeft_) {
                msgCycleTimerRef_.reset();
                _makeImmediateNmTransmissionsTimer();
            } else {
                _makeMsgCycleTimer();
            }
        } else if (kTimerMillSecond <= pEtherUdpNmconfig_->GetNmMsgCycleOffset()) {
            msgCycleTimerRef_.reset();
            _makeMsgCycleOffsetTimer();
        } else {
            static_cast< void >(_sendNmMessage());
            _makeMsgCycleTimer();
        }
    } else {
        if (kTimerMillSecond <= pEtherUdpNmconfig_->GetNmMsgCycleOffset()) {
            msgCycleTimerRef_.reset();
            _makeMsgCycleOffsetTimer();
        } else {
            static_cast< void >(_sendNmMessage());
            _makeMsgCycleTimer();
        }
    }

    _makeRepeateMessageTimer();
    _makeNetworkTimer();
}

/// @brief  Interface set state machine state (not supposed to be set normally)
/// @param desType State machine state
void NMEtherStateMachine::SetEtherStateMachineSate(EthernetStateMachineStateType const desType) noexcept
{
    NmLogger().LogDebug() << "NMEtherStateMachine::SetEtherStateMachineSate, desType:"
                          << static_cast< std::uint8_t >(desType);
    ara::core::Map< EthernetStateMachineStateType, ForceInterfaceCallback > functionMap;
    functionMap[EthernetStateMachineStateType::kStateBusSleep]
        = [this]() noexcept -> void { this->_forceBusSleepMode(); };
    functionMap[EthernetStateMachineStateType::kStatePrepareBusSleep]
        = [this]() noexcept -> void { this->_forcePrepareSleepMode(); };
    functionMap[EthernetStateMachineStateType::kStateReadySleep]
        = [this]() noexcept -> void { this->_forceReadySleep(); };
    functionMap[EthernetStateMachineStateType::kStateNormalOperation]
        = [this]() noexcept -> void { this->_forceNormalOperState(); };
    functionMap[EthernetStateMachineStateType::kStateRepeatMessage]
        = [this]() noexcept -> void { this->_forceRepeatMessageState(); };

    if (functionMap.end() != functionMap.find(desType)) {
        /// Request network normal and repeat_message cannot be deleted!!!!!
        ///  1. If the user has already requested the network, stay in normal_operation
        ///  2. If the user has not requested the network, stay in normal_operation for a period, then enter prepare_bus_sleep
        if ((EthernetStateMachineStateType::kStateNormalOperation != desType)
            && (EthernetStateMachineStateType::kStateRepeatMessage != desType)) {
            requestPnGroup_.clear();
            requestEtherCount_ = 0U;
        }

        nmMsgCycleOffsetTimerRef_         = nullptr;
        msgCycleTimerRef_                 = nullptr;
        repeateMessageTimerRef_           = nullptr;
        immediateNmTransmissionsTimerRef_ = nullptr;
        waitBusSleepTimerRef_             = nullptr;
        if ((EthernetStateMachineStateType::kStateReadySleep == desType)
            || (EthernetStateMachineStateType::kStateNormalOperation == desType)) {
            _makeNetworkTimer();
        } else {
            networkTimerRef_ = nullptr;
        }
        /// Timer settings for each state are slightly different; handled internally
        functionMap[desType]();
    }
}

/// @brief  Interface set state machine state: repeat message state (not supposed to be set normally).
void NMEtherStateMachine::_forceRepeatMessageState() noexcept
{
    NmLogger().LogDebug() << "NMEtherStateMachine::_forceRepeatMessageState";
    nmMsgCycleOffsetTimerRef_         = nullptr;
    msgCycleTimerRef_                 = nullptr;
    repeateMessageTimerRef_           = nullptr;
    immediateNmTransmissionsTimerRef_ = nullptr;
    waitBusSleepTimerRef_             = nullptr;
    networkTimerRef_                  = nullptr;

    /// Force repeat message. If fast transmission is configured, use fast transmission.
    /// After repeat message timeout, if the network is requested, jump to normal_operation, otherwise jump to ready_sleep
    ESMModeType const desMode{ESMModeType::kModeNetwork};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStateRepeatMessage};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);

    /// Some customers require fast transmission in repeat message state
    if (_checkActiveRequested() || pEtherUdpNmconfig_->GetNmSendImmediateInRepeatMessage()) {
        if (0U < pEtherUdpNmconfig_->GetNmImmediateNmTransmissions()) {
            static_cast< void >(_sendNmMessage());
            nmImmediateNmTransmissionsLeft_ = pEtherUdpNmconfig_->GetNmImmediateNmTransmissions();
            if (0U < nmImmediateNmTransmissionsLeft_) {
                nmImmediateNmTransmissionsLeft_--;
            }
            if (0U < nmImmediateNmTransmissionsLeft_) {
                msgCycleTimerRef_.reset();
                _makeImmediateNmTransmissionsTimer();
            } else {
                _makeMsgCycleTimer();
            }
        } else if (kTimerMillSecond <= pEtherUdpNmconfig_->GetNmMsgCycleOffset()) {
            msgCycleTimerRef_.reset();
            _makeMsgCycleOffsetTimer();
        } else {
            static_cast< void >(_sendNmMessage());
            _makeMsgCycleTimer();
        }
    } else {
        if (kTimerMillSecond <= pEtherUdpNmconfig_->GetNmMsgCycleOffset()) {
            msgCycleTimerRef_.reset();
            _makeMsgCycleOffsetTimer();
        } else {
            static_cast< void >(_sendNmMessage());
            _makeMsgCycleTimer();
        }
    }

    _makeRepeateMessageTimer();
    _makeNetworkTimer();
}

/// @brief  Interface set state machine state: normal operation (not supposed to be set normally).
void NMEtherStateMachine::_forceNormalOperState() noexcept
{
    NmLogger().LogDebug() << "NMEtherStateMachine::_forceNormalOperState";

    /// For those who have already requested, send periodically, always
    /// For those who have not requested the network, same as ready sleep
    ESMModeType const desMode{ESMModeType::kModeNetwork};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStateNormalOperation};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);

    nmMsgCycleOffsetTimerRef_         = nullptr;
    msgCycleTimerRef_                 = nullptr;
    repeateMessageTimerRef_           = nullptr;
    immediateNmTransmissionsTimerRef_ = nullptr;
    waitBusSleepTimerRef_             = nullptr;
    networkTimerRef_                  = nullptr;

    _makeNetworkTimer();

    /// It is possible that it is currently in repeat_message and there is no msgCycleTimerRef_ timer
    if ((nullptr == msgCycleTimerRef_) && _checkActiveRequested()) {
        _makeMsgCycleTimer();
    }
}

/// @brief  Interface set state machine state: sleep ready (not supposed to be set normally).
void NMEtherStateMachine::_forceReadySleep() noexcept
{
    NmLogger().LogDebug() << "NMEtherStateMachine::_forceReadySleep";

    nmMsgCycleOffsetTimerRef_         = nullptr;
    msgCycleTimerRef_                 = nullptr;
    repeateMessageTimerRef_           = nullptr;
    immediateNmTransmissionsTimerRef_ = nullptr;
    waitBusSleepTimerRef_             = nullptr;
    networkTimerRef_                  = nullptr;
    /// The timer may have already expired
    _makeNetworkTimer();

    _normalOperationToReadySleep();
}

/// @brief  Interface set state machine state: prepare bus sleep mode (not supposed to be set normally).
void NMEtherStateMachine::_forcePrepareSleepMode() noexcept
{
    NmLogger().LogDebug() << "NMEtherStateMachine::_forcePrepareSleepMode";
    nmMsgCycleOffsetTimerRef_         = nullptr;
    msgCycleTimerRef_                 = nullptr;
    repeateMessageTimerRef_           = nullptr;
    immediateNmTransmissionsTimerRef_ = nullptr;
    waitBusSleepTimerRef_             = nullptr;
    networkTimerRef_                  = nullptr;
    activeWakeUpNet_                  = false;

    /// Enter bus sleep upon timer timeout
    ESMModeType const desMode{ESMModeType::kModePrepareBusSleep};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStatePrepareBusSleep};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);
    requestPnGroup_.clear();
    requestEtherCount_ = 0U;
    _makeWaitBusSleepTimer();
}

/// @brief  Interface set state machine state: bus sleep mode (not supposed to be set normally).
void NMEtherStateMachine::_forceBusSleepMode() noexcept
{
    NmLogger().LogDebug() << "NMEtherStateMachine::_forceBusSleepMode";

    nmMsgCycleOffsetTimerRef_         = nullptr;
    msgCycleTimerRef_                 = nullptr;
    repeateMessageTimerRef_           = nullptr;
    immediateNmTransmissionsTimerRef_ = nullptr;
    waitBusSleepTimerRef_             = nullptr;
    networkTimerRef_                  = nullptr;
    activeWakeUpNet_                  = false;

    ESMModeType const desMode{ESMModeType::kModeBusSleep};
    EthernetStateMachineStateType const desStateType{EthernetStateMachineStateType::kStateBusSleep};
    _printDebugInfo(desMode, desStateType);
    _setMode(desMode);
    _setNetworkStateType(desStateType);
}

/// @brief Notify upper layer of node detection result
void NMEtherStateMachine::_notifyDetectNodeList() noexcept
{
    if (true == detectNode_) {
        ara::core::Vector< std::uint8_t > nodeVec;
        for (ara::core::Map< std::uint8_t, bool >::iterator it{nodeList_.begin()}; nodeList_.end() != it; ++it) {
            nodeVec.push_back(it->first);
        }

        for (ara::core::Vector< DetectNodeCallBack >::iterator it{detctNodCall_.begin()}; detctNodCall_.end() != it;
             ++it) {
            (*it)(nodeVec);
        }
        detectNode_ = false;
        nodeList_.clear();
    }
}

/// @brief Set NMState in user data.
/// @param destType  Target state to set
void NMEtherStateMachine::_setuserDataNMState(EthernetStateMachineStateType const destType) noexcept
{
    if (EthernetStateMachineStateType::kStateRepeatMessage == destType) {
        if (EthernetStateMachineStateType::kStateBusSleep == stateType_) {
            userDataNMState_ = pEtherUdpNmconfig_->GetNmstateBusSleep2Repeat();
        } else if (EthernetStateMachineStateType::kStatePrepareBusSleep == stateType_) {
            userDataNMState_ = pEtherUdpNmconfig_->GetNmstatePrepare2Repeat();
        } else if (EthernetStateMachineStateType::kStateReadySleep == stateType_) {
            userDataNMState_ = pEtherUdpNmconfig_->GetNmstateReadySlep2Repeat();
        } else if (EthernetStateMachineStateType::kStateNormalOperation == stateType_) {
            userDataNMState_ = pEtherUdpNmconfig_->GetNmstateNormal2Repeat();
        } else {
        }
    } else if (EthernetStateMachineStateType::kStateNormalOperation == destType) {
        if (EthernetStateMachineStateType::kStateReadySleep == stateType_) {
            userDataNMState_ = pEtherUdpNmconfig_->GetNmstateReadySlep2Normal();
        } else if (EthernetStateMachineStateType::kStateRepeatMessage == stateType_) {
            userDataNMState_ = pEtherUdpNmconfig_->GetNmstateRepeat2Normal();
        } else {
        }
    } else {
    }
}

/// @brief Set NM message send/receive control
/// @param ctrType Send/receive control
void NMEtherStateMachine::SetMessageCtrlType(ara::nm::MessageCtrlType const &ctrType) noexcept
{
    NmLogger().LogDebug() << "NMEtherStateMachine::SetMessageCtrlType, ctrType:"
                          << static_cast< std::uint8_t >(ctrType);
    ctrType_ = ctrType;
}

/// @brief Get initialization status
/// @returns Initialization status
bool NMEtherStateMachine::HasDtcError() const noexcept
{
    NmLogger().LogDebug() << "NMEtherStateMachine HasDtcError dtcError_=" << dtcError_;
    return dtcError_;
}
/// @brief check the ethernet communicator has been reqeust network proactively
/// by ln or pn.
/// @returns true reqeusted proactively
bool NMEtherStateMachine::_checkActiveRequested() const noexcept
{
    bool const reqByLn{requestEtherCount_ > 0U};
    bool const reqByPn{false == requestPnGroup_.empty()};
    return (reqByLn || reqByPn);
}
/// @brief First time receiving PN, while clearing the existing timeout count
/// @param pnGroup PN set
void NMEtherStateMachine::_checkPassivePnRequest(ara::core::Vector< std::uint16_t > const &pnGroup) noexcept
{
    ara::core::String newPnList;
    NmLogger().LogDebug() << "NMEtherStateMachine::_checkPassivePnRequest, pnGroup.size=" << pnGroup.size();
    for (ara::core::Vector< std::uint16_t >::const_iterator it{pnGroup.cbegin()}; pnGroup.cend() != it; ++it) {
        ara::core::Map< std::uint16_t, std::uint32_t >::iterator const recvit{passivePnGroup_.find(*it)};
        if (recvit != passivePnGroup_.end()) {
            NmLogger().LogDebug() << "NMEtherStateMachine::_checkPassivePnRequest find and reset, pn=" << *it;
            recvit->second = 0U;
        } else {
            NmLogger().LogDebug() << "NMEtherStateMachine:: _checkPassivePnRequest "
                                     "cant find and reset, pn="
                                  << *it;
            passivePnGroup_[*it] = 0U;
            if (newPnList.empty()) {
                newPnList = ara::core::to_string(static_cast< std::uint32_t >(*it));
            } else {
                newPnList
                    = newPnList + ara::core::String(",") + ara::core::to_string(static_cast< std::uint32_t >(*it));
            }
        }
    }
    /// new arrived and inform AA
    if (!newPnList.empty()) {
        for (ara::core::Vector< PnInformHandler >::iterator it{pnRequestObserver_.begin()};
             pnRequestObserver_.end() != it; ++it) {
            (*it)(pEtherUdpNmconfig_->GetiPv4Address(), newPnList);
        }
    }
}

/// @brief process of passive pn check timeout
void NMEtherStateMachine::_passivePnCheckTimerCalled() noexcept
{
    NmLogger().LogDebug() << "NMEtherStateMachine:: _passivePnCheckTimerCalled called";
    ara::core::Vector< std::uint16_t > relasePn;
    ara::core::String releasePnStr;

    for (ara::core::Map< std::uint16_t, std::uint32_t >::iterator it{passivePnGroup_.begin()};
         passivePnGroup_.end() != it; ++it) {
        it->second = it->second + 1U;
        if ((it->second * pEtherUdpNmconfig_->GetNmMsgCycleTime()) > pEtherUdpNmconfig_->GetPassivePnTimeout()) {
            relasePn.push_back(it->first);
        }
    }

    for (ara::core::Vector< std::uint16_t >::iterator it{relasePn.begin()}; relasePn.end() != it; ++it) {
        std::ignore = passivePnGroup_.erase(*it);
        if (releasePnStr.empty()) {
            releasePnStr = ara::core::to_string(static_cast< std::uint32_t >(*it));
        } else {
            releasePnStr
                = releasePnStr + ara::core::String(",") + ara::core::to_string(static_cast< std::uint32_t >(*it));
        }
    }
    if (!releasePnStr.empty()) {
        for (ara::core::Vector< PnInformHandler >::iterator it{pnReleaseObserver_.begin()};
             pnReleaseObserver_.end() != it; ++it) {
            (*it)(pEtherUdpNmconfig_->GetiPv4Address(), releasePnStr);
        }
    }
    NmLogger().LogDebug() << "NMEtherStateMachine:: _passivePnCheckTimerCalled "
                             "passivePnGroup_.size()="
                          << passivePnGroup_.size();
    if (false == passivePnGroup_.empty()) {
        std::ignore = passivePnCheckRef_->UpdateTime(TimeS2MS(pEtherUdpNmconfig_->GetNmMsgCycleTime()));
    } else {
        passivePnCheckRef_ = nullptr;
    }
}
/// @brief Get passive PN list
/// @returns Passive PN list
ara::core::String NMEtherStateMachine::GetPassivePnRequestList() const noexcept
{
    ara::core::String newPnList;
    for (ara::core::Map< std::uint16_t, std::uint32_t >::const_iterator it{passivePnGroup_.begin()};
         passivePnGroup_.end() != it; ++it) {
        if (newPnList.empty()) {
            newPnList = ara::core::to_string(static_cast< std::uint32_t >(it->first));
        } else {
            newPnList
                = newPnList + ara::core::String(",") + ara::core::to_string(static_cast< std::uint32_t >(it->first));
        }
    }
    return newPnList;
}

/// @brief IPC registration callback initialization.
/// @returns               0 ok
std::int32_t NMEtherStateMachine::_initIpcRegister() noexcept
{
    IpcHandlers callHandle{};
    DetectNodeCallBack const detectNodeCallback{
        [this](ara::core::Vector< std::uint8_t > const &nodeList) noexcept -> void {
            ara::core::String nodesStr{};
            for (ara::core::Vector< std::uint8_t >::const_iterator it{nodeList.begin()}; nodeList.end() != it; ++it) {
                if (nodesStr.empty()) {
                    nodesStr = ara::core::to_string(static_cast< std::uint32_t >(*it));
                } else {
                    nodesStr
                        = nodesStr + ara::core::String(",") + ara::core::to_string(static_cast< std::uint32_t >(*it));
                }
            }
            NMIpcProc::GetNmIpcProHandler()->NotifyPresentNodeList(pEtherUdpNmconfig_->GetiPv4Address(), nodesStr);
        }};
    IpcSetEtherHandler const requestDetectNodeHandle{[this, detectNodeCallback]() noexcept -> void {
        NmLogger().LogDebug() << "NMEtherStateMachine::_initIpcRegister "
                                 "requestDetectNode, currentMode:"
                              << _getCurrentModeStateInfo().c_str();
        if (false == detectNode_) {
            nodeList_.clear();
            {
                detctNodCall_.clear();
                detctNodCall_.emplace_back(detectNodeCallback);
            }
            if (ESMModeType::kModeNetwork == modeType_) {
                detectNode_ = true;
                _busSleepToRepeateMessage();
            } else {
                NmLogger().LogError() << "NMEtherStateMachine::_initIpcRegister "
                                         "DetectNode, wrong current mode";
            }
        }
    }};
    callHandle.requestDetectNodeHandle = requestDetectNodeHandle;
    IpcSetEtherHandler const wakeupCall{[this]() noexcept -> void { this->NotifyWakeup(); }};

    callHandle.notifyWakeupHandle = wakeupCall;

    IpcSetEtherSMSHandler const setEtherStateMachineSateHandle{
        [this](ara::nm::NetworkStateType const &ethState) noexcept -> void {
            EthernetStateMachineStateType const desType{GetInnerSMState(ethState)};
            this->SetEtherStateMachineSate(desType);
        }};
    callHandle.setEtherStateMachineSateHandle = setEtherStateMachineSateHandle;

    IpcGetEtherSMSHandler const getEtherStateMachineStateHandler{
        [this]() noexcept -> ara::nm::NetworkStateType { return GetComSMState(this->GetEtherStateMachineState()); }};
    callHandle.getEtherStateMachineStateHandler = getEtherStateMachineStateHandler;
    IpcSetNmMsgCtrlTypeHandler const setNmMsgCtrlTypeHandler{
        [this](ara::nm::MessageCtrlType const &ctrType) noexcept -> void { this->SetMessageCtrlType(ctrType); }};
    callHandle.setEtherNmMsgCtrlTypeHandler = setNmMsgCtrlTypeHandler;
    IpcGetNmMsgCtrlTypeHandler const getNmMsgCtrlTypeHandler{
        [this]() noexcept -> ara::nm::MessageCtrlType { return this->GetMessageCtrlType(); }};

    callHandle.getEtherNmMsgCtrlTypeHandler = getNmMsgCtrlTypeHandler;

    IpcGetInitStateHandler const getInitStateHandler{[this]() noexcept -> std::uint32_t {
        std::uint32_t initCode{0U};
        if (this->HasDtcError()) {
            initCode = 1U;
        }
        return initCode;
    }};
    callHandle.getInitStateHandler = getInitStateHandler;

    callHandle.getExternalPnListHandler
        = {[this]() noexcept -> ara::core::String { return this->GetPassivePnRequestList(); }};
    NMIpcProc::GetNmIpcProHandler()->RegEtherProcHandle(pEtherUdpNmconfig_->GetiPv4Address(), callHandle);

    RegistStateObserverHandler(
        [](ara::core::String const &etherIp, EthernetStateMachineStateType const &stateType) noexcept -> void {
            std::uint32_t const currentState{static_cast< std::uint32_t >(GetComSMState(stateType))};
            NMIpcProc::GetNmIpcProHandler()->NotifyEtherStateChanged(etherIp, currentState);
        });
    RegistPnRequestObserverHandler(
        [](ara::core::String const &etherIp, ara::core::String const &pnList) noexcept -> void {
            NMIpcProc::GetNmIpcProHandler()->NotifyExternalPnRequest(etherIp, pnList);
        });
    RegistPnReleaseObserverHandler(
        [](ara::core::String const &etherIp, ara::core::String const &pnList) noexcept -> void {
            NMIpcProc::GetNmIpcProHandler()->NotifyExternalPnRelease(etherIp, pnList);
        });
    return 0;
}

/// @brief Start ImmediateNmTransmissionsTimer.
void NMEtherStateMachine::_makeImmediateNmTransmissionsTimer() noexcept
{
    immediateNmTransmissionsTimerRef_.reset();
    std::int32_t const nmImmediateNmCycleTimeMs{TimeS2MS(pEtherUdpNmconfig_->GetNmImmediateNmCycleTime())};
    static_cast< void >(NmMakeTimer(mainLoop_, immediateNmTransmissionsTimerRef_, nmImmediateNmCycleTimeMs,
                                    [this]() noexcept -> void { this->_immediateNmTransmissionsTimerCalled(); }));
}

/// @brief Start MsgCycleTimer.
void NMEtherStateMachine::_makeMsgCycleTimer() noexcept
{
    msgCycleTimerRef_.reset();
    std::int32_t const nmMsgCycleTimeMs{TimeS2MS(pEtherUdpNmconfig_->GetNmMsgCycleTime())};
    static_cast< void >(NmMakeTimer(mainLoop_, msgCycleTimerRef_, nmMsgCycleTimeMs,
                                    [this]() noexcept -> void { this->_msgCycleTimerCalled(); }));
}

/// @brief Start MsgCycleOffsetTimer.
void NMEtherStateMachine::_makeMsgCycleOffsetTimer() noexcept
{
    nmMsgCycleOffsetTimerRef_.reset();
    std::int32_t const nmMsgCycleOffsetMs{TimeS2MS(pEtherUdpNmconfig_->GetNmMsgCycleOffset())};
    static_cast< void >(NmMakeTimer(mainLoop_, nmMsgCycleOffsetTimerRef_, nmMsgCycleOffsetMs,
                                    [this]() noexcept -> void { this->_msgCycleOffsetTimerCalled(); }));
}

/// @brief Start RepeateMessageTimer
void NMEtherStateMachine::_makeRepeateMessageTimer() noexcept
{
    repeateMessageTimerRef_.reset();
    std::int32_t const nmRepeatMessageTimeMs{TimeS2MS(pEtherUdpNmconfig_->GetNmRepeatMessageTime())};
    static_cast< void >(NmMakeTimer(mainLoop_, repeateMessageTimerRef_, nmRepeatMessageTimeMs,
                                    [this]() noexcept -> void { this->_repeateMessageTimerCalled(); }));
}

/// @brief Start NetworkTimer.
void NMEtherStateMachine::_makeNetworkTimer() noexcept
{
    networkTimerRef_.reset();
    std::int32_t const nmNetworkTimeoutMs{TimeS2MS(pEtherUdpNmconfig_->GetNmNetworkTimeout())};
    static_cast< void >(NmMakeTimer(mainLoop_, networkTimerRef_, nmNetworkTimeoutMs,
                                    [this]() noexcept -> void { this->_networkTimerCalled(); }));
}

/// @brief Start WaitBusSleepTimer.
void NMEtherStateMachine::_makeWaitBusSleepTimer() noexcept
{
    waitBusSleepTimerRef_.reset();
    std::int32_t const nmWaitBusSleepTimeMs{TimeS2MS(pEtherUdpNmconfig_->GetNmWaitBusSleepTime())};
    static_cast< void >(NmMakeTimer(mainLoop_, waitBusSleepTimerRef_, nmWaitBusSleepTimeMs,
                                    [this]() noexcept -> void { this->_waitBusSleepTimerCalled(); }));
}

}  // namespace internal
}  // namespace nm
}  // namespace ara
