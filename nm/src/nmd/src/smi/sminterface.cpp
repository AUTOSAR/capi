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
/// @file       sminterface.cpp
/// @brief      COM communication management class
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @interface_level=Module
/// @trace_id_sr=SRS_NM_00004,SRS_NM_00005,SRS_NM_00014,SRS_NM_00015,SRS_NM_00016,SRS_NM_00019,SRS_NM_00020,SRS_NM_00021,SRS_NM_00022,SRS_NM_00023,SRS_NM_00024
/// @unit_name=SMInterface
/// @unit_description=COM communication management class
/// @module_path=/NetworkManager/smi
/// @endcode
///
/// ================================================================

#include "include/sminterface.h"

#include "common/common.h"
#include "common/nmerrordomain.h"
#include "config/include/configure.h"
#include "utils/include/utils.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief Request to start node detection
/// @param ethIp NIC IP
/// @returns Setting result
ara::core::Future< void > SMInterface::RequestDetectNode(ara::core::String const &ethIp) noexcept
{
    NmLogger().LogDebug() << "SMInterface::RequestDetectNode ethIp:" << ethIp.c_str();
    ara::core::Promise< void > promise;
    ara::core::Future< void > future{promise.get_future()};
    if (nullptr != handler_.requestDetectNodeHandle) {
        handler_.requestDetectNodeHandle(ethIp, promise);
    } else {
        promise.SetError(static_cast< ara::core::ErrorCode >(NmErrc::kCantFindLNPointer));
    }
    return future;
}

/// @brief Set default IP. If the IP passed to the following interfaces is empty, the default IP will be used.
/// @param ethIp
/// @returns Setting result
ara::core::Future< void > SMInterface::SetDefaultEtherIp(ara::core::String const &ethIp) noexcept
{
    NmLogger().LogDebug() << "SMInterface::SetDefaultEtherIp ethIp:" << ethIp.c_str();
    ara::core::Promise< void > promise;
    ara::core::Future< void > future{promise.get_future()};
    if (nullptr != handler_.setDefaultEtherIpHandle) {
        handler_.setDefaultEtherIpHandle(ethIp, promise);
    } else {
        promise.SetError(static_cast< ara::core::ErrorCode >(NmErrc::kCantFindLNPointer));
    }
    return future;
}

/// @brief Set NIC state machine state
/// @param ethIp  NIC IP
/// @param ethState NIC state
/// @returns Setting result
ara::core::Future< void > SMInterface::SetEtherStateMachineSate(ara::core::String const &ethIp,
                                                                ara::nm::NetworkStateType const &ethState) noexcept
{
    NmLogger().LogDebug() << "SMInterface::SetEtherStateMachineSate ethIp:" << ethIp.c_str()
                          << ", ethState=" << static_cast< std::uint8_t >(ethState);
    ara::core::Promise< void > promise;
    ara::core::Future< void > future{promise.get_future()};
    if (nullptr != handler_.setEtherStateMachineSateHandle) {
        handler_.setEtherStateMachineSateHandle(ethIp, ethState, promise);
    } else {
        promise.SetError(static_cast< ara::core::ErrorCode >(NmErrc::kCantFindLNPointer));
    }
    return future;
}

/// @brief Get NIC state machine state
/// @param ethIp NIC IP
/// @returns NIC state machine state
ara::core::Future< NetworkState::GetEtherStateMachineStateOutput > SMInterface::GetEtherStateMachineState(
    ara::core::String const &ethIp) noexcept
{
    NmLogger().LogDebug() << "SMInterface::GetEtherStateMachineState ethIp:" << ethIp.c_str();
    ara::core::Promise< NetworkState::GetEtherStateMachineStateOutput > promise;
    ara::core::Future< NetworkState::GetEtherStateMachineStateOutput > future{promise.get_future()};
    if (nullptr != handler_.getEtherStateMachineStateHandler) {
        ara::nm::NetworkStateType type;
        handler_.getEtherStateMachineStateHandler(ethIp, type);
        NetworkState::GetEtherStateMachineStateOutput comOutput{};
        comOutput.ethState = type;
        promise.SetResult(comOutput);
    } else {
        promise.SetError(static_cast< ara::core::ErrorCode >(NmErrc::kCantFindLNPointer));
    }
    return future;
}

/// @brief NIC wakeup notification, accelerate sending the first NM message
/// @param ethIp NIC IP
/// @returns Setting result
ara::core::Future< void > SMInterface::NotifyWakeup(ara::core::String const &ethIp) noexcept
{
    NmLogger().LogDebug() << "SMInterface::NotifyWakeup ethIp:" << ethIp.c_str();
    ara::core::Promise< void > promise;
    ara::core::Future< void > future{promise.get_future()};
    if (nullptr != handler_.notifyWakeupHandle) {
        handler_.notifyWakeupHandle(ethIp, promise);
    } else {
        promise.SetError(static_cast< ara::core::ErrorCode >(NmErrc::kCantFindLNPointer));
    }
    return future;
}
/// @brief Node detection result notification
/// @param nodeList  Node set
void SMInterface::DetctNodesCallbak(ara::core::Vector< std::uint8_t > const &nodeList) noexcept
{
    NmLogger().LogDebug() << "SMInterface DetctNodesCallbak size:" << nodeList.size();
    // event send
    ara::com::SampleAllocateePtr< ByteArray > data{NotifyPresentNodeList.Allocate().Value()};
    ara::core::Vector< std::uint8_t >::const_iterator const itend{std::cend(nodeList)};
    for (ara::core::Vector< std::uint8_t >::const_iterator it{std::cbegin(nodeList)}; itend != it; ++it) {
        data->push_back(*it);
    }
    std::ignore = NotifyPresentNodeList.Send(std::move(data));
}

/// @brief NIC state change notification
/// @param etherIp  NIC IP
/// @param stateType  State machine state
void SMInterface::SendEtherStateChanged(ara::core::String const &etherIp,
                                        ara::nm::NetworkStateType const stateType) noexcept
{
    NmLogger().LogDebug() << "SMInterface SendEtherStateChanged etherIp:" << etherIp.c_str()
                          << ", stateType=" << static_cast< std::uint8_t >(stateType);
    // event send
    ara::com::SampleAllocateePtr< ara::nm::NMEtherStateChangedType > data{NotifyEtherStateChanged.Allocate().Value()};
    data->EthIp        = etherIp;
    data->currentState = stateType;
    std::ignore        = NotifyEtherStateChanged.Send(std::move(data));
}

/// @brief Initialize, register callback
/// @param handleIndex  Logical network name
/// @param handle    Callback method set
/// @returns true init ok, false fail.
bool SMInterface::InitAndOffer(ara::core::StringView const &handleIndex, SMIHandlers const &handle) noexcept
{
    handleIndex_ = handleIndex;
    handler_     = handle;

    static_cast< void >(
        NetworkCurrentState.RegisterGetHandler([this]() noexcept -> ara::core::Future< NetworkStateType > {
            ara::core::Promise< NetworkStateType > promise;
            if (true == this->terminate_) {
                this->logicNetworkCurrentState_ = NetworkStateType::kNoCom;
            }
            NmLogger().LogInfo() << "logicNetworkIndex_=" << handleIndex_
                                 << ", getNetworkCurrentState :logicNetworkCurrentState_="
                                 << static_cast< std::uint8_t >(this->logicNetworkCurrentState_);
            promise.set_value(this->logicNetworkCurrentState_);
            return promise.get_future();
        }));
    static_cast< void >(
        NetworkRequestedState.RegisterGetHandler([this]() noexcept -> ara::core::Future< NetworkStateType > {
            ara::core::Promise< NetworkStateType > promise;
            if (true == terminate_) {
                logicNetworkRequestedState_ = NetworkStateType::kNoCom;
            }
            NmLogger().LogInfo() << ",logicNetworkIndex_=" << handleIndex_
                                 << ", getNetworkRequestedState :logicNetworkRequestedState_="
                                 << static_cast< std::uint8_t >(logicNetworkRequestedState_);
            promise.set_value(logicNetworkRequestedState_);
            return promise.get_future();
        }));
    static_cast< void >(NetworkRequestedState.RegisterSetHandler(
        [this](NetworkStateType const requestState) noexcept -> ara::core::Future< NetworkStateType > {
            logicNetworkRequestedState_ = requestState;
            return handler_.setLNRequestStateHandler(requestState);
        }));
    static_cast< void >(NetworkCurrentState.Update(NetworkStateType::kNoCom));
    static_cast< void >(NetworkRequestedState.Update(NetworkStateType::kNoCom));
    ara::core::Result< void > const voidRes{OfferService()};
    if (!voidRes.HasValue()) {
        NmLogger().LogInfo() << "OfferService failed";
        return false;
    }
    NmLogger().LogInfo() << "logicNetworkIndex_=" << handleIndex_ << ",OfferService done ";
    return true;
}

/// @brief Update NetworkCurrentState
/// @param stateType Current state
void SMInterface::UpdateNetworkCurrentState(ara::nm::NetworkStateType const stateType) noexcept
{
    logicNetworkCurrentState_ = stateType;
    std::ignore               = NetworkCurrentState.Update(logicNetworkCurrentState_);
}

/// @brief notify the active logic network change to no-com when terminating nm
/// service
void SMInterface::NotifyNMStop() noexcept
{
    terminate_ = true;
    if (NetworkStateType::kFullCom == logicNetworkRequestedState_) {
        logicNetworkRequestedState_ = NetworkStateType::kNoCom;
        std::ignore                 = NetworkRequestedState.Update(logicNetworkRequestedState_);
    }
    if (NetworkStateType::kFullCom == logicNetworkCurrentState_) {
        logicNetworkCurrentState_ = NetworkStateType::kNoCom;
        std::ignore               = NetworkCurrentState.Update(logicNetworkCurrentState_);
    }
    StopOfferService();
}

/// @brief Set NM message send/receive control
/// @param ethIp  NIC IP
/// @param ctrType  Send/receive control
/// @returns Setting result
ara::core::Future< void > SMInterface::SetMessageCtrlType(ara::core::String const &ethIp,
                                                          ara::nm::MessageCtrlType const &ctrType) noexcept
{
    NmLogger().LogDebug() << "SMInterface::SetMessageCtrlType ethIp:" << ethIp.c_str()
                          << ", ctrType=" << static_cast< uint8_t >(ctrType);
    ara::core::Promise< void > promise;
    ara::core::Future< void > future{promise.get_future()};
    if (nullptr != handler_.setEtherNmMsgCtrlTypeHandler) {
        handler_.setEtherNmMsgCtrlTypeHandler(ethIp, ctrType, promise);
    } else {
        promise.SetError(static_cast< ara::core::ErrorCode >(NmErrc::kCantFindLNPointer));
    }
    return future;
}

/// @brief Get NM message send/receive control
/// @param ethIp  NIC IP
/// @returns NM message send/receive control
ara::core::Future< NetworkState::GetMessageCtrlTypeOutput > SMInterface::GetMessageCtrlType(
    ara::core::String const &ethIp) noexcept
{
    NmLogger().LogDebug() << "SMInterface::GetEtherStateMachineState ethIp:" << ethIp.c_str();
    ara::core::Promise< NetworkState::GetMessageCtrlTypeOutput > promise;
    ara::core::Future< NetworkState::GetMessageCtrlTypeOutput > future{promise.get_future()};
    if (nullptr != handler_.getEtherNmMsgCtrlTypeHandler) {
        ara::nm::MessageCtrlType ctrType{ara::nm::MessageCtrlType::kEnableAll};
        handler_.getEtherNmMsgCtrlTypeHandler(ethIp, ctrType);
        NetworkState::GetMessageCtrlTypeOutput comOutput{};
        comOutput.ctrType = ctrType;
        promise.SetResult(comOutput);
    } else {
        promise.SetError(static_cast< ara::core::ErrorCode >(NmErrc::kCantFindLNPointer));
    }
    return future;
}

/// @brief Get initialization status
/// @returns Initialization status
ara::core::Future< NetworkState::GetInitStateOutput > SMInterface::GetInitState() noexcept
{
    NmLogger().LogDebug() << "SMInterface::GetInitState";
    ara::core::Promise< NetworkState::GetInitStateOutput > promise;
    ara::core::Future< NetworkState::GetInitStateOutput > future{promise.get_future()};
    if (nullptr != handler_.getInitStateHandler) {
        std::uint32_t initCode{0U};
        handler_.getInitStateHandler(initCode);
        NetworkState::GetInitStateOutput comOutput{};
        comOutput.stateCode = initCode;
        promise.SetResult(comOutput);
    } else {
        promise.SetError(static_cast< ara::core::ErrorCode >(NmErrc::kCantFindLNPointer));
    }
    return future;
}

/// @brief PN request notification.
/// @param etherIp NIC IP.
/// @param pnList PN set.
void SMInterface::PassivePnRequest(ara::core::String const &etherIp, ara::core::String const &pnList) noexcept
{
    NmLogger().LogDebug() << "SMInterface::PassivePnRequest  etherIp=" << etherIp.c_str()
                          << ",pnList=" << pnList.c_str();
    // event send
    std::shared_ptr< ara::nm::PassivePNInform > data{std::move(NotifyPassivePnRequest.Allocate()).Value()};
    if (nullptr != data) {
        data->EthIp  = etherIp;
        data->PnList = pnList;
    }
    std::ignore = NotifyPassivePnRequest.Send(std::move(data));
}

/// @brief PN release notification.
/// @param etherIp NIC IP.
/// @param pnList PN set.
void SMInterface::PassivePnRelease(ara::core::String const &etherIp, ara::core::String const &pnList) noexcept
{
    NmLogger().LogDebug() << "SMInterface::PassivePnRelease  etherIp=" << etherIp.c_str()
                          << ",pnList=" << pnList.c_str();
    // event send
    std::shared_ptr< ara::nm::PassivePNInform > data{std::move(NotifyPassivePnRelease.Allocate()).Value()};
    if (nullptr != data) {
        data->EthIp  = etherIp;
        data->PnList = pnList;
    }
    std::ignore = NotifyPassivePnRelease.Send(std::move(data));
}

/// @brief Get passive PN list
/// @param ethIp  NIC IP
/// @returns Passive PN list
ara::core::Future< NetworkState::GetPassivePnRequestListOutput > SMInterface::GetPassivePnRequestList(
    ara::core::String const &ethIp) noexcept
{
    NmLogger().LogDebug() << "SMInterface::GetPassivePnRequestList ethIp:" << ethIp.c_str();
    ara::core::Promise< NetworkState::GetPassivePnRequestListOutput > promise;
    ara::core::Future< NetworkState::GetPassivePnRequestListOutput > future{promise.get_future()};
    if (nullptr != handler_.getEtherNmMsgCtrlTypeHandler) {
        ara::core::String pnList;
        handler_.getPassivePnListHandler(ethIp, pnList);
        NetworkState::GetPassivePnRequestListOutput comOutput;
        comOutput.pnList = pnList;
        promise.SetResult(comOutput);
    } else {
        promise.SetError(static_cast< ara::core::ErrorCode >(NmErrc::kCantFindLNPointer));
    }
    return future;
}
}  // namespace internal
}  // namespace nm
}  // namespace ara
