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
/// @file       config_manager.h
/// @brief      fw configuration management
/// @details    fw configuration management
/// @date       2024-12-20
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/FW/Configuration Managerment
/// @interface_level=module
/// export_level=/FW/Configuration Managerment
/// @trace_id_sr=SR_FW_0003
/// @unit_name=ConfigManager
/// @unit_description=Configuration management processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_CONFIG_MANAGER_H_
#define ARA_FW_CONFIG_MANAGER_H_
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>
#include <isoft/manifestreader/tps_enumeration.h>

#include <set>

#include "ara/fw/common/common.h"
#include "ara/fw/internal/manifest_data.h"
#include "ara/fw/internal/serialization_error_domain.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief Configuration management class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00523
/// @needwork = ad
/// @endcode
class ConfigManager final
{
public:
    /// @brief ConfigManager constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00392
    /// @trace_id_dd=DD_FW_00524
    /// @needwork = ad
    /// @endcode
    ConfigManager() = default;
    /// @brief ConfigManager destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00393
    /// @trace_id_dd=DD_FW_00525
    /// @needwork = ad
    /// @endcode
    ~ConfigManager() = default;

    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00394
    /// @trace_id_dd=DD_FW_00526
    /// @needwork = ad
    /// @endcode
    ConfigManager(ConfigManager &&other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00395
    /// @trace_id_dd=DD_FW_00527
    /// @needwork = ad
    /// @endcode
    ConfigManager &operator=(ConfigManager &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00396
    /// @trace_id_dd=DD_FW_00528
    /// @needwork = ad
    /// @endcode
    ConfigManager &operator=(ConfigManager const &other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class The return value of
    /// this function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00397
    /// @trace_id_dd=DD_FW_00529
    /// @needwork = ad
    /// @endcode
    ConfigManager(ConfigManager const &other) = delete;

public:
    /// @brief Entry function for parsing configuration
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00398
    /// @trace_id_dd=DD_FW_00530
    /// @needwork = ad
    /// @endcode
    bool InitManifest() noexcept;

    /// @brief Parse configuration file and store.
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00399
    /// @trace_id_dd=DD_FW_00531
    /// @needwork = ad
    /// @endcode
    bool ManifestParse() noexcept;

    /// @brief Get the defaultaction corresponding to the fw state.
    /// @param fwState fw state
    /// @return defaultaction(0:block,1 allow)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00400
    /// @trace_id_dd=DD_FW_00532
    /// @needwork = ad
    /// @endcode
    uint32_t GetDefaultActionByFWState(uint32_t const &fwState) noexcept;

    /// @brief Query whether the firewall state corresponding to the client
    /// instance exists
    /// @param instance Client instance
    /// @param enumfwState Firewall state value enumeration
    /// @return  true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00401
    /// @trace_id_dd=DD_FW_00533
    /// @needwork = ad
    /// @endcode
    bool QueryFWStateExist(ara::core::String const &instance, uint32_t const &enumfwState) noexcept;

    /// @brief Get the instance and firewall rule set map
    /// @return MAP_InstanceToStateDepFW
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00402
    /// @trace_id_dd=DD_FW_00534
    /// @needwork = ad
    /// @endcode
    MAP_InstanceToStateDepFW const &GetInstanceAndStateDepFirewall() const noexcept;

    /// @brief Get the firewall rule set corresponding to the firewall state.
    /// @param fwState fw state
    /// @return fwRulesProp corresponding to the firewall state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00403
    /// @trace_id_dd=DD_FW_00535
    /// @needwork = ad
    /// @endcode
    std::set< PFirewallRuleProps > const *GetFWRulesPorpByFWState(uint32_t const &fwState) noexcept;

    /// @brief Get the defaultaction corresponding to the firewall state.
    /// @param fwState fw state
    /// @return defaultaction corresponding to the firewall state.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00404
    /// @trace_id_dd=DD_FW_00536
    /// @needwork = ad
    /// @endcode
    int32_t GetFWDefaultActionByFWState(uint32_t const &fwState) noexcept;

    /// @brief Get firewall rule Props
    /// @param inst Client instance value
    /// @param fwState Firewall state value
    /// @param fwRuleProps Firewall rule set
    /// @param defAction Default action
    /// @return Success/Failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00405
    /// @trace_id_dd=DD_FW_00537
    /// @needwork = ad
    /// @endcode
    bool GetFWPropsRules(SpecifierType const &inst,
                         uint32_t const &fwState,
                         ara::core::Vector< PFirewallRuleProps > &fwRuleProps,
                         int32_t &defAction) noexcept;

    /// @brief Get firewall configuration path
    /// @return Get firewall configuration path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00406
    /// @trace_id_dd=DD_FW_00538
    /// @needwork = ad
    /// @endcode
    ara::core::String const GetConfigFileDirectory() noexcept;

private:
    /// @brief  instance----> fw statedepfirewall+enum fw state.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00539
    /// @needwork = dda
    /// @endcode
    MAP_InstanceToStateDepFW mapInstanceToStateDepFW_{};

    /// @brief instance--->fw states mapping for finding firewall state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00540
    /// @needwork = dda
    /// @endcode
    MAP_InstanceToFWSate mapInstToState_{};

    /// @brief  fwstate----> defaultaction.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00541
    /// @needwork = dda
    /// @endcode
    MAP_FWStateToDefaultAction mapFWStateToDefaultAction_{};

    /// @brief  fwstate----> firewallRuleProps.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00542
    /// @needwork = dda
    /// @endcode
    MAP_FWStateTofirewallRuleProps mapFWStateToFWRulesProps_{};

    // Members within a single firewallRuleProps structure.

    /// @brief fw rule action(0:block|1:allow)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00543
    /// @needwork = dda
    /// @endcode
    uint32_t fwRuleAction_{0};

    /// @brief Firewall ingress rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00544
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PFirewallRule > vecInPutRules_{};

    /// @brief Firewall egress rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00545
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PFirewallRule > vecOutPutRules_{};
};
}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif  ///
