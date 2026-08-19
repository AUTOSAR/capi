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
/// @file       cmd_str.h
/// @brief      nftables nft commands
/// @details    nftables nft commands
/// @date       2025-05-30
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/nft command set used by firewall engine abstraction layer
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0010
/// @unit_name=Firewall_Cmdstr
/// @unit_description=nft command set used by firewall engine abstraction layer
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_CMD_STR_H
#define ARA_FW_CMD_STR_H

#include <ara/core/string.h>

#include "ara/fw/common/common.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief Add nft table command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00196
/// @trace_id_dd=DD_FW_00314
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddTableForNftCmd() noexcept { return "sudo nft add table inet apfilter "; }

/// @brief Add nft table command. (netdev)
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00197
/// @trace_id_dd=DD_FW_00315
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddTableForNetDevCmd() noexcept
{
    return "sudo nft add table netdev ap_netdev_filter ";
}

/// @brief Add nft rule command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00198
/// @trace_id_dd=DD_FW_00316
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForNftCmdTable() noexcept { return "sudo nft add rule inet apfilter "; }

/// @brief Add nft rule command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00199
/// @trace_id_dd=DD_FW_00317
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForNetDevTable() noexcept
{
    return "sudo nft add rule netdev ap_netdev_filter";
}

/// @brief Add nft chain command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00200
/// @trace_id_dd=DD_FW_00318
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddChainForNftCmdTable() noexcept { return "sudo nft add chain inet apfilter "; }

/// @brief Add nft chain command. (netdev)
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00201
/// @trace_id_dd=DD_FW_00319
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddChainForNetDevCmdTable() noexcept
{
    return "sudo nft add chain netdev ap_netdev_filter ";
}

/// @brief Add nft delete table command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00202
/// @trace_id_dd=DD_FW_00320
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDelTableForNftCmd() noexcept { return "sudo nft delete  table  inet  apfilter "; }

/// @brief Add nft delete table command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00203
/// @trace_id_dd=DD_FW_00321
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDelTableForNetDevCmd() noexcept
{
    return "sudo nft delete  table  netdev  ap_netdev_filter ";
}

/// @brief Add nft delete chain command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00204
/// @trace_id_dd=DD_FW_00322
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDelChainForNftCmd() noexcept { return "sudo nft delete chain  inet  filter "; }

/// @brief Add nft delete chain command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00205
/// @trace_id_dd=DD_FW_00323
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDelChainForNetDevCmd() noexcept
{
    return "sudo nft delete chain  netdev  ap_netdev_filter ";
}

/// @brief Add nft delete rules inside chain command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00206
/// @trace_id_dd=DD_FW_00324
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDelChainRulesForNftCmd() noexcept { return "sudo nft flush chain  inet apfilter "; }

/// @brief Add nft delete rules inside chain command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00207
/// @trace_id_dd=DD_FW_00325
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDelChainRulesForNetDevCmd() noexcept
{
    return "sudo nft flush chain  netdev ap_netdev_filter ";
}

/// @brief Add nft chain hook:input action:drop command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00208
/// @trace_id_dd=DD_FW_00326
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddInputChainForDrop() noexcept
{
    return " \\{ type filter hook input priority 0 \\;policy drop \\;\\} ";
}

/// @brief Add nft chain hook:input action:drop command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00209
/// @trace_id_dd=DD_FW_00327
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddOutputChainForDrop() noexcept
{
    return " \\{ type filter hook output priority 0 \\;policy drop \\;\\} ";
}

/// @brief Add nft chain hook:ingress
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00210
/// @trace_id_dd=DD_FW_00328
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddInputNetDdvChainForInitFirst() noexcept
{
    return " \\{ type filter hook ingress device ";
}
/// @brief Add nft chain hook:gress
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00211
/// @trace_id_dd=DD_FW_00329
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddOutputNetDdvChainForInitFirst() noexcept
{
    return " \\{ type filter hook egress device ";
}

/// @brief Add nft chain hook:ingress
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00212
/// @trace_id_dd=DD_FW_00330
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddInputNetDdvChainForInitSecondAccept() noexcept
{
    return " priority 0\\; policy accept\\;\\} ";
}

/// @brief Add nft chain hook:ingress
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00213
/// @trace_id_dd=DD_FW_00331
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddInputNetDdvChainForInitSecondDrop() noexcept
{
    return " priority 0\\; policy drop\\;\\} ";
}

/// @brief Add nft chain output command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00214
/// @trace_id_dd=DD_FW_00332
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddInputChainForAccept() noexcept
{
    return " \\{ type filter hook input priority 0 \\;policy accept \\;\\}";
}

/// @brief Add nft chain output command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00215
/// @trace_id_dd=DD_FW_00333
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddOutputChainForAccept() noexcept
{
    return " \\{ type filter hook output priority 0 \\;policy accept \\;\\}";
}

//********************************/
/// @brief Get nft counter command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00216
/// @trace_id_dd=DD_FW_00334
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForCounter() noexcept { return " counter "; }

/*****************************************datalink*********************************************/

//********************************/
/// @brief Get nft ether type command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00217
/// @trace_id_dd=DD_FW_00335
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForDataLinkSaddrMac() noexcept { return " ether saddr "; }

//********************************/
/// @brief Get nft ether type command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00218
/// @trace_id_dd=DD_FW_00336
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForDataLinkDaddrMac() noexcept { return " ether daddr "; }

//********************************/
/// @brief Get nft datalink ether type command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00219
/// @trace_id_dd=DD_FW_00337
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForDataLinkEtherType() noexcept { return " ether type  "; }

//********************************/
/// @brief Get nft icmp type command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00220
/// @trace_id_dd=DD_FW_00338
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForIcmpType() noexcept { return " icmp type "; }

//********************************/
/// @brief Get nft icmp code command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00221
/// @trace_id_dd=DD_FW_00339
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForIcmpCode() noexcept { return " icmp code "; }

//********************************/
/// @brief Get nft ipv4 saddr command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00222
/// @trace_id_dd=DD_FW_00340
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForSrcAddr() noexcept { return "  ip saddr "; }

//********************************/
/// @brief Get nft ipv4 daddr command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00223
/// @trace_id_dd=DD_FW_00341
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForDestAddr() noexcept { return "  ip daddr "; }

//********************************/
/// @brief Get nft ipv4 dscp command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00224
/// @trace_id_dd=DD_FW_00342
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForDscp() noexcept { return " ip dscp  "; }

//********************************/
/// @brief Get nft ipv4 ecn command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00225
/// @trace_id_dd=DD_FW_00343
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForEcn() noexcept { return " ip ecn "; }

//********************************/
/// @brief Get nft ipv4 DF fragment FLAG command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00226
/// @trace_id_dd=DD_FW_00344
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForDF() noexcept { return " ip  frag-off  \\& 0x4000 != 0 "; }

//********************************/
/// @brief Get nft ipv4 MF fragment FLAG command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00227
/// @trace_id_dd=DD_FW_00345
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForMF() noexcept { return " ip  frag-off  \\& 0x2000 != 0 "; }

//********************************/
/// @brief Get nft ipv4 verify minimum HdrLength command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00228
/// @trace_id_dd=DD_FW_00346
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForHdrLength() noexcept { return " ip hdrlength  \\>= "; }

//********************************/
/// @brief Get nft ipv4 verify protocol type command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00229
/// @trace_id_dd=DD_FW_00347
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForIpProtocol() noexcept { return " ip protocol "; }

//********************************/
/// @brief Get nft ipv4 saddr command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00230
/// @trace_id_dd=DD_FW_00348
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForIp6SrcAddr() noexcept { return "  ip6 saddr "; }

//********************************/
/// @brief Get nft ipv4 daddr command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00231
/// @trace_id_dd=DD_FW_00349
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForIp6DestAddr() noexcept { return "  ip6 daddr "; }

//********************************/
/// @brief Get nft icmp type command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00232
/// @trace_id_dd=DD_FW_00350
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForIcmpIP6Type() noexcept { return " icmpv6 type "; }

//********************************/
/// @brief Get nft icmp code command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00233
/// @trace_id_dd=DD_FW_00351
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForIcmpIp6Code() noexcept { return " icmpv6 code "; }

//********************************/
/// @brief Get nft ipv6 flowlabel command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00234
/// @trace_id_dd=DD_FW_00352
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForFlowLabel() noexcept { return " ip6 flowlabel  "; }

//********************************/
/// @brief Get nft ipv6 hoplimit command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00235
/// @trace_id_dd=DD_FW_00353
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForHopLimit() noexcept { return " ip6 hoplimit  "; }

//********************************/
/// @brief Get nft ipv6 nexthdr command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00236
/// @trace_id_dd=DD_FW_00354
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForNexthdr() noexcept { return " ip6  nexthdr "; }

//********************************/
/// @brief Get nft ipv6 dscp command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00237
/// @trace_id_dd=DD_FW_00355
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForIp6Dscp() noexcept { return " ip6 dscp "; }

//********************************/
/// @brief Get nft ipv6 ecn command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00238
/// @trace_id_dd=DD_FW_00356
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAddRuleForIp6Ecn() noexcept { return " ip6 ecn "; }

//********************************/
/// @brief Get nft udp sport command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00239
/// @trace_id_dd=DD_FW_00357
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkUdpForSport() noexcept { return " udp sport "; }

//********************************/
/// @brief Get nft udp dport command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00240
/// @trace_id_dd=DD_FW_00358
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkUdpForDport() noexcept { return " udp dport "; }

//********************************/
/// @brief Get nft udp command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00241
/// @trace_id_dd=DD_FW_00359
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkUdpForSPortMax() noexcept { return " udp sport \\<= "; }

//********************************/
/// @brief Get nft udp command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00242
/// @trace_id_dd=DD_FW_00360
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkUdpForSPortMin() noexcept { return " udp sport  \\>= "; }

//********************************/
/// @brief Get nft udp command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00243
/// @trace_id_dd=DD_FW_00361
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkUdpForDportMax() noexcept { return " udp dport \\<= "; }

//********************************/
/// @brief Get nft udp command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00244
/// @trace_id_dd=DD_FW_00362
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkUdpForDportMin() noexcept { return " udp dport  \\>= "; }

//********************************/
/// @brief Get nft tcp sport command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00245
/// @trace_id_dd=DD_FW_00363
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkTcpForSport() noexcept { return " tcp sport "; }

//********************************/
/// @brief Get nft tcp dport command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00246
/// @trace_id_dd=DD_FW_00364
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkTcpForDport() noexcept { return " tcp dport "; }

//********************************/
/// @brief Get nft tcp command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00247
/// @trace_id_dd=DD_FW_00365
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkTcpForSPortMax() noexcept { return " tcp sport \\<= "; }

//********************************/
/// @brief Get nft tcp command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00248
/// @trace_id_dd=DD_FW_00366
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkTcpForSPortMin() noexcept { return " tcp sport  \\>= "; }

//********************************/
/// @brief Get nft tcp command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00249
/// @trace_id_dd=DD_FW_00367
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkTcpForDportMax() noexcept { return " tcp dport \\<= "; }

//********************************/
/// @brief Get nft tcp command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00250
/// @trace_id_dd=DD_FW_00368
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkTcpForDportMin() noexcept { return " tcp dport  \\>= "; }

//********************************/
/// @brief Get nft tcp command.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00251
/// @trace_id_dd=DD_FW_00369
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkTcpForMaxTcpSession() noexcept { return " tcp flags syn ct count over  "; }

//********************************/
/// @brief nft allow TCP connections and related connection handling
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00252
/// @trace_id_dd=DD_FW_00370
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkTcpForSetTcpState() noexcept { return " ct state established,related  accept "; }

//********************************/
/// @brief nft new connections only allow SYN packet sending (only exists at
/// input hook point)
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00253
/// @trace_id_dd=DD_FW_00371
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkTcpForSetTcpNewState() noexcept { return "  tcp flags syn ct state new  accept "; }

//********************************/
/// @brief nft drop invalid TCP packets
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00254
/// @trace_id_dd=DD_FW_00372
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkTcpForDropTcpInvaildState() noexcept { return "  ct state invaild  drop "; }

//********************************/
/// @brief nft set rate
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00255
/// @trace_id_dd=DD_FW_00373
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkLimitRate() noexcept { return " limit rate "; }

//********************************/
/// @brief nft set rate (per second)
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00256
/// @trace_id_dd=DD_FW_00374
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkLimitRateSecond() noexcept { return "\\/second "; }

//********************************/
/// @brief nft set app layer sys command input
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00257
/// @trace_id_dd=DD_FW_00375
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAppEchoCmd() noexcept { return "sudo  echo "; }

//********************************/
/// @brief nft set app layer sys command input
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00258
/// @trace_id_dd=DD_FW_00376
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAppSysDirCmd() noexcept
{
    return " | sudo tee /sys/module/kfirewall_app/parameters/";
}

/// @brief Get kernlog log path
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00259
/// @trace_id_dd=DD_FW_00377
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkKernLog() noexcept { return "/var/log/kern.log"; }
}  // namespace internal
}  // namespace fw
}  // namespace ara

#endif  //
