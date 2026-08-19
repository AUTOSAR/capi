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
/// @file       nmethernetudpnmnodeconfig.h
/// @brief      Connector and UDP-NM related configuration
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/config
/// @interface_level=Module
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=NmEthernetUdpNmNodeConfig
/// @unit_description=Connector and UDP-NM related configuration
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_CONFIGURE_NMETHERUDPNMNODE_H_
#define _ARA_NM_CONFIGURE_NMETHERUDPNMNODE_H_
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/define.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include "common/common.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief NmEthernetUdpNmNodeConfig, EthernetCommunicationConnector config and
/// udp-cluster, udp-node config belongs to it
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100051
/// @trace_id_dd=DD_NM_00790
/// @needwork = ad
/// @endcode
class NmEthernetUdpNmNodeConfig final
{
public:
    /// @brief constructor of NmEthernetUdpNmNodeConfig.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00206
    /// @needwork = dda
    /// @endcode
    NmEthernetUdpNmNodeConfig() = default;

    /// @brief copy constructor is default.
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00207
    /// @needwork = dda
    /// @endcode
    NmEthernetUdpNmNodeConfig(NmEthernetUdpNmNodeConfig const &other) = default;

    /// @brief copy operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00208
    /// @needwork = dda
    /// @endcode
    NmEthernetUdpNmNodeConfig &operator=(NmEthernetUdpNmNodeConfig const &other) = default;

    /// @brief move constructor is default
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00209
    /// @needwork = dda
    /// @endcode
    NmEthernetUdpNmNodeConfig(NmEthernetUdpNmNodeConfig &&other) = default;

    /// @brief move operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00210
    /// @needwork = dda
    /// @endcode
    NmEthernetUdpNmNodeConfig &operator=(NmEthernetUdpNmNodeConfig &&other) = default;

    /// @brief Destructor of NmEthernetUdpNmNodeConfig.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00211
    /// @needwork = dda
    /// @endcode
    ~NmEthernetUdpNmNodeConfig() = default;

    /// @brief load NmEthernetUdpNmNodeConfig-configuration
    /// @param node NmEthernetUdpNmNodeConfig-configuration node
    /// @returns kSuccess ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00212
    /// @needwork = dda
    /// @endcode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        this->connectorName_
            = node.GetValue(std::move(ara::core::StringView("ethernetcommunicationconnector")), ara::core::String());
        this->mTUSize_ = node.GetValue(std::move(ara::core::StringView("maximumtransmissionunit")), 0);
        this->pncFilterDataMaskStr_
            = node.GetValue(std::move(ara::core::StringView("pncfilterdatamask")), ara::core::String());
        size_t const pnByteSize{static_cast< size_t >(pncFilterDataMaskStr_.size() / 8U + 1U)};
        this->pncFilterDataMask_.reserve(pnByteSize);
        for (size_t i{0U}; i < pncFilterDataMaskStr_.size(); i += sizeof(std::uint8_t)) {
            std::uint8_t byteVal{0U};
            bool bEnd{false};
            for (std::size_t j{0U}; j < sizeof(std::uint8_t) * kNmConst8U; j++) {
                if ((i * kNmConst8U + j) >= pncFilterDataMaskStr_.size()) {
                    bEnd = true;
                    break;
                }
                std::string const bitStr{
                    pncFilterDataMaskStr_.substr(pncFilterDataMaskStr_.size() - 1U - i * kNmConst8U - j, 1U).c_str()};
                std::size_t const bitVal{static_cast< std::size_t >(std::stoi(bitStr))};
                if (1U == bitVal) {
                    byteVal |= static_cast< std::uint8_t >(1U << j);
                }
            }
            this->pncFilterDataMask_.push_back(byteVal);
            if (true == bEnd) {
                break;
            }
        }
        this->iPv4Address_ = node.GetValue(std::move(ara::core::StringView("ipv4address")), ara::core::String());
        this->iPv4Mask_    = node.GetValue(std::move(ara::core::StringView("networkmask")), ara::core::String());
        this->udpNmClusterName_
            = node.GetValue(std::move(ara::core::StringView("udpnmclustername")), ara::core::String());
        this->nmPncParticipation_ = node.GetValue(std::move(ara::core::StringView("nmpncparticipation")), false);
        this->ipv4MulticastIpAddress_
            = node.GetValue(std::move(ara::core::StringView("ipv4multicastipaddress")), ara::core::String());
        this->udpPort_       = node.GetValue(std::move(ara::core::StringView("udpport")), 0U);
        this->nmCbvPosition_ = node.GetValue(std::move(ara::core::StringView("nmcbvposition")), -1);
        this->nmImmediateNmCycleTime_
            = node.GetValue(std::move(ara::core::StringView("nmimmediatenmcycletime")), kDoubleNegactiveOne);
        this->nmImmediateNmTransmissions_
            = node.GetValue(std::move(ara::core::StringView("nmimmediatenmtransmissions")), 0U);
        this->nmMsgCycleTime_ = node.GetValue(std::move(ara::core::StringView("nmmsgcycletime")), kDoubleNegactiveOne);
        this->nmNetworkTimeout_
            = node.GetValue(std::move(ara::core::StringView("nmnetworktimeout")), kDoubleNegactiveOne);
        this->nmNidPosition_ = node.GetValue(std::move(ara::core::StringView("nmnidposition")), -1);
        std::int16_t const defVal{-1};
        this->nmNid_ = node.GetValue(std::move(ara::core::StringView("nmnodeid")), defVal);
        this->nmRepeatMessageTime_
            = node.GetValue(std::move(ara::core::StringView("nmrepeatmessagetime")), kDoubleNegactiveOne);
        this->nmUserDataLength_ = node.GetValue(std::move(ara::core::StringView("nmuserdatalength")), -1);
        this->nmUserDataOffset_ = node.GetValue(std::move(ara::core::StringView("nmuserdataoffset")), -1);
        this->nmWaitBusSleepTime_
            = node.GetValue(std::move(ara::core::StringView("nmwaitbussleeptime")), kDoubleNegactiveOne);
        this->allNmMessagesKeepAwake_
            = node.GetValue(std::move(ara::core::StringView("allnmmessageskeepawake")), false);
        this->nmMsgCycleOffset_
            = node.GetValue(std::move(ara::core::StringView("nmmsgcycleoffset")), kDoubleNegactiveOne);
        this->nmPnHandleMultipleNetworkRequests_
            = node.GetValue(std::move(ara::core::StringView("nmpnhandlemultiplenetworkrequests")), false);
        this->userDataOriStr_ = node.GetValue(std::move(ara::core::StringView("userDataFilled")), ara::core::String());
        if (!this->userDataOriStr_.empty()) {
            for (size_t i{0U}; i < this->userDataOriStr_.length(); i += kNmConst2U) {
                ara::core::String const byteString{std::move(this->userDataOriStr_.substr(i, kNmConst2U).c_str())};
                std::uint8_t const byte{
                    static_cast< std::uint8_t >(ara::core::internal::stoul(byteString, nullptr, 16))};
                this->userData_.push_back(byte);
            }
        }
        this->nmSendImmediateInRepeatMessage_
            = node.GetValue(std::move(ara::core::StringView("nmSendImmediateInRepeatMessage")), false);
        this->nmSetActiveWakupBit_     = node.GetValue(std::move(ara::core::StringView("nmSetActiveWakupBit")), false);
        this->nmStateInUserDataOffset_ = node.GetValue(std::move(ara::core::StringView("nmStateInUserDataOffset")), -1);
        this->rrcPnCheck_              = node.GetValue(std::move(ara::core::StringView("rrcPnCheck")), false);

        this->nmstateBusSleep2Repeat_ = node.GetValue(std::move(ara::core::StringView("busSleepToRepeatMessageState")),
                                                      kNmStateBusSleep2RepeatMessage);
        this->nmstatePrepare2Repeat_
            = node.GetValue(std::move(ara::core::StringView("prepareBusSleepToRepeatMessageState")),
                            kNmStatePrepareBusSleep2RepeatMessage);
        this->nmstateRepeat2Normal_
            = node.GetValue(std::move(ara::core::StringView("repeatMessageStateToNormalOperationState")),
                            kNmStateRepeatMessage2NormalOperation);
        this->nmstateReadySlep2Normal_
            = node.GetValue(std::move(ara::core::StringView("readySleepStateToNormalOperationState")),
                            kNmStateReadySleep2NormalOperation);
        this->nmstateReadySlep2Repeat_ = node.GetValue(
            std::move(ara::core::StringView("readySleepStateToRepeatMessageState")), kNmStateReadySleep2RepeatMessage);
        this->nmstateNormal2Repeat_
            = node.GetValue(std::move(ara::core::StringView("normalOperationStateToRepeatMessageState")),
                            kNmStateNormalOperation2RepeatMessage);
        this->dropMsgInBusSleepMode_ = node.GetValue(std::move(ara::core::StringView("dropMsgInBusSleepMode")), false);
        this->passivePnTimeout_
            = node.GetValue(std::move(ara::core::StringView("passivePnTimeout")), kDoubleNegactiveOne);
        this->ifName_ = node.GetValue(std::move(ara::core::StringView("ifName")), ara::core::String());
        return isoft::kSuccess;
    }

#if ARA_NM_DEBUG
    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00213
    /// @needwork = dda
    /// @endcode
    void Debug() const noexcept
    {
        std::cout << "ethernetcommunicationconnector: " << connectorName_ << std::endl;
        std::cout << "maximumtransmissionunit: " << mTUSize_ << std::endl;
        std::cout << "pncfilterdatamask: " << pncFilterDataMaskStr_ << std::endl;
        std::cout << "ipv4address: " << iPv4Address_ << std::endl;
        std::cout << "networkmask: " << iPv4Mask_ << std::endl;
        std::cout << "udpnmclustername: " << udpNmClusterName_ << std::endl;
        std::cout << "nmpncparticipation: " << nmPncParticipation_ << std::endl;
        std::cout << "ipv4multicastipaddress: " << ipv4MulticastIpAddress_ << std::endl;
        std::cout << "udpport: " << udpPort_ << std::endl;
        std::cout << "nmcbvposition: " << nmCbvPosition_ << std::endl;
        std::cout << "nmimmediatenmcycletime: " << nmImmediateNmCycleTime_ << std::endl;
        std::cout << "nmimmediatenmtransmissions: " << nmImmediateNmTransmissions_ << std::endl;
        std::cout << "nmmsgcycletime: " << nmMsgCycleTime_ << std::endl;
        std::cout << "nmnetworktimeout: " << nmNetworkTimeout_ << std::endl;
        std::cout << "nmnidposition: " << nmNidPosition_ << std::endl;
        std::cout << "nmnodeid: " << nmNid_ << std::endl;
        std::cout << "nmrepeatmessagetime: " << nmRepeatMessageTime_ << std::endl;
        std::cout << "nmuserdatalength: " << nmUserDataLength_ << std::endl;
        std::cout << "nmuserdataoffset: " << nmUserDataOffset_ << std::endl;
        std::cout << "nmwaitbussleeptime: " << nmWaitBusSleepTime_ << std::endl;
        std::cout << "allnmmessageskeepawake: " << allNmMessagesKeepAwake_ << std::endl;
        std::cout << "nmmsgcycleoffset: " << nmMsgCycleOffset_ << std::endl;
        std::cout << "nmpnhandlemultiplenetworkrequests: " << nmPnHandleMultipleNetworkRequests_ << std::endl;
    }
#endif

    /// @brief get ip address.
    /// @returns ip address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00214
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetiPv4Address() const noexcept { return iPv4Address_; };

    /// @brief get ipv4 Multicast IpAddress of socket configure.
    /// @returns ipv4 Multicast IpAddress
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00215
    /// @needwork = dda
    /// @endcode
    ara::core::String GetIpv4MulticastIpAddress() const noexcept { return ipv4MulticastIpAddress_; };

    /// @brief get udpPort_ of socket configure.
    /// @returns udpPort_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00216
    /// @needwork = dda
    /// @endcode
    std::uint32_t GetUdpPort() const noexcept { return udpPort_; };

    /// @brief get connectorName_.
    /// @returns connectorName_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00217
    /// @needwork = dda
    /// @endcode
    ara::core::String GetConnectorName() const noexcept { return connectorName_; };

    /// @brief get config nmPnHandleMultipleNetworkRequests_.
    /// @returns nmPnHandleMultipleNetworkRequests_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00218
    /// @needwork = dda
    /// @endcode
    bool GetNmPnHandleMultipleNetworkRequests() const noexcept { return nmPnHandleMultipleNetworkRequests_; };

    /// @brief get network timeout seconds.
    /// @returns nm network timeout seconds
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00219
    /// @needwork = dda
    /// @endcode
    double GetNmNetworkTimeout() const noexcept { return nmNetworkTimeout_; };

    /// @brief get RepeatMessage state timeout  seconds.
    /// @returns RepeatMessage state timeout  seconds
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00220
    /// @needwork = dda
    /// @endcode
    double GetNmRepeatMessageTime() const noexcept { return nmRepeatMessageTime_; };

    /// @brief get Immediate Transmission counts.
    /// @returns Immediate Transmission counts
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00221
    /// @needwork = dda
    /// @endcode
    std::uint32_t GetNmImmediateNmTransmissions() const noexcept { return nmImmediateNmTransmissions_; };

    /// @brief get Immediate CycleTime seconds.
    /// @returns Immediate CycleTime seconds
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00222
    /// @needwork = dda
    /// @endcode
    double GetNmImmediateNmCycleTime() const noexcept { return nmImmediateNmCycleTime_; };

    /// @brief get nm message cycle sending time seconds.
    /// @returns nm message cycle sending time seconds.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00223
    /// @needwork = dda
    /// @endcode
    double GetNmMsgCycleTime() const noexcept { return nmMsgCycleTime_; };

    /// @brief get nm message cycle sending offset time seconds.
    /// @returns nm message cycle sending offset time seconds.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00224
    /// @needwork = dda
    /// @endcode
    double GetNmMsgCycleOffset() const noexcept { return nmMsgCycleOffset_; };

    /// @brief get nm message length(udp payload bytes).
    /// @returns nm message length(udp payload bytes).
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00225
    /// @needwork = dda
    /// @endcode
    std::uint32_t GetPduLength() const noexcept { return pduLength_; };

    /// @brief get nm message length(udp payload bytes).
    /// @param length  Message length.
    /// @returns nm message length(udp payload bytes).
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00226
    /// @needwork = dda
    /// @endcode
    void SetPduLength(std::uint32_t const lenth) noexcept { pduLength_ = lenth; };

    /// @brief Get configured NmCbvPosition (-1 if not configured).
    /// @returns nmCbvPosition_.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00227
    /// @needwork = dda
    /// @endcode
    std::int32_t GetNmCbvPosition() const noexcept { return nmCbvPosition_; };

    /// @brief Set configured NmCbvPosition (-1 if not configured).
    /// @param pos position index.
    /// @returns nmCbvPosition_.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00228
    /// @needwork = dda
    /// @endcode
    void SetNmCbvPosition(std::int32_t const pos) noexcept { nmCbvPosition_ = pos; };

    /// @brief Get configured nmPncParticipation.
    /// @returns nmPncParticipation_.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00229
    /// @needwork = dda
    /// @endcode
    bool GetNmPncParticipation() const noexcept { return nmPncParticipation_; };

    /// @brief get WaitBusSleep time seconds.
    /// @returns WaitBusSleep time seconds.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00230
    /// @needwork = dda
    /// @endcode
    double GetNmWaitBusSleepTime() const noexcept { return nmWaitBusSleepTime_; };

    /// @brief Get configured nmNidPosition (-1 if not configured).
    /// @returns nmNidPosition_.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00231
    /// @needwork = dda
    /// @endcode
    std::int32_t GetNmNidPosition() const noexcept { return nmNidPosition_; };

    /// @brief Set configured nmNidPosition (-1 if not configured).
    /// @param pos position index.
    /// @returns nmCbvPosition_.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00232
    /// @needwork = dda
    /// @endcode
    void SetNmNidPosition(std::int32_t const pos) noexcept { nmNidPosition_ = pos; };

    /// @brief Get configured nmNid (-1 if not configured).
    /// @returns nmNid_.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00233
    /// @needwork = dda
    /// @endcode
    std::int16_t GetNmNid() const noexcept { return nmNid_; };

    /// @brief Get configured user data length (-1 if not configured).
    /// @returns User data length.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00234
    /// @needwork = dda
    /// @endcode
    std::int32_t GetNmUserDataLength() const noexcept { return nmUserDataLength_; };

    /// @brief Set configured user data length.
    /// @param length User data length.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00235
    /// @needwork = dda
    /// @endcode
    void SetNmUserDataLength(std::int32_t const length) noexcept { nmUserDataLength_ = length; };

    /// @brief Get configured nmNid (-1 if not configured).
    /// @param nid node id.
    /// @returns nmNid_.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00236
    /// @needwork = dda
    /// @endcode
    void SetNmNid(std::int16_t const nid) noexcept { nmNid_ = nid; };

    /// @brief Get configured filtermask.
    /// @returns nmNid_.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00237
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< std::uint8_t > const &GetPncFilterDataMask() const noexcept { return pncFilterDataMask_; };

    /// @brief Get whether all NM messages keep wakeup.
    /// @returns AllNmMessagesKeepAwake.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00238
    /// @needwork = dda
    /// @endcode
    bool GetAllNmMessagesKeepAwake() const noexcept { return allNmMessagesKeepAwake_; };

    /// @brief Get configured user data offset.
    /// @returns User data offset.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00239
    /// @needwork = dda
    /// @endcode
    std::int32_t GetNmUserDataOffset() const noexcept { return nmUserDataOffset_; };

    /// @brief Get configured NIC maximum transmission unit.
    /// @returns NIC maximum transmission unit.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00240
    /// @needwork = dda
    /// @endcode
    std::int32_t GetMTUSize() const noexcept { return mTUSize_; };

    /// @brief Whether repeat_message are all fast transmissions.
    /// @returns Whether repeat_message are all fast transmissions.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00241
    /// @needwork = dda
    /// @endcode
    bool GetNmSendImmediateInRepeatMessage() const noexcept { return nmSendImmediateInRepeatMessage_; };

    /// @brief Get user data.
    /// @returns Get user data.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00242
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< std::uint8_t > const &GetUserData() const noexcept { return userData_; };

    /// @brief Get user data.
    /// @returns Get user data.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00243
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetUserDataOriStr() const noexcept { return userDataOriStr_; };

    /// @brief Get nmstate offset in user data (-1 if not configured).
    /// @returns nmstate offset in user data.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00244
    /// @needwork = dda
    /// @endcode
    std::int32_t GetNmStateInUserDataOffset() const noexcept { return nmStateInUserDataOffset_; }

    /// @brief Get control info: whether to set the ActiveWakeup bit in CBV when sending NM messages.
    /// @returns Whether to set the ActiveWakeup bit in CBV when sending NM messages.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00245
    /// @needwork = dda
    /// @endcode
    bool GetNmSetActiveWakupBit() const noexcept { return nmSetActiveWakupBit_; }

    /// @brief Get whether node detection request checks the PN of the message.
    /// @returns Whether node detection request checks the PN of the message.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00246
    /// @needwork = dda
    /// @endcode
    bool GetRrcPnCheck() const noexcept { return rrcPnCheck_; }

    /// @brief Get the value of nmstate when switching from bus sleep to repeat message.
    /// @returns The value of nmstate when switching from bus sleep to repeat message.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00247
    /// @needwork = dda
    /// @endcode
    std::uint8_t GetNmstateBusSleep2Repeat() const noexcept { return nmstateBusSleep2Repeat_; }

    /// @brief Get the value of nmstate when switching from prepare bus sleep to repeat message.
    /// @returns The value of nmstate when switching from prepare bus sleep to repeat message.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00248
    /// @needwork = dda
    /// @endcode
    std::uint8_t GetNmstatePrepare2Repeat() const noexcept { return nmstatePrepare2Repeat_; }

    /// @brief Get the value of nmstate when switching from repeat message to normal operation state.
    /// @returns The value of nmstate when switching from repeat message to normal operation state.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00249
    /// @needwork = dda
    /// @endcode
    std::uint8_t GetNmstateRepeat2Normal() const noexcept { return nmstateRepeat2Normal_; }

    /// @brief Get the value of nmstate when switching from sleep ready to normal operation.
    /// @returns The value of nmstate when switching from sleep ready to normal operation.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00250
    /// @needwork = dda
    /// @endcode
    std::uint8_t GetNmstateReadySlep2Normal() const noexcept { return nmstateReadySlep2Normal_; }

    /// @brief Get the value of nmstate when switching from sleep ready to repeat message.
    /// @returns The value of nmstate when switching from sleep ready to repeat message.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00251
    /// @needwork = dda
    /// @endcode
    std::uint8_t GetNmstateReadySlep2Repeat() const noexcept { return nmstateReadySlep2Repeat_; }

    /// @brief Get the value of nmstate when switching from normal operation to repeat message.
    /// @returns The value of nmstate when switching from normal operation to repeat message.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00252
    /// @needwork = dda
    /// @endcode
    std::uint8_t GetNmstateNormal2Repeat() const noexcept { return nmstateNormal2Repeat_; }

    /// @brief Get configuration: passive mode bus-sleep mode discards NM messages and can only be woken up by the wakeup interface.
    /// @returns Passive mode bus-sleep mode discards NM messages and can only be woken up by the wakeup interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00253
    /// @needwork = dda
    /// @endcode
    bool GetDropMsgInBusSleepMode() const noexcept { return dropMsgInBusSleepMode_; }

    /// @brief Get passive NIC timeout check timer.
    /// @returns Passive NIC timeout check timer.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00254
    /// @needwork = dda
    /// @endcode
    double GetPassivePnTimeout() const noexcept { return passivePnTimeout_; }

    /// @brief Get NIC name.
    /// @returns Get NIC name.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00255
    /// @needwork = dda
    /// @endcode
    ara::core::String GetIfName() const noexcept { return ifName_; }

private:
    /// @brief connectorName_
    /// EthernetCommunicationConnector name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00256
    /// @needwork = dda
    /// @endcode
    ara::core::String connectorName_{""};

    /// @brief mTUSize_
    /// This attribute specifies the maximum transmission unit in bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00257
    /// @needwork = dda
    /// @endcode
    std::int32_t mTUSize_{0};

    /// @brief pncFilterDataMaskStr_
    /// iPv4 address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00258
    /// @needwork = dda
    /// @endcode
    ara::core::String pncFilterDataMaskStr_{""};

    /// @brief pncFilterDataMask_
    /// Bit mask for Ethernet Payload used to configure the NM filter mask for the
    /// Network Management
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00259
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< std::uint8_t > pncFilterDataMask_;

    /// @brief iPv4Address_
    /// iPv4 address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00260
    /// @needwork = dda
    /// @endcode
    ara::core::String iPv4Address_{""};

    /// @brief iPv4Mask_
    /// Network mask
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00261
    /// @needwork = dda
    /// @endcode
    ara::core::String iPv4Mask_{""};

    /// @brief udpNmClusterName_
    /// udp-nm cluster name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00262
    /// @needwork = dda
    /// @endcode
    ara::core::String udpNmClusterName_{""};

    /// @brief nmPncParticipation_
    /// whether this NmCluster contributes to the partial network mechanism
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00263
    /// @needwork = dda
    /// @endcode
    bool nmPncParticipation_{false};

    /// @brief ipv4MulticastIpAddress_
    /// UDP multicast IP address of the Nm communication on a VLAN
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00264
    /// @needwork = dda
    /// @endcode
    ara::core::String ipv4MulticastIpAddress_{""};

    /// @brief udpPort_
    /// UDP port of the Nm communication on a VLAN
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00265
    /// @needwork = dda
    /// @endcode
    std::uint32_t udpPort_{0};

    /// @brief nmCbvPosition_
    /// Defines the position of the control bit vector within the Nm Pdu (Byte
    /// position).
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00266
    /// @needwork = dda
    /// @endcode
    std::int32_t nmCbvPosition_{-1};

    /// @brief nmImmediateNmCycleTime_
    /// Defines the immediate NmPdu cycle time in seconds which is used for
    /// nmImmediateNmTransmissions_ NmPdu transmissions.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00267
    /// @needwork = dda
    /// @endcode
    double nmImmediateNmCycleTime_{-1.0};

    /// @brief nmImmediateNmTransmissions_
    /// Defines the number of immediate NmPdus which shall be transmitted. If the
    /// value is zero no immediate NmPdus are transmitted.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00268
    /// @needwork = dda
    /// @endcode
    std::uint32_t nmImmediateNmTransmissions_{0};

    /// @brief nmMsgCycleTime_
    /// Period of a NmPdu in seconds. It determines the periodic rate in the
    /// periodic transmission mode with bus load reduction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00269
    /// @needwork = dda
    /// @endcode
    double nmMsgCycleTime_{-1.0};

    /// @brief nmNetworkTimeout_
    /// Network Timeout for NmPdus in seconds
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00270
    /// @needwork = dda
    /// @endcode
    double nmNetworkTimeout_{-1.0};

    /// @brief nmNidPosition_
    /// Defines the byte position of the source node identifier within the NmPdu
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00271
    /// @needwork = dda
    /// @endcode
    std::int32_t nmNidPosition_{-1};

    /// @brief nmNid_
    /// Node identifier of local NmNode. Shall be unique in the NmCluster
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00272
    /// @needwork = dda
    /// @endcode
    std::int16_t nmNid_{-1};

    /// @brief nmRepeatMessageTime_
    /// Timeout for Repeat Message State in seconds
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00273
    /// @needwork = dda
    /// @endcode
    double nmRepeatMessageTime_{-1.0};

    /// @brief nmUserDataLength_
    /// Defines the length in bytes of the user data contained in the Nm message
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00274
    /// @needwork = dda
    /// @endcode
    std::int32_t nmUserDataLength_{-1};

    /// @brief nmUserDataOffset_
    /// Specifies the offset (in bytes) of the user data information in the NM
    /// message.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00275
    /// @needwork = dda
    /// @endcode
    std::int32_t nmUserDataOffset_{-1};

    /// @brief nmWaitBusSleepTime_
    /// Timeout for bus calm down phase in seconds
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00276
    /// @needwork = dda
    /// @endcode
    double nmWaitBusSleepTime_{-1.0};

    /// @brief allNmMessagesKeepAwake_
    /// Specifies if Nm drops irrelevant NM PDUs
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00277
    /// @needwork = dda
    /// @endcode
    bool allNmMessagesKeepAwake_{false};

    /// @brief nmMsgCycleOffset_
    /// Node specific time offset in the periodic transmission node
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00278
    /// @needwork = dda
    /// @endcode
    double nmMsgCycleOffset_{-1.0};

    /// @brief nmPnHandleMultipleNetworkRequests_
    /// Specifies if NM performs an additional transition from Network Mode to
    /// Repeat Message State (true) or not (false)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00279
    /// @needwork = dda
    /// @endcode
    bool nmPnHandleMultipleNetworkRequests_{false};

    /// @brief pduLength_
    /// valid length of pdu
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00280
    /// @needwork = dda
    /// @endcode
    std::uint32_t pduLength_{0};

    /// @brief userDataOriStr_
    /// User data raw string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00281
    /// @needwork = dda
    /// @endcode
    ara::core::String userDataOriStr_{""};

    /// @brief userData_
    /// How to fill user data, can be left empty, all zeros, or 1 byte, or same as nmUserDataLength
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00282
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< std::uint8_t > userData_;

    /// @brief nmSendImmediateInRepeatMessage_
    /// As long as in repeat_message state, fast transmission if configured
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00283
    /// @needwork = dda
    /// @endcode
    bool nmSendImmediateInRepeatMessage_{false};

    /// @brief nmSetActiveWakupBit_
    /// Whether to set the Active Wakeup Bit in CBV
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00284
    /// @needwork = dda
    /// @endcode
    bool nmSetActiveWakupBit_{false};

    /// @brief nmStateInUserDataOffset_
    /// Fill nmState in user data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00285
    /// @needwork = dda
    /// @endcode
    std::int32_t nmStateInUserDataOffset_{-1};

    /// @brief rrcPnCheck_
    /// When receiving a node detection request, the node belongs to PN, check whether it contains a valid PN ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00286
    /// @needwork = dda
    /// @endcode
    bool rrcPnCheck_{false};

    /// @brief nmstateBusSleep2Repeat_
    /// The value of nmstate when switching from bus sleep to repeat message, used to fill user data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00287
    /// @needwork = dda
    /// @endcode
    std::uint8_t nmstateBusSleep2Repeat_{kNmStateBusSleep2RepeatMessage};

    /// @brief nmstatePrepare2Repeat_
    /// The value of nmstate when switching from prepare bus sleep to repeat message, used to fill user data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00288
    /// @needwork = dda
    /// @endcode
    std::uint8_t nmstatePrepare2Repeat_{kNmStatePrepareBusSleep2RepeatMessage};

    /// @brief nmstateRepeat2Normal_
    /// The value of nmstate when switching from repeat message to normal operation state, used to fill user data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00289
    /// @needwork = dda
    /// @endcode
    std::uint8_t nmstateRepeat2Normal_{kNmStateRepeatMessage2NormalOperation};

    /// @brief nmstateReadySlep2Normal_
    /// The value of nmstate when switching from sleep ready to normal operation, used to fill user data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00290
    /// @needwork = dda
    /// @endcode
    std::uint8_t nmstateReadySlep2Normal_{kNmStateReadySleep2NormalOperation};

    /// @brief nmstateReadySlep2Repeat_
    /// The value of nmstate when switching from sleep ready to repeat message, used to fill user data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00291
    /// @needwork = dda
    /// @endcode
    std::uint8_t nmstateReadySlep2Repeat_{kNmStateReadySleep2RepeatMessage};

    /// @brief nmstateNormal2Repeat_
    /// The value of nmstate when switching from normal operation to repeat message, used to fill user data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00292
    /// @needwork = dda
    /// @endcode
    std::uint8_t nmstateNormal2Repeat_{kNmStateNormalOperation2RepeatMessage};

    /// @brief dropMsgInBusSleepMode_
    /// Passive mode bus-sleep mode discards NM messages and can only be woken up by the wakeup interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00293
    /// @needwork = dda
    /// @endcode
    bool dropMsgInBusSleepMode_{false};

    /// @brief passivePnTimeout_
    /// Passive NIC timeout check timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00294
    /// @needwork = dda
    /// @endcode
    double passivePnTimeout_{kDoubleNegactiveOne};

    /// @brief ifName_
    /// When the NIC is link-down, the IP address cannot be viewed, so the NIC needs to be checked by name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00295
    /// @needwork = dda
    /// @endcode
    ara::core::String ifName_{""};
};

}  // namespace internal
}  // namespace nm
}  // namespace ara
#endif /* _ARA_NM_CONFIGURE_NMETHERUDPNMNODE_H_ */
