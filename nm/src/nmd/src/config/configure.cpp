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
/// @file       configure.cpp
/// @brief      configure of nm service
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @interface_level=Module
/// @unit_name=Configure
/// @unit_description=Configuration file management
/// @module_path=/NetworkManager/config
/// @trace_id_sr=SRS_NM_00001
/// @endcode
///
/// ================================================================

#include "include/configure.h"

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/define.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <cmath>

#include "common/common.h"
#include "utils/include/utils.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief load network-management configuration
/// @param manifestPath configuration file
/// @returns 0 ok
std::int32_t Configure::Load(ara::core::String const &manifestPath) noexcept
{
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(ara::core::StringView(manifestPath.data()))};
    if (!manifestRes.HasValue()) {
        return -1;
    }
    std::unique_ptr< isoft::manifestreader::Manifest > const manifest{std::move(manifestRes).Value()};
    defaultPara_.defaultNmTimeoutTime
        = manifest->GetValue(std::move(ara::core::StringView("defaultnmtimeouttime")), 0.0);
    defaultPara_.defaultNmMsgCycleTime
        = manifest->GetValue(std::move(ara::core::StringView("defaultnmmsgcycletime")), 0.0);
    defaultPara_.defaultNmRepeatMessageTime
        = manifest->GetValue(std::move(ara::core::StringView("defaultnmrepeatmessagetime")), 0.0);
    defaultPara_.defaultNmWaitBusSleepTime
        = manifest->GetValue(std::move(ara::core::StringView("defaultnmwaitbussleeptime")), 0.0);
    defaultPara_.defaultNmMsgCycleOffset
        = manifest->GetValue(std::move(ara::core::StringView("defaultnmmsgcycleoffset")), 0.0);
    defaultPara_.defaultNmPduNidPosition
        = manifest->GetValue(std::move(ara::core::StringView("defaultnmpdunidposition")), -1);
    defaultPara_.defaultNmPduCbvPosition
        = manifest->GetValue(std::move(ara::core::StringView("defaultnmpducbvposition")), -1);
    defaultPara_.defaultNmPnInfoLength
        = manifest->GetValue(std::move(ara::core::StringView("defaultnmpninfolength")), 0);
    defaultPara_.defaultNmPnInfoOffset
        = manifest->GetValue(std::move(ara::core::StringView("defaultnmpninfooffset")), -1);
    defaultPara_.defaultNmUserDataLength
        = manifest->GetValue(std::move(ara::core::StringView("defaultnmuserdatalength")), 0U);
    defaultPara_.defaultNmPnEnabled = manifest->GetValue(std::move(ara::core::StringView("defaultnmpnenabled")), false);
    defaultPara_.defaultNmPnResetTime
        = manifest->GetValue(std::move(ara::core::StringView("defaultnmpnresettime")), 0.0);
    defaultPara_.defaultPncPrepareSleepTime
        = manifest->GetValue(std::move(ara::core::StringView("defaultpncpreparesleeptime")), 0.0);
    defaultPara_.defaultNmNodeId
        = manifest->GetValue(std::move(ara::core::StringView("defaultnmnodeid")), static_cast< std::int16_t >(0));
    defaultPara_.defaultNmUserDataEnabled
        = manifest->GetValue(std::move(ara::core::StringView("defaultnmuserdataenabled")), false);
    defaultPara_.skiptimerCheck = manifest->GetValue(std::move(ara::core::StringView("skiptimerCheck")), false);
    defaultPara_.didInstance
        = manifest->GetValue(std::move(ara::core::StringView("didInstance")), ara::core::String(""));
    defaultPara_.diagMonitorId
        = manifest->GetValue(std::move(ara::core::StringView("diagMonitorInstance")), ara::core::String(""));
    defaultPara_.diagOperationCycleInstance
        = manifest->GetValue(std::move(ara::core::StringView("diagOperationCycleInstance")), ara::core::String(""));
    if (isoft::kSuccess != manifest->Load(std::move(ara::core::StringView("nmlogichandlearray")), logicHandleGroup_)) {
        NmLogger().LogError() << "Configure::Load, error in load logic network";
        return -1;
    }
    if (isoft::kSuccess != manifest->Load(std::move(ara::core::StringView("partialnetworkarray")), pnGroup_)) {
        NmLogger().LogInfo() << "Configure::Load, dont have partial network";
        return -1;
    }
    if (isoft::kSuccess
        != manifest->Load(std::move(ara::core::StringView("ethernetconnectorarray")), ethernetUdpNmNodeGroup_)) {
        NmLogger().LogError() << "Configure::Load, error in load ethernet connector";
        return -1;
    }
    _debug();
    if (!_setDefaultAndCheckEtherConfig()) {
        NmLogger().LogError() << "Configure::Load, error in SetEtherConnectorDefaultConfig";
        return -1;
    }
    return _checkConfig();
}

/// @brief check configuration
/// @trace_id_sws= {SWS_ANM_00066}
/// @returns 0 valid
std::int32_t Configure::_checkConfig() noexcept
{
    std::int32_t ret{_checkLNConfig()};
    if (0 != ret) {
        return ret;
    }
    ret = _checkPNConfig();
    if (0 != ret) {
        return ret;
    }
    if (((defaultPara_.defaultNmPnInfoOffset < kPncVectorOffsetMin) && (-1 != defaultPara_.defaultNmPnInfoOffset))
        || (defaultPara_.defaultNmPnInfoOffset > kPncVectorOffsetMax)) {
        NmLogger().LogError() << "Configure::_checkConfig, invalid defaultNmPnInfoOffset="
                              << defaultPara_.defaultNmPnInfoOffset;
        return -1;
    }
    if (((defaultPara_.defaultNmPnInfoLength < kPncVectorLengthMin) && (-1 != defaultPara_.defaultNmPnInfoLength))
        || (defaultPara_.defaultNmPnInfoLength > kPncVectorLengthMax)) {
        NmLogger().LogError() << "Configure::_checkConfig, invalid defaultNmPnInfoLength="
                              << defaultPara_.defaultNmPnInfoLength;
        return -1;
    }
    return 0;
}

/// @brief check configuration
/// @trace_id_sws= {SWS_ANM_00066}
/// @returns 0 valid
std::int32_t Configure::_checkLNConfig() noexcept
{
    for (ara::core::Vector< NmLogicHandleConfig >::iterator itLn{logicHandleGroup_.begin()};
         logicHandleGroup_.end() != itLn; ++itLn) {
        ara::core::Vector< ara::core::String > etherInPnVec;
        for (ara::core::Vector< PnReferenceConfig >::const_iterator itPn{itLn->GetPncRefVec().begin()};
             itLn->GetPncRefVec().end() != itPn; ++itPn) {
            if (!_checkPnExist(itPn->GetPnIndex())) {
                NmLogger().LogError() << "Configure::_checkLNConfig, cant find configuration for pn "
                                      << itPn->GetPnIndex().c_str() << " in logic network "
                                      << itLn->GetHandleIndex().c_str();
                return -1;
            }
            ara::core::Vector< ara::core::String > etherInOnePnVec;
            if (_getPnEtherConnectorsList(itPn->GetPnIndex(), etherInOnePnVec)) {
                ara::core::Vector< ara::core::String >::const_iterator const it{etherInPnVec.cbegin()};
                static_cast< void >(etherInPnVec.insert(it, etherInOnePnVec.begin(), etherInOnePnVec.end()));
            }
        }
        for (ara::core::Vector< EtherReferenceConfig >::const_iterator itEtherNet{itLn->GetEtherRefVec().begin()};
             itLn->GetEtherRefVec().end() != itEtherNet; ++itEtherNet) {
            if (!_checkEtherConnectorExist(std::move(itEtherNet->GetEherName()))) {
                NmLogger().LogError() << "Configure::_checkLNConfig, cant find configuration for "
                                         "EtherConnector "
                                      << itEtherNet->GetEherName().c_str() << " in logic network "
                                      << itLn->GetHandleIndex().c_str();
                return -1;
            }
            for (ara::core::Vector< ara::core::String >::iterator itEtherNetInPn{etherInPnVec.begin()};
                 etherInPnVec.end() != itEtherNetInPn; ++itEtherNetInPn) {
                if (0 == itEtherNet->GetEherName().compare(*itEtherNetInPn)) {
                    NmLogger().LogError()
                        << "Configure::_checkLNConfig, duplicate include for "
                           "EtherConnector "
                        << itEtherNet->GetEherName().c_str() << " in logic network " << itLn->GetHandleIndex().c_str();
                    return -1;
                }
            }
        }
        if (itLn->GetLnInstanceSpecifier().empty()) {
            NmLogger().LogWarn() << "Configure::_checkLNConfig, lnInstanceSpecifier empty";
        }
    }
    return 0;
}

/// @brief check configuration
/// @trace_id_sws= {SWS_ANM_00066}
/// @returns 0 valid
std::int32_t Configure::_checkPNConfig() noexcept
{
    for (ara::core::Vector< NmPnHandleConfig >::iterator itPn{pnGroup_.begin()}; pnGroup_.end() != itPn; ++itPn) {
        for (ara::core::Vector< EtherReferenceConfig >::const_iterator itethRefItem{itPn->GetEthernetVec().begin()};
             itPn->GetEthernetVec().end() != itethRefItem; ++itethRefItem) {
            if (!_checkEtherConnectorExist(itethRefItem->GetEherName())) {
                NmLogger().LogError() << "Configure::_checkPNConfig, cant find configuration for "
                                         "EtherConnector "
                                      << itethRefItem->GetEherName().c_str() << " in partial network "
                                      << itPn->GetPnIndex().c_str();
                return -1;
            }
        }
        if (itPn->GetPnId() > (kPncIDMax)) {
            NmLogger().LogError() << "Configure::_checkPNConfig error, PN identifier " << itPn->GetPnId()
                                  << " is big than " << kPncIDMax;
            return -1;
        }
    }
    return 0;
}

/// @brief existence-check of partial network
/// @param pnHandle partial network name
/// @returns true valid
bool Configure::_checkPnExist(ara::core::String const &pnHandle) noexcept
{
    for (ara::core::Vector< NmPnHandleConfig >::iterator it{pnGroup_.begin()}; pnGroup_.end() != it; ++it) {
        if (0 == pnHandle.compare(it->GetPnIndex())) {
            return true;
        }
    }
    return false;
}

/// @brief existence-check of partial network
/// @param pnHandle partial network name
/// @param connectList connector list
/// @returns true valid
bool Configure::_getPnEtherConnectorsList(ara::core::String const &pnHandle,
                                          ara::core::Vector< ara::core::String > &connectList) noexcept
{
    bool bFind{false};
    for (ara::core::Vector< NmPnHandleConfig >::iterator it{pnGroup_.begin()}; pnGroup_.end() != it; ++it) {
        if (0 == pnHandle.compare(it->GetPnIndex())) {
            bFind = true;
            for (ara::core::Vector< EtherReferenceConfig >::const_iterator itEthernet{it->GetEthernetVec().begin()};
                 it->GetEthernetVec().end() != itEthernet; ++itEthernet) {
                connectList.push_back(itEthernet->GetEherName());
            }
            break;
        }
    }
    return bFind;
}

/// @brief existence-check of ethernet-connector
/// @param connectorName ethernet-connector name
/// @returns true valid
bool Configure::_checkEtherConnectorExist(ara::core::String const &connectorName) noexcept
{
    bool bFind{false};
    for (ara::core::Vector< NmEthernetUdpNmNodeConfig >::iterator it{ethernetUdpNmNodeGroup_.begin()};
         ethernetUdpNmNodeGroup_.end() != it; ++it) {
        if (0 == connectorName.compare(it->GetConnectorName())) {
            bFind = true;
            break;
        }
    }
    return bFind;
}

/// @brief validation-check of ethernet-connector array
/// @returns true valid
bool Configure::_setDefaultAndCheckEtherConfig() noexcept
{
    for (ara::core::Vector< NmEthernetUdpNmNodeConfig >::iterator it{ethernetUdpNmNodeGroup_.begin()};
         ethernetUdpNmNodeGroup_.end() != it; ++it) {
        if (0 > it->GetNmCbvPosition()) {
            it->SetNmCbvPosition(defaultPara_.defaultNmPduCbvPosition);
        }
        if (0 > it->GetNmNidPosition()) {
            it->SetNmNidPosition(defaultPara_.defaultNmPduNidPosition);
        }
        if (0 > it->GetNmNid()) {
            it->SetNmNid(defaultPara_.defaultNmNodeId);
        }
        if ((0 > it->GetNmUserDataLength()) && (true == defaultPara_.defaultNmUserDataEnabled)) {
            it->SetNmUserDataLength(static_cast< std::int32_t >(defaultPara_.defaultNmUserDataLength));
        }
        std::int32_t headUSed{0};
        if (0 <= it->GetNmCbvPosition()) {
            headUSed++;
        }
        if (0 <= it->GetNmNidPosition()) {
            headUSed++;
        }
        /// offset starts from 0 Offset (pncVectorOffset) starting from byte 0
        std::int32_t const pmInofStart{defaultPara_.defaultNmPnInfoOffset};
        std::int32_t const pmInofEnd{defaultPara_.defaultNmPnInfoOffset + defaultPara_.defaultNmPnInfoLength - 1};
        std::int32_t const nmUserDataOffStart{it->GetNmUserDataOffset()};
        std::int32_t const nmUserDataOffEnd{it->GetNmUserDataOffset() + it->GetNmUserDataLength() - 1};
        if (!((pmInofEnd < nmUserDataOffStart) || (nmUserDataOffEnd < pmInofStart))) {
            NmLogger().LogError() << "_setDefaultAndCheckEtherConfig, UserData overlap with PmInfo";
            return false;
        }
        if ((pmInofEnd < nmUserDataOffStart) && (0 < nmUserDataOffEnd)) {
            std::int32_t const pduLen{nmUserDataOffEnd + 1};
            it->SetPduLength(static_cast< std::uint32_t >(pduLen));
        } else if (pmInofEnd > 0) {
            std::int32_t const pduLen{pmInofEnd + 1};
            it->SetPduLength(static_cast< std::uint32_t >(pduLen));
        } else {
            it->SetPduLength(static_cast< std::uint32_t >(headUSed));
        }

        if (it->GetPduLength() >= static_cast< std::uint32_t >(it->GetMTUSize())) {
            NmLogger().LogError() << "_setDefaultAndCheckEtherConfig, pduLength= " << it->GetPduLength()
                                  << " mTUSize= " << it->GetMTUSize();
            return false;
        }
        if (0U == it->GetPduLength()) {
            NmLogger().LogError() << "_setDefaultAndCheckEtherConfig, pduLength= " << it->GetPduLength();
            return false;
        }
        if (-1 > it->GetNmNidPosition()) {
            NmLogger().LogError() << "_setDefaultAndCheckEtherConfig, NmNidPosition= " << it->GetNmNidPosition();
            return false;
        }
        if (1 < it->GetNmNidPosition()) {
            NmLogger().LogError() << "_setDefaultAndCheckEtherConfig, NmNidPosition= " << it->GetNmNidPosition();
            return false;
        }
        if (-1 > it->GetNmCbvPosition()) {
            NmLogger().LogError() << "_setDefaultAndCheckEtherConfig, NmNidPosition= " << it->GetNmCbvPosition();
            return false;
        }
        if (1 < it->GetNmCbvPosition()) {
            NmLogger().LogError() << "_setDefaultAndCheckEtherConfig, NmNidPosition= " << it->GetNmCbvPosition();
            return false;
        }
        if (it->GetNmCbvPosition() == it->GetNmNidPosition()) {
            if (-1 != it->GetNmCbvPosition()) {
                NmLogger().LogError() << "_setDefaultAndCheckEtherConfig, NmNidPosition = nmCbvPosition";
                return false;
            }
        }
        if (0 < it->GetNmUserDataLength()) {
            if (1U < it->GetUserData().size()) {
                if (it->GetUserData().size() != static_cast< std::uint32_t >(it->GetNmUserDataLength())) {
                    NmLogger().LogError()
                        << "_setDefaultAndCheckEtherConfig, doesnt match, "
                           "nmUserDataLength is "
                        << it->GetNmUserDataLength() << ", userDataFilled is " << it->GetUserDataOriStr().c_str();
                }
            }
        }
        if (true != _checketherTimerConfig(*it)) {
            return false;
        }
    }
    return true;
}

/// @brief validation-check of ethernet-connector timer array
/// @param ref ethernet-connector reference
/// @returns true valid
bool Configure::_checketherTimerConfig(NmEthernetUdpNmNodeConfig const &ref) const noexcept
{
    if (false == defaultPara_.skiptimerCheck) {
        if (kTimerMillSecond > ref.GetNmMsgCycleTime()) {
            NmLogger().LogError() << "_checketherTimerConfig,"
                                  << "NmMsgCycleTime should be configured with a value not smaller "
                                     "than "
                                  << kTimerMillSecond << " " << ref.GetConnectorName().c_str();
            return false;
        }
        if (kTimerMillSecond > ref.GetNmNetworkTimeout()) {
            NmLogger().LogError() << "_checketherTimerConfig,"
                                  << "NmNetworkTimeout should be configured with a value not smaller "
                                     "than "
                                  << kTimerMillSecond << " " << ref.GetConnectorName().c_str();
            return false;
        }
        if (0 > ref.GetNmRepeatMessageTime()) {
            NmLogger().LogError() << "_checketherTimerConfig,"
                                  << "NmRepeatMessageTime should be configured with "
                                     "a value not smaller than "
                                  << 0 << " " << ref.GetConnectorName().c_str();
            return false;
        }
        if (0 > ref.GetNmWaitBusSleepTime()) {
            NmLogger().LogError() << "_checketherTimerConfig,"
                                  << "NmWaitBusSleepTime should be configured with a "
                                     "value not smaller than "
                                  << 0 << " " << ref.GetConnectorName().c_str();
            return false;
        }
        if (0 > ref.GetNmMsgCycleOffset()) {
            NmLogger().LogError() << "_checketherTimerConfig,"
                                  << "nmMsgCycleOffset should be configured with a "
                                     "value not smaller than "
                                  << 0 << " " << ref.GetConnectorName().c_str();
            return false;
        }
        if (ref.GetNmMsgCycleOffset() >= ref.GetNmMsgCycleTime()) {
            NmLogger().LogError() << "_checketherTimerConfig, nmMsgCycleOffset "
                                     "should be smaller than GetNmMsgCycleTime "
                                  << ref.GetConnectorName().c_str();
            return false;
        }
        /// 2011 cp udpnm
        /// Timer accurate to milliseconds, event loop timer accurate to milliseconds, currently only checks integer multiples to milliseconds
        if (0 != (TimeS2MS(ref.GetNmNetworkTimeout()) % TimeS2MS(ref.GetNmMsgCycleTime()))) {
            NmLogger().LogError() << "_checketherTimerConfig, NmNetworkTimeout shall be n * "
                                  << "NmMsgCycleTime with n > 1" << ref.GetConnectorName().c_str();
            return false;
        }
        if (1.0 >= (ref.GetNmNetworkTimeout() / ref.GetNmMsgCycleTime())) {
            NmLogger().LogError() << "_checketherTimerConfig, NmNetworkTimeout shall be n * "
                                  << "NmMsgCycleTime with n > 1" << ref.GetConnectorName().c_str();
            return false;
        }
        /// 2011 cp udpnm
        /// Timer accurate to milliseconds, event loop timer accurate to milliseconds, currently only checks integer multiples to milliseconds
        if (0 != (TimeS2MS(ref.GetNmRepeatMessageTime()) % TimeS2MS(ref.GetNmMsgCycleTime()))) {
            NmLogger().LogError() << "_checketherTimerConfig, NmRepeatMessageTime "
                                     "should be multiple of "
                                     "NmMsgCycleTime "
                                  << ref.GetConnectorName().c_str();
            return false;
        }
        if (ref.GetNmRepeatMessageTime()
            < (static_cast< double >(ref.GetNmImmediateNmTransmissions()) * ref.GetNmImmediateNmCycleTime())) {
            NmLogger().LogError() << "_checketherTimerConfig, NmRepeatMessageTime is smaller than "
                                     "nmImmediateNmTransmissions * nmImmediateNmCycleTime "
                                  << ref.GetConnectorName().c_str();
            return false;
        }
        if (0U < ref.GetNmImmediateNmTransmissions()) {
            if (kTimerMillSecond > ref.GetNmImmediateNmCycleTime()) {
                NmLogger().LogError() << "_checketherTimerConfig, nmImmediateNmTransmissions is bigger "
                                         "than 0,"
                                         "nmImmediateNmCycleTime is smaller than "
                                      << kTimerMillSecond << " " << ref.GetConnectorName().c_str();
                return false;
            }
        }
        if (ref.GetNmRepeatMessageTime() <= ref.GetNmMsgCycleOffset()) {
            NmLogger().LogError() << "_checketherTimerConfig, NmRepeatMessageTime is smaller than "
                                     "nmMsgCycleOffset "
                                  << ref.GetConnectorName().c_str();
            return false;
        }
        if (false == CheckTimerMaxium(ref.GetNmNetworkTimeout())) {
            NmLogger().LogError() << "_checketherTimerConfig, NmNetworkTimeout is too big, "
                                  << ref.GetNmNetworkTimeout() << " " << ref.GetConnectorName().c_str();
            return false;
        }
        if (false == CheckTimerMaxium(ref.GetNmRepeatMessageTime())) {
            NmLogger().LogError() << "_checketherTimerConfig, NmRepeatMessageTime is too big, "
                                  << ref.GetNmRepeatMessageTime() << " " << ref.GetConnectorName().c_str();
            return false;
        }
        if (false == CheckTimerMaxium(ref.GetNmWaitBusSleepTime())) {
            NmLogger().LogError() << "_checketherTimerConfig, NmWaitBusSleepTime is too big, "
                                  << ref.GetNmWaitBusSleepTime() << " " << ref.GetConnectorName().c_str();
            return false;
        }
    }
    if (_belongToPn(ref.GetConnectorName())) {
        if (false == defaultPara_.skiptimerCheck) {
            if (!(defaultPara_.defaultNmPnResetTime > ref.GetNmMsgCycleTime())) {
                NmLogger().LogError() << "_checketherTimerConfig, defaultNmPnResetTime should larger "
                                         "than nmMsgCycleTime,"
                                      << ref.GetConnectorName().c_str();
                return false;
            }
            if (!((defaultPara_.defaultNmPnResetTime + defaultPara_.defaultPncPrepareSleepTime)
                  < ref.GetNmNetworkTimeout())) {
                NmLogger().LogError() << "_checketherTimerConfig, nmNetworkTimeout is smaller than "
                                         "PnResetTime + PncPrepareSleepTime,"
                                      << ref.GetConnectorName().c_str();
                return false;
            }
        }
    }
    return true;
}

/// @brief get partial network configurations of ethernet connector
/// @param connectorName ethernet connector name
/// @param configPnGroup partial network configuration reference
/// @returns true valid
bool Configure::GetPnsOfEthernet(ara::core::String const &connectorName,
                                 ara::core::Vector< std::uint16_t > &configPnGroup) noexcept
{
    for (ara::core::Vector< NmPnHandleConfig >::iterator it{pnGroup_.begin()}; pnGroup_.end() != it; ++it) {
        for (ara::core::Vector< EtherReferenceConfig >::const_iterator etherit{it->GetEthernetVec().begin()};
             it->GetEthernetVec().end() != etherit; ++etherit) {
            if (0 == etherit->GetEherName().compare(connectorName)) {
                configPnGroup.push_back(it->GetPnId());
                break;
            }
        }
    }
    return true;
}

/// @brief if ethernet connector belong to pn
/// @param connectorName ethernet connector name
/// @returns true belong
bool Configure::_belongToPn(ara::core::String const &connectorName) const noexcept
{
    for (ara::core::Vector< NmPnHandleConfig >::const_iterator it{pnGroup_.cbegin()}; pnGroup_.cend() != it; ++it) {
        for (ara::core::Vector< EtherReferenceConfig >::const_iterator etherit{it->GetEthernetVec().begin()};
             it->GetEthernetVec().end() != etherit; ++etherit) {
            if (0 == etherit->GetEherName().compare(connectorName)) {
                return true;
            }
        }
    }
    return false;
}

/// @brief get all ethernet connector configuration
/// @param connectorName network connector name
/// @returns configuration reference
NmEthernetUdpNmNodeConfig const *Configure::GetEtherConfig(ara::core::String const &connectorName) noexcept
{
    for (ara::core::Vector< NmEthernetUdpNmNodeConfig >::iterator it{ethernetUdpNmNodeGroup_.begin()};
         ethernetUdpNmNodeGroup_.end() != it; ++it) {
        if (connectorName == it->GetConnectorName()) {
            return &(*it);
        }
    }
    return nullptr;
}
/// @brief Print debug information
void Configure::_debug() const noexcept
{
#if ARA_NM_DEBUG
    std::cout << "defaultNmTimeoutTime: " << defaultPara_.defaultNmTimeoutTime << std::endl;
    std::cout << "defaultNmMsgCycleTime: " << defaultPara_.defaultNmMsgCycleTime << std::endl;
    std::cout << "defaultNmRepeatMessageTime: " << defaultPara_.defaultNmRepeatMessageTime << std::endl;
    std::cout << "defaultNmWaitBusSleepTime: " << defaultPara_.defaultNmWaitBusSleepTime << std::endl;
    std::cout << "defaultNmMsgCycleOffset: " << defaultPara_.defaultNmMsgCycleOffset << std::endl;
    std::cout << "defaultNmPduNidPosition: " << defaultPara_.defaultNmPduNidPosition << std::endl;
    std::cout << "defaultNmPduCbvPosition: " << defaultPara_.defaultNmPduCbvPosition << std::endl;
    std::cout << "defaultNmPnInfoLength: " << defaultPara_.defaultNmPnInfoLength << std::endl;
    std::cout << "defaultNmPnInfoOffset: " << defaultPara_.defaultNmPnInfoOffset << std::endl;
    std::cout << "defaultNmUserDataLength: " << defaultPara_.defaultNmUserDataLength << std::endl;
    std::cout << "defaultNmPnEnabled: " << defaultPara_.defaultNmPnEnabled << std::endl;
    std::cout << "defaultNmPnResetTime: " << defaultPara_.defaultNmPnResetTime << std::endl;
    std::cout << "defaultPncPrepareSleepTime: " << defaultPara_.defaultPncPrepareSleepTime << std::endl;
    std::cout << "defaultNmNodeId: " << defaultPara_.defaultNmNodeId << std::endl;
    std::cout << "defaultNmUserDataEnabled: " << defaultPara_.defaultNmUserDataEnabled << std::endl;

    std::cout << "nmlogichandlearray: [" << std::endl;
    ara::core::Vector< NmLogicHandleConfig >::const_iterator const logicHandleGroupEnd{std::end(logicHandleGroup_)};
    for (ara::core::Vector< NmLogicHandleConfig >::const_iterator logicHandleGroupIt{std::begin(logicHandleGroup_)};
         logicHandleGroupEnd != logicHandleGroupIt; ++logicHandleGroupIt) {
        logicHandleGroupIt->Debug();
    }
    std::cout << "]" << std::endl;

    std::cout << "partialnetworkarray: [" << std::endl;
    ara::core::Vector< NmPnHandleConfig >::const_iterator const pnGroupEnd{std::end(pnGroup_)};
    for (ara::core::Vector< NmPnHandleConfig >::const_iterator pnGroupIt{std::begin(pnGroup_)}; pnGroupEnd != pnGroupIt;
         ++pnGroupIt) {
        pnGroupIt->Debug();
    }
    std::cout << "]" << std::endl;

    std::cout << "ethernetconnectorarray: [" << std::endl;
    ara::core::Vector< NmEthernetUdpNmNodeConfig >::const_iterator const itEtherEnd{std::end(ethernetUdpNmNodeGroup_)};
    for (ara::core::Vector< NmEthernetUdpNmNodeConfig >::const_iterator itEther{std::begin(ethernetUdpNmNodeGroup_)};
         itEtherEnd != itEther; ++itEther) {
        itEther->Debug();
    }
    std::cout << "]" << std::endl;
#else
    std::ignore = pnGroup_;  /// just for qac
#endif
}
}  // namespace internal
}  // namespace nm
}  // namespace ara
