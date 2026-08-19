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
/// @file       udpnm.cpp
/// @brief      Internal object organization management
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=UdpNm
/// @unit_description=Internal object organization management
/// @module_path=/NetworkManager/nmm
/// @interface_level=/NetworkManager/nmm
/// @endcode
///
/// ================================================================

#include "include/udpnm.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "common/common.h"
#include "config/include/configure.h"
#include "utils/include/utils.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief udpnm initialization
/// @returns 0 ok
std::int32_t UdpNm::Open() noexcept
{
    ara::core::String const tmConf{(isoft::ara_fsh::Platform().GetPlatformEtcDir() + "/nm.json")};
    configure_ = std::make_shared< Configure >();
    std::int32_t retCode{configure_->Load(tmConf)};
    if (0 == retCode) {
        ara::core::String dtcError{"NM init error:"};
        bool bInitEtherError{false};
        ara::core::Vector< NmEthernetUdpNmNodeConfig > const &etherVec{configure_->GetAllEther()};
        ara::core::Vector< NmEthernetUdpNmNodeConfig >::const_iterator const etherVecEnd{std::cend(etherVec)};
        for (ara::core::Vector< NmEthernetUdpNmNodeConfig >::const_iterator it{std::cbegin(etherVec)};
             etherVecEnd != it; ++it) {
            stateMachineRes_[it->GetConnectorName()] = std::make_shared< NMEtherStateMachine >();
            if (nullptr == stateMachineRes_[it->GetConnectorName()]) {
                NmLogger().LogError() << "UdpNm::Open, fail to make_shared<NMEtherStateMachine> ";
                retCode = -1;
                break;
            }
            std::shared_ptr< NMEtherStateMachine > etherStateMachine{stateMachineRes_[it->GetConnectorName()]};
            std::int32_t const retCode1{etherStateMachine->Init(it->GetConnectorName(), configure_)};
            /// When starting AP-NM, the NIC is not ready yet. First ensure the process starts, then check the NIC when needed later
            if (0 != retCode1) {
                NmLogger().LogWarn() << "UdpNm::Open, failed to Init connectorName=" << it->GetConnectorName().c_str()
                                     << ", it may not exist or its not link-up";
                bInitEtherError = true;
                dtcError += it->GetiPv4Address() + ";";
                /// break;
            }
        }
        if (0 == retCode) {
            ara::core::Vector< NmLogicHandleConfig > const &allLogicRef{configure_->GetAllLoicHandle()};
            ara::core::Vector< NmLogicHandleConfig >::const_iterator const allLogicRefEnd{std::cend(allLogicRef)};
            for (ara::core::Vector< NmLogicHandleConfig >::const_iterator itLn{std::cbegin(allLogicRef)};
                 allLogicRefEnd != itLn; ++itLn) {
                ara::core::String const lnHandleIndex{itLn->GetHandleIndex()};
                logicHandleMap_[lnHandleIndex] = std::make_shared< LogicNetworkOper >();
                if (nullptr == logicHandleMap_[lnHandleIndex]) {
                    NmLogger().LogError() << "UdpNm::Open, LogicNetworkOper fail";
                    retCode = -1;
                    break;
                }

                ara::core::Vector< PnReferenceConfig > const &pnRef{itLn->GetPncRefVec()};

                ara::core::Vector< PnReferenceConfig >::const_iterator const pnRefEnd{std::cend(pnRef)};
                for (ara::core::Vector< PnReferenceConfig >::const_iterator pnRefIt{std::cbegin(pnRef)};
                     pnRefEnd != pnRefIt; ++pnRefIt) {
                    ara::core::String const pnHandleIndex{pnRefIt->GetPnIndex()};
                    if (nullptr == partialNetworkMap_[pnHandleIndex]) {
                        partialNetworkMap_[pnHandleIndex] = std::make_shared< PartialNetworkOper >();
                    }
                    std::shared_ptr< PartialNetworkOper > const partialNetworkStatus{partialNetworkMap_[pnHandleIndex]};
                    logicHandleMap_[lnHandleIndex]->AddPartialNetworkStatusPtr(partialNetworkStatus);
                }
                ara::core::Vector< EtherReferenceConfig > const &ethernetVecRef{itLn->GetEtherRefVec()};

                ara::core::Vector< EtherReferenceConfig >::const_iterator const ethVecEnd{std::cend(ethernetVecRef)};
                for (ara::core::Vector< EtherReferenceConfig >::const_iterator ethernetRefIt{
                         std::cbegin(ethernetVecRef)};
                     ethVecEnd != ethernetRefIt; ++ethernetRefIt) {
                    ara::core::String const ethernetName{ethernetRefIt->GetEherName()};
                    std::shared_ptr< NMEtherStateMachine > const etherStateMachine{stateMachineRes_[ethernetName]};
                    logicHandleMap_[lnHandleIndex]->AddNMEtherStateMachinePtr(etherStateMachine);
                }
#ifdef ARA_NM_WITH_COM
                if (!itLn->GetLnInstanceSpecifier().empty()) {
                    if (!logicHandleMap_[lnHandleIndex]->Init(itLn->GetLnInstanceSpecifier(), lnHandleIndex)) {
                        NmLogger().LogError() << "UdpNm::Open , init error lnHandleIndex=" << lnHandleIndex.c_str();
                        return -1;
                    }
                }
#endif
                if (!logicHandleMap_[lnHandleIndex]->InitIpc(lnHandleIndex)) {
                    NmLogger().LogError() << "UdpNm::Open , InitIpc error lnHandleIndex=" << lnHandleIndex.c_str();
                    return -1;
                }
            }
            ara::core::Vector< NmPnHandleConfig > const &allPnRef{configure_->GetAllPn()};
            ara::core::Vector< NmPnHandleConfig >::const_iterator const allPnEnd{std::cend(allPnRef)};
            for (ara::core::Vector< NmPnHandleConfig >::const_iterator pnItemIt{std::cbegin(allPnRef)};
                 allPnEnd != pnItemIt; ++pnItemIt) {
                ara::core::String const pnHandleIndex{pnItemIt->GetPnIndex()};
                std::shared_ptr< PartialNetworkOper > const partialNetworkStatus{partialNetworkMap_[pnHandleIndex]};
                if (nullptr == partialNetworkStatus) {
                    continue;
                }
                if (0U >= partialNetworkStatus->GetLogicNetworkCount()) {
                    continue;
                }
                static_cast< void >(partialNetworkStatus->Init(pnItemIt->GetPnId(), pnHandleIndex,
                                                               configure_->GetDefaultNmPnResetTime(),
                                                               configure_->GetDefaultPncPrepareSleepTime()));

                ara::core::Vector< EtherReferenceConfig > const &pnEthernetVecRef{pnItemIt->GetEthernetVec()};

                ara::core::Vector< EtherReferenceConfig >::const_iterator const ethEnd{std::cend(pnEthernetVecRef)};
                for (ara::core::Vector< EtherReferenceConfig >::const_iterator ethRefIt{std::cbegin(pnEthernetVecRef)};
                     ethEnd != ethRefIt; ++ethRefIt) {
                    std::shared_ptr< NMEtherStateMachine > const etherStateMachine{
                        stateMachineRes_[ethRefIt->GetEherName()]};
                    partialNetworkStatus->AddNMEtherStateMachinePtr(etherStateMachine);
                }
            }
        }
        bInitEtherError_ = bInitEtherError;
        dtcError_        = dtcError;
    }
#if ARA_NM_WITH_DIAG
    /// DTC reporting will block the event loop and wait for a return, so it cannot be placed directly in the timer; it needs to be executed in a thread
    if (HasDtcRecord()) {
        std::ignore = isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
            dtctimer_, kDTCTimerDelay,
            [this]() noexcept -> void { pthread_ = std::make_shared< std::thread >(&UdpNm::_writeDtcRecord, this); });
    }
#endif
    return retCode;
}

/// @brief udpnm close
void UdpNm::Close() noexcept
{
    NmLogger().LogDebug() << "UdpNm is stopping";
    for (ara::core::Map< ara::core::String, std::shared_ptr< LogicNetworkOper > >::iterator itbegin{
             logicHandleMap_.begin()};
         logicHandleMap_.end() != itbegin; ++itbegin) {
        itbegin->second->NotifyNMStop();
    }
    logicHandleMap_.clear();
    partialNetworkMap_.clear();
    stateMachineRes_.clear();
#if ARA_NM_WITH_DIAG
    if (nullptr != pDiagDtcServer_) {
        pDiagDtcServer_->NotifyStop();
    }
    dtctimer_ = nullptr;
    if (nullptr != pthread_) {
        if (true == pthread_->joinable()) {
            pthread_->join();
        }
        pthread_ = nullptr;
    }
    if (nullptr != pDiagDtcServer_) {
        pDiagDtcServer_ = nullptr;
    }
#endif
    configure_ = nullptr;
}

/// @brief If initialization problems occur, record diagnostic events
void UdpNm::_writeDtcRecord() noexcept  // NOLINT
{
#if ARA_NM_WITH_DIAG
    if (bInitEtherError_) {
        ara::diag::DataIdentifierReentrancyType reentrancy{};
        reentrancy.read      = ara::diag::ReentrancyType::kFully;
        reentrancy.readWrite = ara::diag::ReentrancyType::kFully;
        reentrancy.write     = ara::diag::ReentrancyType::kNot;
        ara::core::StringView didStringView{configure_->GetDidInstance().c_str()};
        ara::core::Result< ara::core::InstanceSpecifier > const didInst{
            ara::core::InstanceSpecifier::Create(std::move(didStringView))};
        ara::core::StringView monitorStringView{configure_->GetDiagMonitorId().c_str()};
        ara::core::Result< ara::core::InstanceSpecifier > const diagMonitorInst{
            ara::core::InstanceSpecifier::Create(std::move(monitorStringView))};
        ara::core::StringView cycleInstanceStringView{configure_->GetDiagOperCycleInstance().c_str()};
        ara::core::Result< ara::core::InstanceSpecifier > const diagCycleInstance{
            ara::core::InstanceSpecifier::Create(std::move(cycleInstanceStringView))};
        bool const difInstOK{didInst.HasValue()};
        bool const diagMonitorInstOK{diagMonitorInst.HasValue()};
        bool const cycleInstanceOK{diagCycleInstance.HasValue()};
        if (difInstOK && diagMonitorInstOK) {
            if (cycleInstanceOK) {
                pDiagDtcServer_
                    = std::make_shared< DiagDtcServer >(didInst.Value(), reentrancy, diagCycleInstance.Value());
                if (nullptr != pDiagDtcServer_) {
                    pDiagDtcServer_->Init(diagMonitorInst.Value());
                    std::ignore = pDiagDtcServer_->Offer();
                    pDiagDtcServer_->PushData(dtcError_);
                }
            } else {
                pDiagDtcServer_ = std::make_shared< DiagDtcServer >(didInst.Value(), reentrancy);
                if (nullptr != pDiagDtcServer_) {
                    pDiagDtcServer_->Init(diagMonitorInst.Value());
                    std::ignore = pDiagDtcServer_->Offer();
                    pDiagDtcServer_->PushData(dtcError_);
                }
            }
            std::ignore = reentrancy;  // just for qac
        } else {
            NmLogger().LogError() << "UdpNm diag InstanceSpecifier abnormal";
        }
    }
#endif
}

#if ARA_NM_DEBUG
/// @brief request logic network, for test
/// @param logicHandle logic network handle
/// @returns kNmOperOK ok
NmOperCode UdpNm::LogicNetworkRequest(ara::core::String const &logicHandle) noexcept
{
    logicHandleMap_[logicHandle]->SetLogicNetworkRequestedState(ara::nm::NetworkStateType::kFullCom);
    return NmOperCode::kNmOperOK;
}

/// @brief release logic network, for test
/// @param logicHandle logic network handle
/// @returns kNmOperOK ok
NmOperCode UdpNm::LogicNetworkRelease(ara::core::String const &logicHandle) noexcept
{
    logicHandleMap_[logicHandle]->SetLogicNetworkRequestedState(ara::nm::NetworkStateType::kNoCom);
    return NmOperCode::kNmOperOK;
}
#endif

}  // namespace internal
}  // namespace nm
}  // namespace ara
