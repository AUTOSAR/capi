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
/// @file       engine_common.h
/// @brief      fw engine common processing part
/// @details    fw engine common processing part
/// @date       2025-05-06
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Firewall Engine Abstraction Layer
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0010
/// @unit_name=Engine_Common
/// @unit_description=Firewall engine abstraction layer common part
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef _ARA_FW_FILTER_COMMON_H_
#define _ARA_FW_FILTER_COMMON_H_
#include <ara/core/string.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "ara/fw/internal/log_api.h"

namespace ara {
namespace fw {
namespace internal {

// IPMASK = 255.255.255.255 /32 subnet
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00039
/// @needwork = dd
/// @endcode
uint32_t constexpr kIpMask32Value = 0xFFFFFFFFUL;
// IPMASK = 255.255.255.254 /31 subnet
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00040
/// @needwork = dd
/// @endcode
uint32_t constexpr kIpMask31Value = 0xFFFFFFFEUL;

// ipv6 dscp  right  shift  2
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00041
/// @needwork = dd
/// @endcode
uint8_t constexpr kIp6DscpShift = 2;

// ipv6 ecn left  shift  6
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00042
/// @needwork = dd
/// @endcode
uint8_t constexpr kIp6EcnShift = 6;

// ipv6 Mask 128
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00043
/// @needwork = dd
/// @endcode
int32_t constexpr kIp6MaskMaxBit = 128;

// ipv6 Mask 0
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00044
/// @needwork = dd
/// @endcode
int32_t constexpr kIp6MaskMinBit = 0;

// ipv6 address 8 segments.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00045
/// @needwork = dd
/// @endcode
int32_t constexpr kIp6Segment = 8;

// ipv6 address conversion digit definition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00046
/// @needwork = dd
/// @endcode
int32_t constexpr kIp6Value16 = 16;

// ipv6 address conversion digit definition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00047
/// @needwork = dd
/// @endcode
int32_t constexpr kIp6Value32 = 32;

// ipv6 address conversion digit definition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00048
/// @needwork = dd
/// @endcode
int32_t constexpr kIp6Value48 = 48;

// ipv6 segment[5]
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00049
/// @needwork = dd
/// @endcode
int32_t constexpr kIp6Segment5 = 5;

// ipv6  segment[6]
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00050
/// @needwork = dd
/// @endcode
int32_t constexpr kIp6Segment6 = 6;

// ipv6 segment[7]
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00051
/// @needwork = dd
/// @endcode
int32_t constexpr kIp6Segment7 = 7;

// ipv6 segment
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00052
/// @needwork = dd
/// @endcode
uint16_t constexpr kIp6Uint16Max = 0xFFFF;

//
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00053
/// @needwork = dd
/// @endcode
int32_t constexpr kDataLinkMacLength = 12;

// MAC has 6 segments in total.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00054
/// @needwork = dd
/// @endcode
int32_t constexpr kDataLinkMacSegment = 6;

// Rate configuration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00055
/// @needwork = dd
/// @endcode
int32_t constexpr kRateOnly = 1;

// User did not configure rate && queue length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00056
/// @needwork = dd
/// @endcode
int32_t constexpr kNoRateAndQueueLen = 0;

// User configured rate && queue length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00391
/// @trace_id_dd=DD_FW_00057
/// @needwork = dd
/// @endcode
int32_t constexpr kRateAndQueueLen = 2;

/// @brief system call function
/// @param strCmd nft command
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00028
/// @trace_id_dd=DD_FW_00058
/// @needwork = ad
/// @endcode
bool CallSystem(ara::core::String const &strCmd) noexcept;

/// @brief Check whether the string contains only hexadecimal characters
/// @param str Hexadecimal string
/// @return yes/not
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00029
/// @trace_id_dd=DD_FW_00059
/// @needwork = ad
/// @endcode
bool Is_Hex_String(const std::string &str) noexcept;

/// @brief Get the corresponding IP range of the firewall through IP and subnet
/// mask. (ipv4)
/// @param ip
/// @param ipMask
/// @return Get the corresponding ip range.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00030
/// @trace_id_dd=DD_FW_00060
/// @needwork = ad
/// @endcode
ara::core::String GetIpRange(ara::core::String const &ip, ara::core::String const &ipMask) noexcept;

/// @brief Verify whether the Mask is valid
/// @param mask
/// @return true/false.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00031
/// @trace_id_dd=DD_FW_00061
/// @needwork = ad
/// @endcode
bool Is_Valid_Mask(uint32_t mask) noexcept;

/// @brief Get GetActionBlock
/// @return drop
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00032
/// @trace_id_dd=DD_FW_00062
/// @needwork = ad
/// @endcode
ara::core::String GetActionBlock() noexcept;

/// @brief Get ActionAllow
/// @return allow
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00033
/// @trace_id_dd=DD_FW_00063
/// @needwork = ad
/// @endcode
ara::core::String GetActionAllow() noexcept;

/// @brief Get input chain chain
/// @return apinputChain
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00034
/// @trace_id_dd=DD_FW_00064
/// @needwork = ad
/// @endcode
ara::core::String GetInputChain() noexcept;

/// @brief Get output chain
/// @return apoutputChain
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00035
/// @trace_id_dd=DD_FW_00065
/// @needwork = ad
/// @endcode
ara::core::String GetOutPutChain() noexcept;

/// @brief Get netdev InputChain name ingress
/// @return apinputchain
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00036
/// @trace_id_dd=DD_FW_00066
/// @needwork = ad
/// @endcode
ara::core::String GetNetDevInputChain() noexcept;

/// @brief Get netdev OutputChain name ingress
/// @return outputchain
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00037
/// @trace_id_dd=DD_FW_00067
/// @needwork = ad
/// @endcode
ara::core::String GetNetDevOutputChain() noexcept;

/// @brief Set packet flow rate (both ingress and egress can be set)
/// @param nftCmd First half of nft command
/// @param SetPacketRate User flow rate setting
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00038
/// @trace_id_dd=DD_FW_00068
/// @needwork = ad
/// @endcode
void SetPacketRate(ara::core::String &nftCmd, int32_t packetRate) noexcept;

/// @brief Set packet flow rate limit
/// @param nftFlowLenCmd nft command
/// @param packetRate User flow rate setting
/// @param queueLength Firewall queue length setting
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00039
/// @trace_id_dd=DD_FW_00069
/// @needwork = ad
/// @endcode
void SetPacketFlowLength(ara::core::String &nftFlowLenCmd, int32_t packetRate, int32_t queueLength) noexcept;

/// @brief Set log and action
/// @param cmdFirHalf First half of nft command
/// @param action Action
/// @param logPrefix nft log identifier
/// @return Add log command string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00040
/// @trace_id_dd=DD_FW_00070
/// @needwork = ad
/// @endcode
ara::core::String SetFirewallLogAndAction(ara::core::String &cmdFirHalf,
                                          int32_t action,
                                          ara::core::String const &logPrefix = "") noexcept;

/// @brief Check whether the user has set the rate
/// @param rate
/// @return true: rate field set false: not set
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00041
/// @trace_id_dd=DD_FW_00071
/// @needwork = ad
/// @endcode
bool CheckRate(int32_t rate) noexcept;

/// @brief Check whether the user has set the rate
/// @param rate
/// @return true: rate field set false: not set
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00042
/// @trace_id_dd=DD_FW_00072
/// @needwork = ad
/// @endcode
int32_t CheckRateAndQueueLen(int32_t rate, int32_t queueLen) noexcept;

/// @brief Set rate and log command string
/// @param cmd First half of nft command
/// @param rate Rate
/// @param queueLen Queue length
/// @param action Action
/// @param logPrefix nft log identifier
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00043
/// @trace_id_dd=DD_FW_00073
/// @needwork = ad
/// @endcode
void SetLogAndRata(ara::core::String &cmd,
                   int32_t rate,
                   int32_t queueLen,
                   int32_t action,
                   ara::core::String const &logPrefix) noexcept;

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif