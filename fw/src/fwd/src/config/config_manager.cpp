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
/// @file       config_manager.cpp
/// @brief      fw configuration management
/// @details    fw configuration management
/// @date       2024-12-26
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/FW/Configuration Managerment
/// @interface_level=module
/// @trace_id_sr=SR_FW_0003
/// @unit_name=ConfigManager
/// @unit_description=Configuration management processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "config_manager.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/manifestreader/manifest.h>

#include "ara/fw/internal/log_api.h"
#include "ara/fw/internal/manifest_data.h"
#include "ara/fw/internal/manifest_fw_common.h"
#include "ara/fw/internal/manifest_fw_syntax.h"

namespace manifest = ara::fw::internal;

// payloadPyttenPart.

// PFirewallRuleProps
// PRQA S 2024,2025 QAC /// @qac: Cannot modify inside macro definition
MR_HEADER(manifest::PFirewallRuleProps)
MR_FIELD(manifest::GetkAction(), action)
MR_FIELD(manifest::GetkFWEgressRule(), vecEgressRules)
MR_FIELD(manifest::GetkFWIngressRule(), vecIngressRules)
MR_FOOTER

/// firewallRule
MR_HEADER(manifest::PFirewallRule)
MR_FIELD(manifest::GetkBucketSize(), bucketSize)
MR_FIELD(manifest::GetkRefillAmount(), refillAmount)
MR_FIELD(manifest::GetkDataLinkLayerRule(), vecDataLinkRules)
MR_FIELD(manifest::GetkIpv4Rule(), vecIpv4Rules)
MR_FIELD(manifest::GetkIpv6Rule(), vecIpv6Rules)
MR_FIELD(manifest::GetkTcpRule(), vecTcpRules)
MR_FIELD(manifest::GetkUdpRule(), vecUdpRules)
MR_FIELD(manifest::GetkSomeIpRule(), vecSomeIpRules)
MR_FIELD(manifest::GetkSomeIpSdRule(), vecSomeIpSdRules)
MR_FIELD(manifest::GetkDdsRule(), veDdsRules)
MR_FIELD(manifest::GetkDoIpRule(), vecDoIpRules)
MR_FIELD(manifest::GetkPayloadRule(), vecPayloadRules)
MR_FOOTER

#if 0
///DataLinkLayerRule
MR_HEADER(manifest::PDatalinkLayerRule)
MR_FIELD(manifest::GetkDataLinkLayerRule(), vecDataLinkRules)
MR_FOOTER

///Ipv4Rule
MR_HEADER(manifest::PIpv4Rule)
MR_FIELD(manifest::GetkIpv4Rule(), vecIpv4Rules)
MR_FOOTER
///Ipv6Rule
MR_HEADER(manifest::PIpv6Rule)
MR_FIELD(manifest::GetkIpv6Rule(), vecIpv6Rules)
MR_FOOTER

///TcpRule
MR_HEADER(manifest::PTcpRule)
MR_FIELD(manifest::GetkTcpRule(), vecTcpRules)
MR_FOOTER

///UdpRule
MR_HEADER(manifest::PUdpRule)
MR_FIELD(manifest::GetkUdpRule(), vecUdpRules)
MR_FOOTER
#endif

/// PRuleParse
MR_HEADER(manifest::PPayloadRulePart)
MR_FIELD(manifest::GetkOffset(), offset)
MR_FIELD(manifest::GetkValue(), value)
MR_FOOTER

/// PRuleParse
MR_HEADER(manifest::PRuleParse)
MR_FIELD(manifest::GetkRuleName(), ruleName)
MR_FIELD(manifest::GetkRuleData(), ruleValue)
MR_FOOTER

// clientinstance -->stateDepfirewall
MR_HEADER(manifest::PInstanceToFWRules)
MR_FIELD(manifest::GetkFWInstanceName(), strInstName)
MR_FIELD(manifest::GetkStateDepFW(), vecStateDepFW)
MR_FIELD(manifest::GetkFWStatesAll(), vecFWStateEnums)
MR_FOOTER

// stateDepFirewall
MR_HEADER(manifest::PStatesDepFirewall)
MR_FIELD(manifest::GetkFWState(), fwState)
MR_FIELD(manifest::GetkDefaultAction(), defaultAction)
MR_FIELD(manifest::GetkFWRuleProps(), vecRuleProps)
MR_FOOTER

// firewallStatusAll
MR_HEADER(manifest::PStatusAll)
MR_FIELD(manifest::GetkFWEnumState(), uStatusValue)
MR_FOOTER
#if 0
//MatchingGressRule
MR_HEADER(manifest::PMatchingGressRule)
MR_FIELD(manifest::GetkFWEgressRule(), vecFWRules)
MR_FOOTER

//MatchingIngressRule
MR_HEADER(manifest::PMatchingGressRule)
MR_FIELD(manifest::GetkFWIngressRule(), vecFWRules)
MR_FOOTER
#endif

namespace ara {
namespace fw {
namespace internal {

/// @brief Entry function for parsing configuration
/// @return true/false
bool ConfigManager::InitManifest() noexcept
{
    ara::core::String jsonParseFile{GetConfigFileDirectory()};
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(jsonParseFile)};

    if (!manifestRes.HasValue()) {
        LogError() << "ConfigManager::InitManifest OpenManifest failed!";
        return false;
    }
    std::unique_ptr< isoft::manifestreader::Manifest > const pManifestRes{std::move(manifestRes).Value()};
    LogInfo() << "ConfigManager::InitManifest jsonName = " << jsonParseFile.data();

    // instance parse.
    std::ignore = ReadMapData< PInstanceToFWRules >(
        pManifestRes, ara::core::String(GetkRequireInstancesName()),
        [this](PInstanceToFWRules const &data) -> void { mapInstanceToStateDepFW_[data.strInstName] = data; });

    // manifest parse.
    if (!ManifestParse()) {
        LogError() << "ConfigManager::InitManifest ManifestParse failed!";
        return false;
    }
    return true;
}

/// @brief manifest file parsing and storage
/// @return true/false
bool ConfigManager::ManifestParse() noexcept
{
    LogDebug() << "ManifestParse in!";

    if (mapInstanceToStateDepFW_.empty()) {
        LogError() << "ManifestParse: mapInstanceToStateDepFW_ is null!";
        return false;
    }
    // Loop instance--->stateDepFirewall map.
    for (auto const &it : mapInstanceToStateDepFW_) {
        if (it.second.vecFWStateEnums.empty() || it.second.vecStateDepFW.empty()) {
            LogError() << "ManifestParse: mapInstanceToStateDepFW_.vecFWStateEnums "
                          "or vecStateDepFW null!";
            return false;
        }

        for (auto const &itStatusAll : it.second.vecFWStateEnums) {
            mapInstToState_[it.first].insert(itStatusAll.uStatusValue);
        }

        // vector ---->stateDepFirewall loop
        for (auto const &itStateDepFW : it.second.vecStateDepFW) {
            // fwstate---->fwRuleProps.

            mapFWStateToFWRulesProps_[itStateDepFW.fwState].insert(itStateDepFW.vecRuleProps.begin(),
                                                                   itStateDepFW.vecRuleProps.end());

            // fwstate----->defaultaction.
            mapFWStateToDefaultAction_.emplace(itStateDepFW.fwState, itStateDepFW.defaultAction);
        }
    }
    return true;
}

/// @brief
/// @return
MAP_InstanceToStateDepFW const &ConfigManager::GetInstanceAndStateDepFirewall() const noexcept
{
    return mapInstanceToStateDepFW_;
}

/// @brief Get firewall rule set
/// @param inst Client instance
/// @param fwState Firewall state
/// @param fwRuleProps Firewall rule set
/// @param defAction Default rule.
/// @return true/false
bool ConfigManager::GetFWPropsRules(SpecifierType const &inst,
                                    uint32_t const &fwState,
                                    ara::core::Vector< PFirewallRuleProps > &fwRuleProps,
                                    int32_t &defAction) noexcept
{
    auto const &instToStateDepFW{mapInstanceToStateDepFW_.find(inst)};
    if (instToStateDepFW == mapInstanceToStateDepFW_.end()) {
        LogError() << "GetInPutRules: not found instance from manifest! ";
        return false;
    }
    // stateDepFw  loop
    for (auto const &stateDepFW : instToStateDepFW->second.vecStateDepFW) {
        // find matching firewall state
        if (stateDepFW.fwState == fwState) {
            defAction   = stateDepFW.defaultAction;
            fwRuleProps = stateDepFW.vecRuleProps;
            LogInfo() << "GetFWPropsRules: Get defaultaction and RuleProps! ";
            return true;
        }
    }
    return false;
}

/// @brief Query whether the firewall state corresponding to the client instance
/// exists
/// @param instance Instance name
/// @param EnumfwState Firewall state enumeration
/// @return  true/false
bool ConfigManager::QueryFWStateExist(ara::core::String const &instance, uint32_t const &enumfwState) noexcept
{
    for (const auto &e : mapInstToState_) {
        LogInfo() << "QueryFWStateExist:instName:" << e.first;
        for (const auto &e1 : e.second) {
            LogInfo() << "QueryFWStateExist:fwStatus:" << e1;
        }
    }

    std::set< uint32_t > setFWQuery{mapInstToState_[instance]};
    auto const itFind{setFWQuery.find(enumfwState)};
    if (itFind != setFWQuery.end()) {
        LogInfo() << "QueryFWStateExist: Get fwstate by instance.!";
        return true;
    }
    return false;
}

/// @brief Get the firewall rule set corresponding to the firewall state.
/// @param fwState Firewall state
/// @return fwRulesProp corresponding to the firewall state
std::set< PFirewallRuleProps > const *ConfigManager::GetFWRulesPorpByFWState(uint32_t const &fwState) noexcept
{
    auto const itFind{mapFWStateToFWRulesProps_.find(fwState)};
    if (itFind == mapFWStateToFWRulesProps_.end()) {
        LogError() << "GetFWRulesPorpByFWState: not Get firewallProp by fwstate!";
        return nullptr;
    }
    return &(itFind->second);
}

/// @brief Get the defaultaction corresponding to the firewall state.
/// @param fwState Firewall state
/// @return defaultaction corresponding to the firewall state.
int32_t ConfigManager::GetFWDefaultActionByFWState(uint32_t const &fwState) noexcept
{
    auto itFind{mapFWStateToDefaultAction_.find(fwState)};
    if (itFind == mapFWStateToDefaultAction_.end()) {
        LogError() << "GetFWDefaultActionByFWState: not Get defaultaction by fwstate!";
        return -1;
    }
    return itFind->second;
}

/// @brief Get the defaultaction corresponding to the firewall state.
/// @param fwState
/// @return defaultaction corresponding to the firewall state.
ara::core::String const ConfigManager::GetConfigFileDirectory() noexcept
{
    ara::core::String stConfigFileName;
    isoft::ara_fsh::Platform const platForm;
    stConfigFileName = platForm.GetPlatformEtcDir();
    stConfigFileName += GetkFirewallFileName();
    return stConfigFileName;
}

}  // namespace internal
}  // namespace fw
}  // namespace ara
