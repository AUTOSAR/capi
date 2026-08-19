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
/// @file       fw_filter_chain.h
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
/// export_level=module
/// @trace_id_sr=SR_FW_0006,SR_FW_0007,SR_FW_0008
/// @unit_name=Filter_Chain
/// @unit_description=Firewall filter chain management class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_FILTER_CHAIN_H_
#define ARA_FW_FILTER_CHAIN_H_
#include <set>

#include "ara/fw/internal/manifest_data.h"
#include "config/config_manager.h"
#include "filter/filterbase.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief fw filter chain.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00407
/// @trace_id_dd=DD_FW_00546
/// @needwork = ad
/// @endcode
class FWFilterChain
{
public:
    /// @brief Modify map alias.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00407
    /// @trace_id_dd=DD_FW_00546
    /// @needwork = ad
    /// @endcode
    using M_Rules = ara::core::Map< ara::core::String, std::unique_ptr< FilterBase > >;

public:
    /// @brief fw filter chain processing constructor
    /// @param ptrConfigManager Configuration management object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00408
    /// @trace_id_dd=DD_FW_00547
    /// @needwork = ad
    /// @endcode
    explicit FWFilterChain(std::shared_ptr< ConfigManager > const &ptrConfigManager) noexcept;

    /// @brief set the instance descriptor and firewall state that need to be
    /// configured for fw firewall.
    /// @param inst Client instance
    /// @param fwState Firewall state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00409
    /// @trace_id_dd=DD_FW_00548
    /// @needwork = ad
    /// @endcode
    void SetFWStateAndInstance(ara::core::String const &inst, uint32_t fwState) noexcept;

    /// @brief Filter rule entry
    /// @return success/failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00410
    /// @trace_id_dd=DD_FW_00549
    /// @needwork = ad
    /// @endcode
    bool AllRulesCheck() noexcept;

    /// @brief  GetFWRulePropsAndDefaultAction
    /// @return success/failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00411
    /// @trace_id_dd=DD_FW_00550
    /// @needwork = ad
    /// @endcode
    bool GetFWRulePropsAndDefaultAction() noexcept;

    /// @brief fw creates nft tables and chains.
    /// @return success/failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00412
    /// @trace_id_dd=DD_FW_00551
    /// @needwork = ad
    /// @endcode
    bool CreateTableAndChain() const noexcept;

    /// @brief Clear application layer filter rules
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00413
    /// @trace_id_dd=DD_FW_00552
    /// @needwork = ad
    /// @endcode
    void ClearAppRules() noexcept;

protected:
    /// @brief for loop to clear firewallRule.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00553
    /// @needwork = dda
    /// @endcode
    void _DestoryRules() noexcept;

    /// @brief Reset all APP layer rule sets.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00554
    /// @needwork = dda
    /// @endcode
    void _DestoryAppRules() noexcept;

    /// @brief Reset APP layer ingress rules to -1.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00555
    /// @needwork = dda
    /// @endcode
    void _DestoryAppInRules() noexcept;

    /// @brief After each FW rule input is completed, clear the created tables and
    /// chains.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00556
    /// @needwork = dda
    /// @endcode
    void _DestoryCmd() noexcept;

    /// @brief Firewall rule filtering
    /// @param filterType Filter ingress/egress
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00557
    /// @needwork = dda
    /// @endcode
    bool _RulesCheck(uint32_t const filterType) noexcept;

    /// @brief Create egress rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00558
    /// @needwork = dda
    /// @endcode
    void _RulesCheckByIngress() noexcept;

    /// @brief Create ingress rule set
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00559
    /// @needwork = dda
    /// @endcode
    void _RulesCheckByGress() noexcept;

    /// @brief Get SomeipRules rule set
    /// @param vecSomeIpRules someip rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00560
    /// @needwork = dda
    /// @endcode
    void _GetSomeipRules(const ara::core::Vector< PRuleParse > &vecSomeIpRules) noexcept;

    /// @brief Get SomeipsdRules rule set
    /// @param vecSomeIpSdRules someipsd rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00561
    /// @needwork = dda
    /// @endcode
    void _GetSomeipSdRules(const ara::core::Vector< PRuleParse > &vecSomeIpSdRules) noexcept;

    /// @brief Get SomeipsdRules rule set
    /// @param vecDoipRules doip rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00562
    /// @needwork = dda
    /// @endcode
    void _GetDoipRules(const ara::core::Vector< PRuleParse > &vecDoipRules) noexcept;

    /// @brief Get SomeipsdRules rule set
    /// @param vecDdsRules dds rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00563
    /// @needwork = dda
    /// @endcode
    void _GetDdsRules(const ara::core::Vector< PRuleParse > &vecDdsRules) noexcept;

    /// @brief Set APP rules and send to Linux kernel.
    /// @param inOutFlag IN/OUT Flag.
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00564
    /// @needwork = dda
    /// @endcode
    void _SetAppRuleIntoKernal(int32_t inOutFlag) noexcept;

    /// @brief Set APP layer array data to the kernel.
    /// @param vec Data
    /// @param strPar Setting
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00565
    /// @needwork = dda
    /// @endcode
    void _SetAppRuleData(ara::core::Vector< int32_t > const &vec, ara::core::String const &strPar) noexcept;
    /// @brief Set a single APP layer rule parameter to the corresponding kernel
    /// parameter.
    /// @param ruleData Data
    /// @param strPar Single rule string
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00566
    /// @needwork = dda
    /// @endcode
    void _SetAppSingleRuleData(int32_t ruleData, ara::core::String const &strPar) noexcept;

private:
    /// @brief Firewall state obtained via IPC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00567
    /// @needwork = dda
    /// @endcode
    uint32_t fwState_{0U};

    /// @brief Client instance obtained via IPC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00568
    /// @needwork = dda
    /// @endcode
    ara::core::String instSpec_{};

    /// @brief Configuration management pointer.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00569
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ConfigManager > configManagerPtr_{nullptr};

    /// @brief Firewall ingress rule map.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00570
    /// @needwork = dda
    /// @endcode
    M_Rules fwIngressRules_{};

    /// @brief Firewall egress rule map.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00571
    /// @needwork = dda
    /// @endcode
    M_Rules fwGressRules_{};

private:
    /// @brief firewall rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00572
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PFirewallRuleProps > vecRuleProps_{};
    /// @brief Ingress rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00573
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PFirewallRule > vecIngressRules_{};
    /// @brief Egress rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00574
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PFirewallRule > vecGressRules_{};

    /// @brief Current action. (0: block | 1: allow)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00575
    /// @needwork = dda
    /// @endcode
    int32_t action_{-1};

    /// @brief Default action. (0: block | 1: allow)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00576
    /// @needwork = dda
    /// @endcode
    int32_t defaultAction_{-1};

    // Get payload network interface name.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00577
    /// @needwork = dda
    /// @endcode
    ara::core::String netCardName_{};

private:
    /// @brief Clientid
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00578
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecClientid_{};
    /// @brief veLengthVerify
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00579
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecLengthVerify_{};
    /// @brief vecMajorVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00580
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecMajorVersion_{};
    /// @brief vecMessageType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00581
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecMessageType_{};
    /// @brief vecMethodId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00582
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecMethodId_{};
    /// @brief vecProtocolVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00583
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecProtocolVersion_{};
    /// @brief vecReturnCode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00584
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecReturnCode_{};
    /// @brief vecServiceInterfaceId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00585
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecServiceInterfaceId_{};
    // SOMEIP SD Rules
    /// @brief vecEntryType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00586
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecEntryType_{};
    /// @brief vecEventGroupId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00587
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecEventGroupId_{};
    /// @brief vecMaxMajorVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00588
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecMaxMajorVersion_{};
    /// @brief vecMaxMinorVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00589
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecMaxMinorVersion_{};
    /// @brief vecMinMajorVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00590
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecMinMajorVersion_{};
    /// @brief vecMinMinorVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00591
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecMinMinorVersion_{};
    /// @brief vecServiceInstanceId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00592
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecServiceInstanceId_{};
    /// @brief vecSdServiceInterfaceId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00593
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecSdServiceInterfaceId_{};

    // Doip Rules
    /// @brief vecDestMaxAddr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00594
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecDestMaxAddr_{};
    /// @brief vecDestMinAddr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00595
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecDestMinAddr_{};
    /// @brief vecInverseProtocolVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00596
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecInverseProtocolVersion_{};
    /// @brief vecProtocolVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00597
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecDoipProtocolVersion_{};
    /// @brief vecPayloadType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00598
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecPayloadType_{};
    /// @brief vecMajorVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00599
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecPayloadLength_{};
    /// @brief vecSrcMaxAddress
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00600
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecSrcMaxAddress_{};
    /// @brief vecSrcMinAddress
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00601
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecSrcMinAddress_{};
    /// @brief vecUdsService
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00602
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecUdsService_{};

    // DDS Rules

    /// @brief vecAppId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00603
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecAppId_{};
    /// @brief vecHostId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00604
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecHostId_{};
    /// @brief vecInstanceId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00605
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecInstanceId_{};
    /// @brief vecMajorProtocolVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00606
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecMajorProtocolVersion_{};
    /// @brief vecMinorProtocolVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00607
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecMinorProtocolVersion_{};
    /// @brief vecProductId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00608
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecProtocolId_{};
    /// @brief vecReaderEntityId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00609
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecReaderEntityId_{};
    /// @brief vecWriterEntityId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00610
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecWriterEntityId_{};
    /// @brief vecSubmessageType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00611
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecSubmessageType_{};
    /// @brief vecVendorId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00612
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< int32_t > vecVendorId_{};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif