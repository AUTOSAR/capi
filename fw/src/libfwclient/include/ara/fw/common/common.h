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
/// @file       common.h
/// @brief      Common processing
/// @details    Common processing
/// @date       2024-11-26
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/common processing
/// @interface_level=module
/// @trace_id_sr=SR_FW_0002,SR_FW_0003,SR_FW_0004,SR_FW_0005
/// @unit_name=Firewall_Common
/// @unit_description=Firewall type definition file
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_COMMON_H_
#define ARA_FW_COMMON_H_
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <cstdint>
#include <set>

#include "ara/fw/internal/manifest_data.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief instance---->fw_enum_states
using char8_t = char;  // NOLINT : Type definition

/// @brief Type redefinition: Define the data structure of byte stream
using Chunk = ara::core::Vector< std::uint8_t >;

/// types define.
/// @brief instance---->fw_enum_states
using MAP_InstanceToFWSate = ara::core::Map< ara::core::String, std::set< uint32_t > >;
/// @brief enum_fw_states --->state_dep_fw
using MAP_EnumFWStateToStateDepFW = ara::core::Map< uint32_t, PStatesDepFirewall >;
/// @brief instance --->statedepFirewall+fw_enum_states
using MAP_InstanceToStateDepFW = ara::core::Map< ara::core::String, PInstanceToFWRules >;
/// @brief  instance vector.
using VECTOR_Instance = ara::core::Vector< ara::core::String >;
/// @brief  fwstate--->defaultaction.
using MAP_FWStateToDefaultAction = ara::core::Map< uint32_t, int32_t >;
/// @brief  fwstate--->firewallRuleProps.
using MAP_FWStateTofirewallRuleProps = ara::core::Map< uint32_t, std::set< PFirewallRuleProps > >;
/// @brief vector ingress/gress firewallrules.
using VECTOR_FireWallRules = ara::core::Vector< PFirewallRule >;
/// @brief  fwstate--->firewallRules.
using MAP_FWStateToFirewallRules = ara::core::Map< uint32_t, std::set< PFirewallRule > >;

/// @brief  change name
using SpecifierType = ara::core::String;
/// @brief  change name
using FWState = int32_t;
/// @brief  change name
using InstanceType = int32_t;

/// @brief ipv4 protocol type.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00161
/// @needwork = dd
/// @endcode
enum class Ipv4Protocol : uint32_t
{
    kIcmp = 1,
    kTcp  = 6,
    kUdp  = 17
};

/// @brief Firewall
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00162
/// @needwork = dd
/// @endcode
enum class FirewallActionEnum : uint8_t
{
    kBlock = 0,
    kAllow = 1
};

/// @brief Firewall rule set ingress type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00163
/// @needwork = dd
/// @endcode
constexpr uint32_t kIngressType{1};
/// @brief Firewall rule set egress type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00164
/// @needwork = dd
/// @endcode
constexpr uint32_t kGressType{2};

/// @brief Firewall APP layer maximum firewall rule count
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00165
/// @needwork = dd
/// @endcode
constexpr uint32_t kAppMaxRules{1};

/// @brief Firewall rule set egress type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00166
/// @needwork = dd
/// @endcode
constexpr int32_t kPayloadOffset{8};

/// @brief log synchronization fetch mac range
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00167
/// @needwork = dd
/// @endcode
constexpr int32_t kPayloadOffset17{17};

/// @brief Firewall rule set egress type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00168
/// @needwork = dd
/// @endcode
constexpr int32_t kPayloadOffset18{18};

}  // namespace internal
}  // namespace fw
}  // namespace ara

#endif