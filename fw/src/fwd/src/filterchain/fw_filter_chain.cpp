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
/// @file       fw_filter_chain.cpp
/// @brief      fw filter chain processing class
/// @details    fw filter chain processing class
/// @date       2024-11-26
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Filter Manager
/// @interface_level=module
/// @trace_id_sr=SR_FW_0006,SR_FW_0007,SR_FW_0008
/// @unit_name=Filter_Chain
/// @unit_description=Firewall filter chain management class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "fw_filter_chain.h"

#include <sys/wait.h>
#include <unistd.h>

#include "ara/fw/common/common.h"
#include "ara/fw/filterengine/create.h"
#include "ara/fw/filterengine/delete.h"
#include "ara/fw/filterengine/engine_common.h"
#include "ara/fw/internal/cmd_str.h"
#include "ara/fw/internal/log_api.h"
#include "ara/fw/internal/manifest_data.h"
#include "filter/datalinkfilter.h"
#include "filter/ddsfilter.h"
#include "filter/doipfilter.h"
#include "filter/ipv4filter.h"
#include "filter/ipv6filter.h"
#include "filter/someipfilter.h"
#include "filter/someipsdfilter.h"
#include "filter/tcpfilter.h"
#include "filter/udpfilter.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief Parameterized constructor
/// @param ptrConfigManager Configuration management class pointer
FWFilterChain::FWFilterChain(std::shared_ptr< ConfigManager > const &ptrConfigManager) noexcept
    : configManagerPtr_(std::move(ptrConfigManager))
{
}

/// @brief set the instance descriptor and firewall state that need to be
/// configured for fw firewall.
/// @param inst_
/// @param fwState
void FWFilterChain::SetFWStateAndInstance(ara::core::String const &inst, uint32_t fwState) noexcept
{
    this->fwState_  = fwState;
    this->instSpec_ = inst;
}

/// @brief Get FWRuleProps and defaultaction.
/// @return Success/Failure
bool FWFilterChain::GetFWRulePropsAndDefaultAction() noexcept
{
    if (!configManagerPtr_) {
        LogInfo() << "GetFWRuleProps: get configManagerPtr_ is null!";
        return false;
    }

    if (!configManagerPtr_->GetFWPropsRules(this->instSpec_, this->fwState_, this->vecRuleProps_,
                                            this->defaultAction_)) {
        LogInfo() << "GetFWRuleProps: get GetFWPropsRulesfailed!";
        return false;
    }
    LogDebug() << "GetFWRulePropsAndDefaultAction: get defaultaction and "
                  "RuleProps success! ";
    return true;
}

/// @brief Get FW ingress rule set
/// @return Success/Failure
bool FWFilterChain::AllRulesCheck() noexcept
{
    if (vecRuleProps_.empty()) {
        LogError() << "AllRulesCheck: vecRuleProps_ is null! please check manifest "
                      "config! ";
        return false;
    }
    // Clear previous rule set before checking.
    FilterRulesDelete(1);
    FilterRulesDelete(0);
    // Delete kernel ingress module configuration.
    _DestoryAppInRules();
    // Delete kernel egress module configuration.
    _DestoryAppRules();
    // defaultAction.
    _SetAppSingleRuleData(-1, "g_DefaultAction");

    // Loop firewallProps to filter all rules.
    for (auto const &it : vecRuleProps_) {
        this->action_          = it.action;
        this->vecIngressRules_ = it.vecIngressRules;
        this->vecGressRules_   = it.vecEgressRules;
        // Ingress
        _RulesCheckByIngress();

        // Egress
        _RulesCheckByGress();
    }
    // fw rule configuration completed, clear the created fw chains, instance
    // descriptors, and fw state.
    // _DestoryCmd();
    // APP layer clears defaultAction configuration.
    return true;
}

/// @brief fw creates nft tables and chains (both INPUT and OUTPUT are created.)
/// @return success/failed
bool FWFilterChain::CreateTableAndChain() const noexcept
{
    // bugfix: If the FirewallRuleProps passed by the user do not contain ingress
    // or egress information, then do not create egress/ingress chains. inflag
    bool ingressConfigFlag{false};
    // outflag
    bool gressConfigFlag{false};
    for (auto const &it : vecRuleProps_) {
        // If any input is non-empty, an input chain needs to be created.
        if (!it.vecIngressRules.empty()) {
            ingressConfigFlag = true;
        }
        // If any output is non-empty, an output chain needs to be created.
        if (!it.vecEgressRules.empty()) {
            gressConfigFlag = true;
        }
    }
    // Create table first.
    if (!FilterTableCreate()) {
        LogError() << "CreateTableAndChain: create table failed!";
        return false;
    }
    // Create ingress chain.
    if (ingressConfigFlag) {
        if (!InputFilterChainCreate(this->defaultAction_, 0)) {
            LogError() << "CreateTableAndChain: create ingress chain failed!";
            return false;
        }
    }
    // Create egress chain.
    if (gressConfigFlag) {
        if (!InputFilterChainCreate(this->defaultAction_, 1)) {
            LogError() << "CreateTableAndChain: create gress chain failed!";
            return false;
        }
    }
    return true;
}

/// @brief Firewall rule validation.
/// @param filterType
/// @return  success/failed
bool FWFilterChain::_RulesCheck(uint32_t const filterType) noexcept
{
    LogInfo() << " FWFilterChain:_RulesCheck() in!";
    // Ingress
    if (filterType == kIngressType) {
        for (const std::pair< const ara::core::String, std::unique_ptr< ara::fw::internal::FilterBase > > &inputRules :
             this->fwIngressRules_) {
            // get rules.
            inputRules.second->GetRules();
            // filter rules.
            inputRules.second->RulesFilter();
        }
        // Push APP layer rule data to the kernel.
        _SetAppRuleIntoKernal(1);
        // Clear map data after each check.
        fwIngressRules_.clear();
    } else if (filterType == kGressType) {
        // Egress
        for (const std::pair< const ara::core::String, std::unique_ptr< ara::fw::internal::FilterBase > > &outputRules :
             this->fwGressRules_) {
            // get rules.
            outputRules.second->GetRules();
            // filter rules.
            outputRules.second->RulesFilter();
        }
        // Push APP layer rule data to the kernel.
        _SetAppRuleIntoKernal(0);
        // Clear map data after each check.
        fwGressRules_.clear();
    } else {
        LogError() << "_RulesCheck:get filterType failed!";
        return false;
    }
    return true;
}

/// @brief Clear rules
/// @return
void FWFilterChain::_DestoryRules() noexcept
{
    LogInfo() << " FWFilterChain:_DestoryRules() now _DestoryRules!";
    // Clear rule set.
    // Perform actual filtering map
    fwIngressRules_.clear();
    fwGressRules_.clear();
    // Ingress and Egress loop iterator.
    vecIngressRules_.clear();
    vecGressRules_.clear();
    this->action_ = -1;
    _DestoryAppRules();
    return;
}

/// @brief Reset APP layer ingress rules to -1.
void FWFilterChain::_DestoryAppInRules() noexcept
{
    // someip Rules.
    _SetAppSingleRuleData(-1, "g_ClientId_In");
    _SetAppSingleRuleData(-1, "g_Target_Length_Flag_In");
    _SetAppSingleRuleData(-1, "g_Target_Major_Version_In");
    _SetAppSingleRuleData(-1, "g_Target_Msg_Type_In");
    _SetAppSingleRuleData(-1, "g_Target_Method_Id_In");
    _SetAppSingleRuleData(-1, "g_Target_Pro_Version_In");
    _SetAppSingleRuleData(-1, "g_Target_Ret_Code_In");
    _SetAppSingleRuleData(-1, "g_Target_Service_Id_In");

    // someip sd Rules
    _SetAppSingleRuleData(-1, "g_Sd_EntType_In");
    _SetAppSingleRuleData(-1, "g_Sd_GroupId_In");
    _SetAppSingleRuleData(-1, "g_Sd_ServiceInstId_In");
    _SetAppSingleRuleData(-1, "g_Sd_ServiceId_In");
    // doip Rules
    _SetAppSingleRuleData(-1, "g_ProtocolVersion_In");
    _SetAppSingleRuleData(-1, "g_InverProtocolVersion_In");
    _SetAppSingleRuleData(-1, "g_PayType_In");
    _SetAppSingleRuleData(-1, "g_PayLoadLen_In");
    _SetAppSingleRuleData(-1, "g_UdsService_In");
}

/// @brief Reset APP layer rules to -1 (out)
void FWFilterChain::_DestoryAppRules() noexcept
{
    // Reset all rule sets
    // someip Rules.
    _SetAppSingleRuleData(-1, "g_ClientId_Out");

    _SetAppSingleRuleData(-1, "g_Target_Length_Flag_Out");

    _SetAppSingleRuleData(-1, "g_Target_Major_Version_Out");

    _SetAppSingleRuleData(-1, "g_Target_Msg_Type_Out");

    _SetAppSingleRuleData(-1, "g_Target_Method_Id_Out");

    _SetAppSingleRuleData(-1, "g_Target_Pro_Version_Out");

    _SetAppSingleRuleData(-1, "g_Target_Ret_Code_Out");

    _SetAppSingleRuleData(-1, "g_Target_Service_Id_Out");

    // someip sd Rules

    _SetAppSingleRuleData(-1, "g_Sd_EntType_Out");

    _SetAppSingleRuleData(-1, "g_Sd_GroupId_Out");

    _SetAppSingleRuleData(-1, "g_Sd_ServiceInstId_Out");

    _SetAppSingleRuleData(-1, "g_Sd_ServiceId_Out");

    // doip Rules
    _SetAppSingleRuleData(-1, "g_ProtocolVersion_Out");

    _SetAppSingleRuleData(-1, "g_InverProtocolVersion_Out");

    _SetAppSingleRuleData(-1, "g_PayType_Out");

    _SetAppSingleRuleData(-1, "g_PayLoadLen_Out");

    _SetAppSingleRuleData(-1, "g_UdsService_Out");
    // action
    _SetAppSingleRuleData(-1, "g_Action");
    // inoutFlag in:1
    // _SetAppSingleRuleData(-1, "g_InOutFlag");
}

/// @brief After each FW rule input is completed, clear the created tables and
/// chains.
/// @return
void FWFilterChain::_DestoryCmd() noexcept
{
    LogInfo() << " FWFilterChain:_DestoryCmd() now _DestoryCmd!";
    // After each fw rule deployment is completed, clear the corresponding
    // instance descriptor and firewall state to maintain the initial fw state.
    this->instSpec_.clear();
    this->fwState_ = 0;
    // Delete all created tables. inet ap_filter
    if (FilterTableDelete()) {
        LogError() << "_DestoryCmd: delete table or chain failed! ";
    }
    return;
}

/// @brief Ingress rule filtering
void FWFilterChain::_RulesCheckByIngress() noexcept
{
    if (this->vecIngressRules_.empty()) {
        LogError() << "_RulesCheckByIngress: fwGressRules_ is null! please check "
                      "manifest config! ";
        return;
    }
    // Loop validation.
    for (auto const &it : this->vecIngressRules_) {
        if (!it.vecDataLinkRules.empty()) {
            // datalinkRules check.
            fwIngressRules_["DataLinkLayerRule"]
                = std::make_unique< DataLinkFilter >(it.vecDataLinkRules, action_, 0, it.refillAmount, it.bucketSize);
        }
        if (!it.vecIpv4Rules.empty()) {
            // ipv4 check.
            fwIngressRules_["Ipv4Rule"]
                = std::make_unique< Ipv4Filter >(it.vecIpv4Rules, action_, 0, it.refillAmount, it.bucketSize);
        }
        if (!it.vecIpv6Rules.empty()) {
            // ipv6 check.
            fwIngressRules_["Ipv6Rule"]
                = std::make_unique< Ipv6Filter >(it.vecIpv6Rules, action_, 0, it.refillAmount, it.bucketSize);
        }
        if (!it.vecTcpRules.empty()) {
            // tcp check.
            fwIngressRules_["TcpRule"]
                = std::make_unique< TcpFilter >(it.vecTcpRules, action_, 0, it.refillAmount, it.bucketSize);
        }
        if (!it.vecUdpRules.empty()) {
            // udp check.
            fwIngressRules_["UdpRule"]
                = std::make_unique< UdpFilter >(it.vecUdpRules, action_, 0, it.refillAmount, it.bucketSize);
        }
        // do application protocol filter
        // someip.
        if (!it.vecSomeIpRules.empty()) {
            _GetSomeipRules(it.vecSomeIpRules);
        }
        // someipsd.
        if (!it.vecSomeIpSdRules.empty()) {
            _GetSomeipSdRules(it.vecSomeIpSdRules);
        }
        // doip.
        if (!it.vecDoIpRules.empty()) {
            _GetDoipRules(it.vecDoIpRules);
        }

        // dds.
        if (!it.veDdsRules.empty()) {
            _GetDdsRules(it.veDdsRules);
        }
        // do every level rules filter.
        std::ignore = _RulesCheck(kIngressType);
    }
}

/// @brief Firewall egress rule filtering
void FWFilterChain::_RulesCheckByGress() noexcept
{
    if (this->vecGressRules_.empty()) {
        LogError() << "_RulesCheckByGress: vecGressRules_ is null! please check "
                      "manifest config! ";
        return;
    }
    for (auto const &it : this->vecGressRules_) {
        if (!it.vecDataLinkRules.empty()) {
            // datalinkRules check.
            fwGressRules_["DataLinkLayerRule"]
                = std::make_unique< DataLinkFilter >(it.vecDataLinkRules, action_, 1, it.refillAmount, it.bucketSize);
        }
        if (!it.vecIpv4Rules.empty()) {
            // ipv4 check.
            fwGressRules_["Ipv4Rule"]
                = std::make_unique< Ipv4Filter >(it.vecIpv4Rules, action_, 1, it.refillAmount, it.bucketSize);
        }
        if (!it.vecIpv6Rules.empty()) {
            // ipv6 check.
            fwGressRules_["Ipv6Rule"]
                = std::make_unique< Ipv6Filter >(it.vecIpv6Rules, action_, 1, it.refillAmount, it.bucketSize);
        }
        if (!it.vecTcpRules.empty()) {
            // tcp check.
            fwGressRules_["TcpRule"]
                = std::make_unique< TcpFilter >(it.vecTcpRules, action_, 1, it.refillAmount, it.bucketSize);
        }
        if (!it.vecUdpRules.empty()) {
            // udp check.
            fwGressRules_["UdpRule"]
                = std::make_unique< UdpFilter >(it.vecUdpRules, action_, 1, it.refillAmount, it.bucketSize);
        }
        // do application protocol filter
        // someip.
        if (!it.vecSomeIpRules.empty()) {
            _GetSomeipRules(it.vecSomeIpRules);
        }
        // someipsd.
        if (!it.vecSomeIpSdRules.empty()) {
            _GetSomeipSdRules(it.vecSomeIpSdRules);
        }
        // doip.
        if (!it.vecDoIpRules.empty()) {
            _GetDoipRules(it.vecDoIpRules);
        }

        // dds.
        if (!it.veDdsRules.empty()) {
            _GetDdsRules(it.veDdsRules);
        }
        // do rules filter.
        std::ignore = _RulesCheck(kGressType);
    }
}

/// @brief Get SomeipRules rule set
/// @return
void FWFilterChain::_GetSomeipRules(const ara::core::Vector< PRuleParse > &vecSomeIpRules) noexcept
{
    LogInfo() << "_GetSomeipRules in!";
    for (auto const &it : vecSomeIpRules) {
        if (it.ruleName == "clientId") {
            this->vecClientid_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "lengthVerification") {
            it.ruleValue == "true" ? this->vecLengthVerify_.push_back(1) : this->vecLengthVerify_.push_back(0);
        }
        if (it.ruleName == "majorVersion") {
            this->vecMajorVersion_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "messageType") {
            this->vecMessageType_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "methodId") {
            this->vecMethodId_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "protocolVersion") {
            this->vecProtocolVersion_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "returnCode") {
            this->vecReturnCode_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "serviceInterfaceId") {
            this->vecServiceInterfaceId_.push_back(std::stoi(it.ruleValue.c_str()));
        }
    }
}

/// @brief Get SomeipSDRules rule set
/// @return
void FWFilterChain::_GetSomeipSdRules(const ara::core::Vector< PRuleParse > &vecSomeIpSdRules) noexcept
{
    LogInfo() << "_GetSomeipSdRules in!";
    for (auto const &it : vecSomeIpSdRules) {
        if (it.ruleName == "entryType") {
            this->vecEntryType_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "eventGroupId") {
            this->vecEventGroupId_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "maxMajorVersion") {
            this->vecMaxMajorVersion_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "minMajorVersion") {
            this->vecMinMajorVersion_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "maxMinorVersion") {
            this->vecMaxMinorVersion_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "minMinorVersion") {
            this->vecMinMinorVersion_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "serviceInstanceId") {
            this->vecServiceInstanceId_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "serviceInterfaceId") {
            this->vecSdServiceInterfaceId_.push_back(std::stoi(it.ruleValue.c_str()));
        }
    }
}

/// @brief Get DoipRules rule set
/// @return
void FWFilterChain::_GetDoipRules(const ara::core::Vector< PRuleParse > &vecDoipRules) noexcept
{
    LogInfo() << "_GetDoipRules in!";
    for (auto const &it : vecDoipRules) {
        if (it.ruleName == "destinationMaxAddress") {
            this->vecDestMaxAddr_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "destinationMinAddress") {
            this->vecDestMinAddr_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "inverseProtocolVersion") {
            this->vecInverseProtocolVersion_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "payloadLength") {
            this->vecPayloadLength_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "payloadType") {
            this->vecPayloadType_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "protocolVersion") {
            this->vecDoipProtocolVersion_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "sourceMaxAddress") {
            this->vecSrcMaxAddress_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "sourceMinAddress") {
            this->vecSrcMinAddress_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "udsService") {
            this->vecUdsService_.push_back(std::stoi(it.ruleValue.c_str()));
        }
    }
}
/// @brief Get DoipRules rule set
/// @return
void FWFilterChain::_GetDdsRules(const ara::core::Vector< PRuleParse > &vecDdsSdRules) noexcept
{
    LogInfo() << "_GetDdsRules in!";
    for (auto const &it : vecDdsSdRules) {
        if (it.ruleName == "appId") {
            this->vecAppId_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "hostId") {
            this->vecHostId_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "instanceId") {
            this->vecInstanceId_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "majorProtocolVersion") {
            this->vecMajorProtocolVersion_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "minorProtocolVersion") {
            this->vecMinorProtocolVersion_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "protocolId") {
            this->vecProtocolId_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "readerEntityId") {
            this->vecReaderEntityId_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "submessageType") {
            this->vecSubmessageType_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "vendorId") {
            this->vecVendorId_.push_back(std::stoi(it.ruleValue.c_str()));
        }
        if (it.ruleName == "writerEntityId") {
            this->vecWriterEntityId_.push_back(std::stoi(it.ruleValue.c_str()));
        }
    }
}

/// @brief Set APP rules and send to Linux kernel.
/// @return
void FWFilterChain::_SetAppRuleIntoKernal(int32_t inOutFlag) noexcept
{
    LogInfo() << "_GetDdsRules in!";

    ara::core::String strNums{};

    // someip Rules.
    if (!vecClientid_.empty()) {
        strNums = (inOutFlag == 1 ? "g_ClientId_In" : "g_ClientId_Out");
        _SetAppRuleData(vecClientid_, strNums);
    }
    if (!vecLengthVerify_.empty()) {
        strNums = (inOutFlag == 1 ? "g_Target_Length_Flag_In" : "g_Target_Length_Flag_Out");
        _SetAppRuleData(vecLengthVerify_, strNums);
    }
    if (!vecMajorVersion_.empty()) {
        strNums = (inOutFlag == 1 ? "g_Target_Major_Version_In" : "g_Target_Major_Version_Out");
        _SetAppRuleData(vecMajorVersion_, strNums);
    }
    if (!vecMessageType_.empty()) {
        strNums = (inOutFlag == 1 ? "g_Target_Msg_Type_In" : "g_Target_Msg_Type_Out");
        _SetAppRuleData(vecMessageType_, strNums);
    }
    if (!vecMethodId_.empty()) {
        strNums = (inOutFlag == 1 ? "g_Target_Method_Id_In" : "g_Target_Method_Id_Out");
        _SetAppRuleData(vecMethodId_, strNums);
    }
    if (!vecProtocolVersion_.empty()) {
        strNums = (inOutFlag == 1 ? "g_Target_Pro_Version_In" : "g_Target_Pro_Version_Out");
        _SetAppRuleData(vecProtocolVersion_, strNums);
    }
    if (!vecReturnCode_.empty()) {
        strNums = (inOutFlag == 1 ? "g_Target_Ret_Code_In" : "g_Target_Ret_Code_Out");
        _SetAppRuleData(vecReturnCode_, strNums);
    }

    if (!vecServiceInterfaceId_.empty()) {
        strNums = (inOutFlag == 1 ? "g_Target_Service_Id_In" : "g_Target_Service_Id_Out");
        _SetAppRuleData(vecServiceInterfaceId_, strNums);
    }
    // someip sd Rules
    if (!vecEntryType_.empty()) {
        strNums = (inOutFlag == 1 ? "g_Sd_EntType_In" : "g_Sd_EntType_Out");
        _SetAppRuleData(vecEntryType_, strNums);
    }
    if (!vecEventGroupId_.empty()) {
        strNums = (inOutFlag == 1 ? "g_Sd_GroupId_In" : "g_Sd_GroupId_Out");
        _SetAppRuleData(vecEventGroupId_, strNums);
    }
    if (!vecServiceInstanceId_.empty()) {
        strNums = (inOutFlag == 1 ? "g_Sd_ServiceInstId_In" : "g_Sd_ServiceInstId_Out");
        _SetAppRuleData(vecServiceInstanceId_, strNums);
    }
    if (!vecSdServiceInterfaceId_.empty()) {
        strNums = (inOutFlag == 1 ? "g_Sd_ServiceId_In" : "g_Sd_ServiceId_Out");
        _SetAppRuleData(vecSdServiceInterfaceId_, strNums);
    }
    // doip Rules.
    if (!vecDoipProtocolVersion_.empty()) {
        strNums = (inOutFlag == 1 ? "g_ProtocolVersion_In" : "g_ProtocolVersion_Out");
        _SetAppRuleData(vecDoipProtocolVersion_, strNums);
    }
    if (!vecInverseProtocolVersion_.empty()) {
        strNums = (inOutFlag == 1 ? "g_InverProtocolVersion_In" : "g_InverProtocolVersion_Out");
        _SetAppRuleData(vecInverseProtocolVersion_, strNums);
    }
    if (!vecPayloadType_.empty()) {
        strNums = (inOutFlag == 1 ? "g_PayType_In" : "g_PayType_Out");
        _SetAppRuleData(vecPayloadType_, strNums);
    }
    if (!vecPayloadLength_.empty()) {
        strNums = (inOutFlag == 1 ? "g_PayLoadLen_In" : "g_PayLoadLen_Out");
        _SetAppRuleData(vecPayloadLength_, strNums);
    }
    if (!vecUdsService_.empty()) {
        strNums = (inOutFlag == 1 ? "g_UdsService_In" : "g_UdsService_Out");
        _SetAppRuleData(vecUdsService_, strNums);
    }

    // defaultAction.
    _SetAppSingleRuleData(defaultAction_, "g_DefaultAction");
    // action
    _SetAppSingleRuleData(action_, "g_Action");
    // inoutFlag in:1
    //_SetAppSingleRuleData(1, "g_InOutFlag");
}

/// @brief Set APP layer parameters to the corresponding kernel parameters.
/// @param vec Data
/// @param str
/// @return
void FWFilterChain::_SetAppRuleData(ara::core::Vector< int32_t > const &vec, ara::core::String const &strPar) noexcept
{
    std::stringstream ss{};
    ara::core::String str{};
    ara::core::String cmd{};

    // Return directly if no data.
    if (vec.empty()) {
        return;
    }
    // only one.
    if (vec.size() == 1) {
        str = std::to_string(vec[0]);
    } else {
        ss << vec[0];
        for (size_t cnt = 1; cnt < vec.size(); ++cnt) {
            ss << "," << vec[cnt];
        }
        str = ss.str();
    }
    cmd = GetkAppEchoCmd() + str + GetkAppSysDirCmd() + strPar;
    LogDebug() << "[_SetAppRuleData] CallSystem cmd is :" << cmd.c_str();

    if (!CallSystem(cmd)) {
        LogError() << "_SetAppRuleData: set applayer cmd  failed!";
        return;
    }
}

/// @brief Set APP layer parameters to the corresponding kernel parameters.
/// @param ruleData Data
/// @param strPar
/// @return
void FWFilterChain::_SetAppSingleRuleData(int32_t ruleData, ara::core::String const &strPar) noexcept
{
    std::stringstream ss{};
    ara::core::String str{};
    ara::core::String cmd{};

    str = std::to_string(ruleData);

    cmd = GetkAppEchoCmd() + str + GetkAppSysDirCmd() + strPar;
    LogDebug() << "[_SetAppSingleRuleData] CallSystem cmd is :" << cmd.c_str();

    if (!CallSystem(cmd)) {
        LogError() << "_SetAppSingleRuleData: set applayer cmd  failed!";
        return;
    }
}

}  // namespace internal
}  // namespace fw
}  // namespace ara
