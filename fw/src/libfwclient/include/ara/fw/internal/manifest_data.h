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
/// @file       manifest_data.h
/// @brief      fw module
/// @details    Data structures needed for firewall parsing configuration files.
/// @date       2024-12-24
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/FW/configuration management
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0003
/// @unit_name=Manifetst_Data
/// @unit_description=Configuration file structure generation class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_MANIFEST_DATA_H_
#define ARA_FW_MANIFEST_DATA_H_
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/vector.h"

namespace ara {
namespace fw {
namespace internal {
//********************************/

/// @brief fw structure corresponding to each client instance.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00177
/// @trace_id_dd=DD_FW_00260
/// @needwork = ad
/// @endcode
class PStatusAll
{
public:
    /// @brief firewall state enumeration value.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00261
    /// @needwork = dda
    /// @endcode
    uint32_t uStatusValue;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00178
    /// @trace_id_dd=DD_FW_00262
    /// @needwork = ad
    /// @endcode
    bool operator<(const PStatusAll &other) const { return uStatusValue < other.uStatusValue; }
};

/// @brief Rule parsing structure name: data:
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00179
/// @trace_id_dd=DD_FW_00263
/// @needwork = ad
/// @endcode
class PRuleParse
{
public:
    /// @brief Rule name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00264
    /// @needwork = dda
    /// @endcode
    ara::core::String ruleName;
    /// @brief Rule value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00265
    /// @needwork = dda
    /// @endcode
    ara::core::String ruleValue;
};

/// @brief Data link layer rule parsing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00180
/// @trace_id_dd=DD_FW_00266
/// @needwork = ad
/// @endcode
class PDatalinkLayerRule
{
public:
    /// @brief datalink processing class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00267
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecDataLinkRules;
};

/// @brief ipv4 rule parsing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00181
/// @trace_id_dd=DD_FW_00268
/// @needwork = ad
/// @endcode
class PIpv4Rule
{
public:
    /// @brief ipv4 rule filtering
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00269
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecIpv4Rules;
};

/// @brief ipv6 rule parsing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00182
/// @trace_id_dd=DD_FW_00270
/// @needwork = ad
/// @endcode
class PIpv6Rule
{
public:
    /// @brief ipv6 rule filtering
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00271
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecIpv6Rules;
};

/// @brief TCP rule parsing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00183
/// @trace_id_dd=DD_FW_00272
/// @needwork = ad
/// @endcode
class PTcpRule
{
public:
    /// @brief tcp rule filtering
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00273
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecTcpRules;
};

/// @brief UDP rule parsing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00184
/// @trace_id_dd=DD_FW_00274
/// @needwork = ad
/// @endcode
class PUdpRule
{
public:
    /// @brief udp rule filtering
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00275
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecUdpRules;
};

/// @brief some/ip rule parsing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00185
/// @trace_id_dd=DD_FW_00276
/// @needwork = ad
/// @endcode
class PSomeIpRule
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00277
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecSomeIpRules;
};

/// @brief some/ip sd rule parsing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00186
/// @trace_id_dd=DD_FW_00278
/// @needwork = ad
/// @endcode
class PSomeIpSdRule
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00279
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecSomeIpSdRules;
};

/// @brief dds rule parsing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00187
/// @trace_id_dd=DD_FW_00280
/// @needwork = ad
/// @endcode
class PDdsRule
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00281
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecDdsRules;
};

/// @brief DoIP rule parsing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00188
/// @trace_id_dd=DD_FW_00282
/// @needwork = ad
/// @endcode
class PDoIpRule
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00283
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecDoIpRules;
};

/// @brief Common rule parsing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00189
/// @trace_id_dd=DD_FW_00284
/// @needwork = ad
/// @endcode
class PPayloadRulePart
{
public:
    /// @brief Rule name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00285
    /// @needwork = dda
    /// @endcode
    ara::core::String offset;
    /// @brief Rule value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00286
    /// @needwork = dda
    /// @endcode
    ara::core::String value;
};

// /// @brief Common rule parsing
// class PPayloadRule
// {
// public:
//     ara::core::Vector< PPayloadRulePart > vecPayloadRules;
// };

// old define
#ifdef ARA_FW_OLD_RULES_DEFINE
/// @brief Data link layer rule parsing
class PDatalinkLayerRule
{
public:
    /// @brief Destination address
    ara::core::String destMacAddress;
    /// @brief Destination address Mask
    ara::core::String destMacAddressMask;
    /// @brief Ethernet type
    uint32_t etherType;
    /// @brief VLAN ID
    uint16_t vLanId;
    /// @brief VLAN Priority
    uint16_t vlanPriority;
};

/// @brief ipv4 rule parsing
class PIpv4Rule
{
public:
    /// @brief Checksum
    bool checksumVerification;
    /// @brief Destination address
    ara::core::String destAddress;
    /// @brief Destination address Mask
    ara::core::String destAddressMask;
    /// @brief DSCP value.
    uint8_t dscpVaule;
    /// @brief Fragment flag
    bool doNotFragment;
    /// @brief ECN value
    uint16_t vlanPriority;
    /// @brief icmp checksum
    bool icmpChecksumVerification;
    /// @brief icmp code
    uint8_t icmpCode;
    /// @brief icmp type
    uint8_t icmpType;
    /// @brief internetHeaderLength
    uint32_t headerLength;
    /// @brief moreFragments
    bool moreFragments;
    /// @brief Corresponding protocol example: 6:TCP
    uint32t protocol;
    /// @brief Source address
    ara::core::String srcAddress;
    /// @brief Source address Mask
    ara::core::String srcAddressMask;
    /// @brief ttl maximum value
    int32_t ttlMax;
    /// @brief ttl minimum value
    int32_t ttlMin;
};

/// @brief ipv6 rule parsing
class pIpv6Rule
{
public:
    /// @brief Destination address
    ara::core::String destMacAddress;
    /// @brief Destination address Mask
    ara::core::String destMacAddressMask;
    /// @brief flowLabel value
    uint32_t flowLabel;
    /// @brief hoplimit
    uint16_t doNotFragment;
    /// @brief ECN value
    uint16_t vlanPriority;
    /// @brief icmp checksum
    bool icmpChecksumVerification;
    /// @brief icmp code
    uint8_t icmpCode;
    /// @brief icmp type
    uint8_t icmpType;
    /// @brief next header
    uint16_t nextHeader;
    /// @brief Source address
    ara::core::String srcAddress;
    /// @brief Source address Mask
    ara::core::String srcAddressMask;
    /// @brief trafficClass value
    uint8_t trafficClass;
};

/// @brief tcp rule parsing
class PTcpRule
{
public:
    /// @brief tcp checksum
    bool tcpChecksumVerification;
    /// @brief dest max port
    uint16_t maxDestPort;
    /// @brief dest min port
    uint16_t minDestPort;
    /// @brief src max port
    uint16_t maxSrcPort;
    /// @brief src max port
    uint16_t minSrcPort;
    /// @brief tcp max session count
    uint16_t tcpMaxSession;
    /// @brief tcp state management flag.
    bool tcpStateManagementflag;
    /// @brief TCP timeout setting
    uint16_t tcpTimeoutCheck;
};

/// @brief udp rule parsing
class PUdpRule
{
public:
    /// @brief tcp checksum
    bool tcpChecksumVerification;
    /// @brief dest max port
    uint16_t maxDestPort;
    /// @brief dest min port
    uint16_t minDestPort;
    /// @brief src max port
    uint16_t maxSrcPort;
    /// @brief src max port
    uint16_t minSrcPort;
};

#endif

/// @brief Firewall rule set
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00190
/// @trace_id_dd=DD_FW_00287
/// @needwork = ad
/// @endcode
class PFirewallRule
{
public:
    /// @brief Ingress queue size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00288
    /// @needwork = dda
    /// @endcode
    int32_t bucketSize{-1};
    /// @brief Egress rate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00289
    /// @needwork = dda
    /// @endcode
    int32_t refillAmount{-1};
    /// @brief Filtering rules for each layer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00290
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecDataLinkRules;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00291
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecIpv4Rules;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00292
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecIpv6Rules;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00293
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecTcpRules;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00294
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecUdpRules;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00295
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecSomeIpRules;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00296
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecSomeIpSdRules;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00297
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > veDdsRules;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00298
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecDoIpRules;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00299
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PPayloadRulePart > vecPayloadRules;
};

#if 0
/// @brief Firewall ingress/egress firewall rule set
class PMatchingGressRule {
 public: 
 /// @brief Name of each layer rule set
    ara::core::Vector<PFirewallRule> vecFWRules;
};
#endif

/// @brief Firewall rule set
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00191
/// @trace_id_dd=DD_FW_00300
/// @needwork = ad
/// @endcode
class PFirewallRuleProps
{
public:
    /// @brief fw filter rule action: 0: block 1: allow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00301
    /// @needwork = dda
    /// @endcode
    int32_t action{-1};
    /// @brief Egress
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00302
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PFirewallRule > vecEgressRules;
    /// @brief Ingress
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00303
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PFirewallRule > vecIngressRules;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00192
    /// @trace_id_dd=DD_FW_00304
    /// @needwork = ad
    /// @endcode
    bool operator<(const PFirewallRuleProps &other) const { return action < other.action; }
};

/// @brief fw statesdepfirewall firewall state corresponding firewall processing
/// set.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00193
/// @trace_id_dd=DD_FW_00305
/// @needwork = ad
/// @endcode
class PStatesDepFirewall
{
public:
    /// @brief fw state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00306
    /// @needwork = dda
    /// @endcode
    uint32_t fwState{0};
    /// @brief fw default action 0: block 1: allow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00307
    /// @needwork = dda
    /// @endcode
    int32_t defaultAction{-1};
    /// @brief fw rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00308
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PFirewallRuleProps > vecRuleProps;
};

/// @brief fw structure corresponding to each client instance.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00194
/// @trace_id_dd=DD_FW_00309
/// @needwork = ad
/// @endcode
class PInstanceToFWRules
{
public:
    /// @brief instance instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00310
    /// @needwork = dda
    /// @endcode
    ara::core::String strInstName;
    /// @brief fw rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00311
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PStatesDepFirewall > vecStateDepFW;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00312
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PStatusAll > vecFWStateEnums;
};

//********************************/
}  // namespace internal
}  // namespace fw
}  // namespace ara

#endif  // ARA_FW_MANIFEST_DATA_H_
