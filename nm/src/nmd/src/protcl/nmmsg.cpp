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
/// @file       nmmsg.cpp
/// @brief      NM message management
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @interface_level=Module
/// @trace_id_sr=SRS_NM_00001,SRS_NM_00002
/// @unit_name=NmMsg
/// @unit_description=NM message management
/// @module_path=/NetworkManager/protcl
/// @endcode
///
/// ================================================================

#include "include/nmmsg.h"

#include "utils/include/netcard.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief content initialization.
/// @param connectorName  ethernet communicator name
/// @param pConfigRefTmp      json configure reference
/// @returns               0 ok
std::int32_t NmMsg::Init(ara::core::String const &connectorName,
                         std::shared_ptr< Configure > const &pConfigRefTmp) noexcept
{
    connectorName_     = connectorName;
    pEtherUdpNmconfig_ = pConfigRefTmp->GetEtherConfig(connectorName);
    if (nullptr == pEtherUdpNmconfig_) {
        return -1;
    }
    pConfigRef_ = pConfigRefTmp;
    static_cast< void >(pConfigRefTmp->GetPnsOfEthernet(connectorName, configPnGroup_));
    if (nullptr == pEtherSocket_) {
        if (true == CheckNetCard(pEtherUdpNmconfig_->GetiPv4Address())) {
            pEtherSocket_ = std::make_shared< EtherSocket >();
            if (nullptr != pEtherSocket_) {
                std::int32_t const retCode{pEtherSocket_->OpenSocket(
                    pEtherUdpNmconfig_->GetiPv4Address(), pEtherUdpNmconfig_->GetIpv4MulticastIpAddress(),
                    pEtherUdpNmconfig_->GetUdpPort(), pEtherUdpNmconfig_->GetPduLength())};
                if (0 == retCode) {
                    pEtherSocket_->RegistMsgProceHandler(
                        [this](std::uint8_t const *const pMsgBuffer, std::uint32_t const msgLenth) noexcept -> void {
                            static_cast< void >(_recvNmMessage(pMsgBuffer, msgLenth));
                        });
                } else {
                    NmLogger().LogError() << "NmMsg::Init::OpenSocket fail";
                    pEtherSocket_ = nullptr;
                }
            }
        }
    }
    std::int32_t ret{-1};
    if (nullptr == pEtherSocket_) {
        ret = -1;
    } else {
        ret = 0;
    }
    return ret;
}

/// @brief destructor of NmMsg.
void NmMsg::DeInit() noexcept
{
    pEtherUdpNmconfig_ = nullptr;
    pSndBuffer_        = nullptr;
}
/// @brief send nm message.
/// @param detectNode Whether it is node detection
/// @param pnGroup   The set of partial networks currently in use by this node
/// @param userDataNMState   The value of NMState
/// @param activeWakeUpSet Whether to set the activeWakeUp bit in CBV
/// @returns kNmOperOK ok
std::int32_t NmMsg::SendNmMessage(bool const detectNode,
                                  ara::core::Vector< std::uint16_t > const &pnGroup,
                                  std::uint8_t const userDataNMState,
                                  bool const activeWakeUpSet) noexcept
{
    std::size_t const pduLen{pEtherUdpNmconfig_->GetPduLength()};
    std::size_t bufLen{pduLen};
    std::size_t startPnMinByte{0U};
    bufLen = bufLen + kSocketExtBuffer;
    if (nullptr == pSndBuffer_) {
        pSndBuffer_ = std::make_unique< std::uint8_t[] >(bufLen);
    }
    static_cast< void >(memset(pSndBuffer_.get(), 0, bufLen));
    detectNode_ = detectNode;

    if (pEtherUdpNmconfig_->GetNmCbvPosition() >= 0) {
        std::size_t const cbvPos{static_cast< std::uint32_t >(pEtherUdpNmconfig_->GetNmCbvPosition())};
        if (pEtherUdpNmconfig_->GetNmPncParticipation()) {
            pSndBuffer_[cbvPos] = kUdpNmPniBitSet;
        }
        if (detectNode) {
            pSndBuffer_[cbvPos] |= kUdpNmRMRBitSet;
        }
        if (pEtherUdpNmconfig_->GetNmSetActiveWakupBit() && activeWakeUpSet) {
            pSndBuffer_[cbvPos] |= kUdpNmActiveWakeBitSet;
        }
        startPnMinByte++;
    }
    if (pEtherUdpNmconfig_->GetNmNidPosition() >= 0) {
        std::size_t const nidPos{static_cast< std::uint32_t >(pEtherUdpNmconfig_->GetNmNidPosition())};
        pSndBuffer_[nidPos] = static_cast< std::uint8_t >(pEtherUdpNmconfig_->GetNmNid());
        startPnMinByte++;
    }
    ara::core::Vector< std::uint8_t > const &userData{pEtherUdpNmconfig_->GetUserData()};
    if ((false == userData.empty()) && (0 < pEtherUdpNmconfig_->GetNmUserDataLength())) {
        std::size_t const userDataLen{static_cast< std::uint32_t >(pEtherUdpNmconfig_->GetNmUserDataLength())};
        if (1U == userData.size()) {
            /// User only sets one byte; all user data is filled with the same value
            std::uint8_t const userData0{userData.at(0U)};
            std::int32_t const userVal{static_cast< std::int32_t >(userData0)};
            std::ignore = memset(pSndBuffer_.get() + pEtherUdpNmconfig_->GetNmUserDataOffset(), userVal, userDataLen);
        } else {
            /// Fill by byte
            if (userData.size() != userDataLen) {
                NmLogger().LogError() << "SendNmMessage, config error, nmUserDataLength=" << userDataLen
                                      << ", userDataFilled.length=" << userData.size();
            } else {
                std::ignore = memcpy(pSndBuffer_.get() + pEtherUdpNmconfig_->GetNmUserDataOffset(), userData.data(),
                                     userDataLen);
            }
        }
    }
    if (0 < pEtherUdpNmconfig_->GetNmUserDataLength()) {
        if (0 <= pEtherUdpNmconfig_->GetNmStateInUserDataOffset()) {
            std::int32_t const statePos{pEtherUdpNmconfig_->GetNmUserDataOffset()
                                        + pEtherUdpNmconfig_->GetNmStateInUserDataOffset()};
            pSndBuffer_[static_cast< std::size_t >(statePos)] = userDataNMState;
        }
    }
    ara::core::String requestPnStr;
    if (pEtherUdpNmconfig_->GetNmPncParticipation()) {
        ara::core::Vector< std::uint16_t >::const_iterator const itend{std::cend(pnGroup)};
        for (ara::core::Vector< std::uint16_t >::const_iterator it{std::cbegin(pnGroup)}; itend != it; ++it) {
            std::size_t byteIndex{*it};
            byteIndex = byteIndex / kNmConst8U;
            if (startPnMinByte > byteIndex) {
                NmLogger().LogError() << "SendNmMessage pnid is too small: " << *it;
                continue;
            }
            if (byteIndex < pduLen) {
                std::size_t val{*it};
                val = val % kNmConst8U;
                std::uint32_t changeVal{1U};
                changeVal = changeVal << val;
                std::uint8_t const pnbitVal{static_cast< std::uint8_t >(changeVal)};
                pSndBuffer_[byteIndex] |= pnbitVal;
                requestPnStr += std::to_string(static_cast< std::uint32_t >(*it)).c_str();
                requestPnStr += " ";
            }
        }
    }
    NmLogger().LogDebug() << "SendNmMessage," << connectorName_.c_str() << ", requestPnStr=" << requestPnStr.c_str();
    std::int32_t ret{0};
    if (nullptr == pEtherSocket_) {
        if (true == CheckNetCard(pEtherUdpNmconfig_->GetiPv4Address())) {
            pEtherSocket_ = std::make_shared< EtherSocket >();
            if (nullptr != pEtherSocket_) {
                ret = pEtherSocket_->OpenSocket(pEtherUdpNmconfig_->GetiPv4Address(),
                                                pEtherUdpNmconfig_->GetIpv4MulticastIpAddress(),
                                                pEtherUdpNmconfig_->GetUdpPort(), pEtherUdpNmconfig_->GetPduLength());
                if (0 == ret) {
                    pEtherSocket_->RegistMsgProceHandler(
                        [this](std::uint8_t const *const pMsgBuffer, std::uint32_t const msgLenth) noexcept -> void {
                            static_cast< void >(_recvNmMessage(pMsgBuffer, msgLenth));
                        });
                } else {
                    NmLogger().LogError() << "NmMsg::SendNmMessage, OpenSocket fail";
                    pEtherSocket_ = nullptr;
                }
            }
        }
    }
    if (nullptr != pEtherSocket_) {
        ret = pEtherSocket_->SendNmMessage(pSndBuffer_.get(), pduLen);
    }
    return ret;
}

/// @brief process of received nm message.
/// @param buffer message buffer
/// @param bufferLen buffer length
/// @returns kNmOperOK ok
NmOperCode NmMsg::_recvNmMessage(std::uint8_t const buffer[], std::uint32_t const bufferLen) noexcept
{
    NmLogger().LogDebug() << "_recvNmMessage, " << connectorName_.c_str() << ", bufferLen=" << bufferLen;

    if (pEtherUdpNmconfig_->GetPduLength() == bufferLen) {
        bool bValidQuest{false};
        ara::core::Vector< std::uint16_t > passiveValidPnGroup{};
        ara::core::String requestPnStr{};
        bool repeatMessageBitIndication{false};
        /// Return to ether nodeid, to be judged by the ether upper layer
        /// AAAAA nodeId
        std::uint8_t nodeId{0U};
        if (0 <= pEtherUdpNmconfig_->GetNmNidPosition()) {
            nodeId = buffer[pEtherUdpNmconfig_->GetNmNidPosition()];
        }

        if (0 <= pEtherUdpNmconfig_->GetNmCbvPosition()) {
            std::uint8_t const cbv{buffer[pEtherUdpNmconfig_->GetNmCbvPosition()]};
            repeatMessageBitIndication = ((cbv & kUdpNmRepeatMessageRequest) == kUdpNmRepeatMessageRequest);
            bool const pniBitSet{(cbv & kUdpNmPniBitSet) == kUdpNmPniBitSet};
            if (pniBitSet && pEtherUdpNmconfig_->GetNmPncParticipation()) {
                for (ara::core::Vector< std::uint16_t >::iterator it{configPnGroup_.begin()};
                     configPnGroup_.end() != it; ++it) {
                    std::uint16_t const configPnIndex{*it};
                    std::uint16_t const byteIndex{static_cast< std::uint16_t >(configPnIndex / 8U)};
                    std::uint8_t const bitIndex{static_cast< std::uint8_t >(configPnIndex % 8U)};
                    if (byteIndex >= static_cast< std::uint32_t >(pEtherUdpNmconfig_->GetPduLength())) {
                        continue;
                    }
                    std::uint32_t const pduLenth{static_cast< std::uint32_t >(byteIndex)};
                    if (pduLenth >= pEtherUdpNmconfig_->GetPduLength()) {
                        continue;
                    }
                    if (0U
                        != ((static_cast< uint8_t >(1U << static_cast< std::uint32_t >(bitIndex)))
                            & buffer[static_cast< std::size_t >(byteIndex)])) {
                        requestPnStr += std::to_string(static_cast< std::uint32_t >(configPnIndex)).c_str();
                        requestPnStr += " ";
                        ara::core::Vector< std::uint8_t > const &filterMask{pEtherUdpNmconfig_->GetPncFilterDataMask()};
                        if (!filterMask.empty()) {
                            std::uint32_t const pnIndex{configPnIndex};
                            std::uint32_t const pnByteSize{pnIndex / 8U + 1U};
                            if (filterMask.size() >= (pnByteSize)) {
                                std::uint32_t const pnByteIndex{pnIndex / 8U};
                                std::uint8_t const byteVal{filterMask[static_cast< size_t >(pnByteIndex)]};
                                std::uint32_t bitSet{1U};
                                std::size_t const bitsChange{static_cast< std::size_t >(pnIndex) % kNmConst8U};
                                bitSet = bitSet << bitsChange;
                                if (0U != (byteVal & (static_cast< std::uint8_t >(bitSet)))) {
                                    passiveValidPnGroup.push_back(configPnIndex);
                                    bValidQuest = true;
                                }
                            }
                        } else {
                            passiveValidPnGroup.push_back(configPnIndex);
                            bValidQuest = true;
                        }
                    }
                }
            }
        }
        if (pEtherUdpNmconfig_->GetAllNmMessagesKeepAwake()) {
            bValidQuest = true;
        }
        if ((true == detectNode_) && (0 <= pEtherUdpNmconfig_->GetNmNidPosition())) {
            bValidQuest = true;
        }
        if ((pEtherUdpNmconfig_->GetRrcPnCheck()) && (true == passiveValidPnGroup.empty())
            && (pEtherUdpNmconfig_->GetNmPncParticipation())) {
            repeatMessageBitIndication = false;
        }
        NmLogger().LogDebug() << "_recvNmMessage, " << connectorName_.c_str() << ", bValidQuest=" << bValidQuest
                              << ", requestPnStr=" << requestPnStr.c_str()
                              << ",repeatMessageRequestBit=" << repeatMessageBitIndication;
        if (repeatMessageBitIndication) {
            bValidQuest = true;
        }
        if (bValidQuest) {
            msgCallBack_(nodeId, repeatMessageBitIndication, passiveValidPnGroup);
        }
    }
    return NmOperCode::kNmOperOK;
}

}  // namespace internal
}  // namespace nm
}  // namespace ara
