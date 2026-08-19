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
/// @file       manifest_fw_syntax.h
/// @brief      fw json node name definition
/// @details    fw json node name definition
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
/// @unit_name=Manifest_Syntax
/// @unit_description=Configuration file node name definition
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_MANIFEST_FW_SYNTAX_H_
#define ARA_FW_MANIFEST_FW_SYNTAX_H_

#include "ara/fw/common/common.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief firewall manifest  full  directory.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00260
/// @trace_id_dd=DD_FW_00378
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkFirewallFileName() noexcept { return "/fw_manifest.json"; }

//********************************/
/// @brief Get configuration file tag string constant: fw state and rule
/// mapping.
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00261
/// @trace_id_dd=DD_FW_00379
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkRequireInstancesName() noexcept { return "adaptiveFirewallToPortPrototypeMapping"; }
//********************************/
/// @brief Get configuration file tag string constant: require_fw_instances
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00262
/// @trace_id_dd=DD_FW_00380
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkFWInstanceName() noexcept { return "instanceName"; }

// stateDepFirewall
/// @brief Get configuration file tag string constant: stateDepFirewall
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00263
/// @trace_id_dd=DD_FW_00381
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkStateDepFW() noexcept { return "stateDepFirewall"; }
/// @brief Get configuration file tag string constant: firewallState (fw state
/// inside statesdependentfirewall)
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00264
/// @trace_id_dd=DD_FW_00382
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkFWState() noexcept { return "firewallState"; }
/// @brief Get configuration file tag string constant: defaultAction
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00265
/// @trace_id_dd=DD_FW_00383
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDefaultAction() noexcept { return "defaultAction"; }

/// @brief Get configuration file tag string constant: netCardName
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00266
/// @trace_id_dd=DD_FW_00384
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkNetCardName() noexcept { return "netCardName"; }
// firewallRuleProps
/// @brief Get configuration file tag string constant: firewallRuleProps
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00267
/// @trace_id_dd=DD_FW_00385
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkFWRuleProps() noexcept { return "firewallRuleProps"; }
/// @brief Get configuration file tag string constant: firewallRuleProps
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00268
/// @trace_id_dd=DD_FW_00386
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAction() noexcept { return "action"; }
/// @brief Get configuration file tag string constant: fw_status_all
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00269
/// @trace_id_dd=DD_FW_00387
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkFWStatesAll() noexcept { return "firewallStatesAll"; }
/// @brief Get configuration file tag string constant: firewallStateVaule
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00270
/// @trace_id_dd=DD_FW_00388
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkFWEnumState() noexcept { return "StateEnumValue"; }
/// @brief Get configuration file tag string constant: fw_status_name
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00271
/// @trace_id_dd=DD_FW_00389
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkFWStateName() noexcept { return "fw_state_name"; }
/// @brief Get configuration file tag string constant: matchingEgressRule
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00272
/// @trace_id_dd=DD_FW_00390
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkFWEgressRule() noexcept { return "matchingEgressRule"; }
/// @brief Get configuration file tag string constant: matchingIngressRule
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00273
/// @trace_id_dd=DD_FW_00391
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkFWIngressRule() noexcept { return "matchingIngressRule"; }

/// @brief Get configuration file tag string constant: refillAmount
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00274
/// @trace_id_dd=DD_FW_00392
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkRefillAmount() noexcept { return "refillAmount"; }
/// @brief Get configuration file tag string constant: bucketSize
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00275
/// @trace_id_dd=DD_FW_00393
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkBucketSize() noexcept { return "bucketSize"; }

/// @brief Get configuration file tag string constant: dataLinkLayerRule
/// @return
/// @code{.isoft}
/// DataLinkLayerRule
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00276
/// @trace_id_dd=DD_FW_00394
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDataLinkLayerRule() noexcept { return "dataLinkLayerRule"; }
/// @brief Get configuration file tag string constant: destinationMacAddress
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00277
/// @trace_id_dd=DD_FW_00395
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDataLinkDestMacAddr() noexcept { return "destinationMacAddress"; }
/// @brief Get configuration file tag string constant: destinationMacAddress
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00278
/// @trace_id_dd=DD_FW_00396
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDataLinkDestMacAddrMask() noexcept { return "destinationMacAddressMask"; }
/// @brief Get configuration file tag string constant: etherTypes
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00279
/// @trace_id_dd=DD_FW_00397
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDataLinkEtherType() noexcept { return "etherType"; }
/// @brief Get configuration file tag string constant: vlanId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00280
/// @trace_id_dd=DD_FW_00398
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDataLinkVlanId() noexcept { return "vlanId"; }
/// @brief Get configuration file tag string constant: vlanPriority
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00281
/// @trace_id_dd=DD_FW_00399
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDataLinkVlanPriority() noexcept { return "vlanPriority"; }

/// @brief Get configuration file tag string constant: ipv4Rule
/// @return
/// @code{.isoft}
/// ipv4Rule
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00282
/// @trace_id_dd=DD_FW_00400
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv4Rule() noexcept { return "ipv4Rule"; }
/// @brief Get configuration file tag string constant: checksumVerification
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00283
/// @trace_id_dd=DD_FW_00401
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv4CheckSumVerify() noexcept { return "checksumVerification"; }
/// @brief Get configuration file tag string constant: GetkIpv4DestIpAddr
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00284
/// @trace_id_dd=DD_FW_00402
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv4DestIpAddr() noexcept { return "destinationIpAddress"; }
/// @brief Get configuration file tag string constant: destinationNetworkMask
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00285
/// @trace_id_dd=DD_FW_00403
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv4DestIpAddrMask() noexcept { return "destinationNetworkMask"; }
/// @brief Get configuration file tag string constant:
/// differentiatedServiceCodePoint
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00286
/// @trace_id_dd=DD_FW_00404
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv4Dscp() noexcept { return "ipvdifferentiatedServiceCodePoint4Rule"; }
/// @brief Get configuration file tag string constant: doNotFragment
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00287
/// @trace_id_dd=DD_FW_00405
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv4DoNotFragment() noexcept { return "doNotFragment"; }
/// @brief Get configuration file tag string constant:
/// explicitCongestionNotification
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00288
/// @trace_id_dd=DD_FW_00406
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv4Ecn() noexcept { return "explicitCongestionNotification"; }
/// @brief Get configuration file tag string constant: ipv4Rule
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00289
/// @trace_id_dd=DD_FW_00407
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv4HeaderLength() noexcept { return "internetHeaderLength"; }
/// @brief Get configuration file tag string constant: ipv4Rule
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00290
/// @trace_id_dd=DD_FW_00408
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv4MoreFragment() noexcept { return "moreFragments"; }
/// @brief Get configuration file tag string constant: protocol
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00291
/// @trace_id_dd=DD_FW_00409
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv4Protocol() noexcept { return "protocol"; }
/// @brief Get configuration file tag string constant: sourceIpAddress
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00292
/// @trace_id_dd=DD_FW_00410
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv4SrcIPAddr() noexcept { return "sourceIpAddress"; }
/// @brief Get configuration file tag string constant: sourceNetworkMask
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00293
/// @trace_id_dd=DD_FW_00411
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv4SrcIPAddrMask() noexcept { return "sourceNetworkMask"; }
/// @brief Get configuration file tag string constant: ttlMax
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00294
/// @trace_id_dd=DD_FW_00412
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv4ttlMax() noexcept { return "ttlMax"; }
/// @brief Get configuration file tag string constant: ttlMin
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00295
/// @trace_id_dd=DD_FW_00413
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv4ttlMin() noexcept { return "ttlMin"; }

// Ipv6Rule
/// @brief Get configuration file tag string constant: ipv6Rule
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00296
/// @trace_id_dd=DD_FW_00414
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv6Rule() noexcept { return "ipv6Rule"; }
/// @brief Get configuration file tag string constant: destinationIpAddress
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00297
/// @trace_id_dd=DD_FW_00415
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv6DestIpAddr() noexcept { return "destinationIpAddress"; }
/// @brief Get configuration file tag string constant: destinationNetworkMask
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00298
/// @trace_id_dd=DD_FW_00416
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv6DestIpAddrMask() noexcept { return "destinationNetworkMask"; }
/// @brief Get configuration file tag string constant: flowLabel
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00299
/// @trace_id_dd=DD_FW_00417
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv6FlowLabel() noexcept { return "flowLabel"; }
/// @brief Get configuration file tag string constant: hopLimit
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00300
/// @trace_id_dd=DD_FW_00418
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv6HopLimit() noexcept { return "hopLimit"; }
/// @brief Get configuration file tag string constant: doNotFragment
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00301
/// @trace_id_dd=DD_FW_00419
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv6DoNotFragment() noexcept { return "doNotFragment"; }
/// @brief Get configuration file tag string constant: nextHeader
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00302
/// @trace_id_dd=DD_FW_00420
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv6NextHeader() noexcept { return "nextHeader"; }
/// @brief Get configuration file tag string constant: sourceIpAddress
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00303
/// @trace_id_dd=DD_FW_00421
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv6SrcIPAddr() noexcept { return "sourceIpAddress"; }
/// @brief Get configuration file tag string constant: sourceNetworkMask
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00304
/// @trace_id_dd=DD_FW_00422
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv6SrcIPAddrMask() noexcept { return "sourceNetworkMask"; }
/// @brief Get configuration file tag string constant: trafficClass
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00305
/// @trace_id_dd=DD_FW_00423
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIpv6TrafficClass() noexcept { return "trafficClass"; }

// ICMP Rule
/// @brief Get configuration file tag string constant: icmpchecksum
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00306
/// @trace_id_dd=DD_FW_00424
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIcmpCheckSum() noexcept { return "icmpCheckSum"; }
/// @brief Get configuration file tag string constant: icmpcode
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00307
/// @trace_id_dd=DD_FW_00425
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIcmpCode() noexcept { return "icmpCode"; }
/// @brief Get configuration file tag string constant: icmptype
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00308
/// @trace_id_dd=DD_FW_00426
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkIcmpType() noexcept { return "icmpType"; }

// TransportRule configuration.
/// @brief Get configuration file tag string constant: icmpchecksum
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00309
/// @trace_id_dd=DD_FW_00427
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkCheckSum() noexcept { return "checksumVerification"; }
/// @brief Get configuration file tag string constant: maxDestinationPortNumber
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00310
/// @trace_id_dd=DD_FW_00428
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkMaxDestPortNum() noexcept { return "maxDestinationPortNumber"; }
/// @brief Get configuration file tag string constant: minDestinationPortNumber
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00311
/// @trace_id_dd=DD_FW_00429
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkMinDestPortNum() noexcept { return "minDestinationPortNumber"; }
/// @brief Get configuration file tag string constant: maxSrcinationPortNumber
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00312
/// @trace_id_dd=DD_FW_00430
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkMaxSrcPortNum() noexcept { return "maxSourcePortNumber"; }
/// @brief Get configuration file tag string constant: minSrcPortNumber
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00313
/// @trace_id_dd=DD_FW_00431
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkMinSrcPortNum() noexcept { return "minSourcePortNumber"; }

// TCP Rule
/// @brief Get configuration file tag string constant: tcpRule
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00314
/// @trace_id_dd=DD_FW_00432
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkTcpRule() noexcept { return "tcpRule"; }
/// @brief Get configuration file tag string constant:
/// numberOfParallelTcpSessions
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00315
/// @trace_id_dd=DD_FW_00433
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkTcpMaxSessionNum() noexcept { return "numberOfParallelTcpSessions"; }
/// @brief Get configuration file tag string constant:
/// stateManagementBasedOnTcpFlags
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00316
/// @trace_id_dd=DD_FW_00434
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkTcpStateManagementFlag() noexcept { return "stateManagementBasedOnTcpFlags"; }
/// @brief Get configuration file tag string constant: timeoutCheck
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00317
/// @trace_id_dd=DD_FW_00435
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkTcpTimeOutCheck() noexcept { return "timeoutCheck"; }

// UDP Rule
/// @brief Get configuration file tag string constant: udpRule
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00318
/// @trace_id_dd=DD_FW_00436
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkUdpRule() noexcept { return "udpRule"; }

// UDP Rule
/// @brief Rule name: RuleName
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00319
/// @trace_id_dd=DD_FW_00437
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkRuleName() noexcept { return "ruleName"; }

// UDP Rule
/// @brief Rule value: RuleData
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00320
/// @trace_id_dd=DD_FW_00438
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkRuleData() noexcept { return "ruleData"; }

/// @brief Get configuration file tag string constant: DdsRule
/// @return
/// @code{.isoft}
/// DDS Rule
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00321
/// @trace_id_dd=DD_FW_00439
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDdsRule() noexcept { return "ddsRule"; }

/// @brief Get configuration file tag string constant: appId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00322
/// @trace_id_dd=DD_FW_00440
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkAppId() noexcept { return "appId"; }

/// @brief Get configuration file tag string constant: hostId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00323
/// @trace_id_dd=DD_FW_00441
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkHostId() noexcept { return "hostId"; }

/// @brief Get configuration file tag string constant: instanceId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00324
/// @trace_id_dd=DD_FW_00442
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkInstanceId() noexcept { return "instanceId"; }

/// @brief Get configuration file tag string constant: majorProtocolVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00325
/// @trace_id_dd=DD_FW_00443
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkMajorProtocolVersion() noexcept { return "majorProtocolVersion"; }

/// @brief Get configuration file tag string constant: minorProtocolVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00326
/// @trace_id_dd=DD_FW_00444
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkMinorProtocolVersion() noexcept { return "minorProtocolVersion"; }

/// @brief Get configuration file tag string constant: protocolId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00327
/// @trace_id_dd=DD_FW_00445
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkProtocolId() noexcept { return "protocolId"; }

/// @brief Get configuration file tag string constant: readerEntityId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00328
/// @trace_id_dd=DD_FW_00446
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkReaderEntityId() noexcept { return "readerEntityId"; }

/// @brief Get configuration file tag string constant: submessageType
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00329
/// @trace_id_dd=DD_FW_00447
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkSubmessageType() noexcept { return "submessageType"; }

/// @brief Get configuration file tag string constant: vendorId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00330
/// @trace_id_dd=DD_FW_00448
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkVendorId() noexcept { return "vendorId"; }

/// @brief Get configuration file tag string constant: writerEntityId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00331
/// @trace_id_dd=DD_FW_00449
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkWriterEntityId() noexcept { return "writerEntityId"; }

/// @brief Get configuration file tag string constant: someIpRule
/// @return
/// @code{.isoft}
/// SOME/IP Rule
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00332
/// @trace_id_dd=DD_FW_00450
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkSomeIpRule() noexcept { return "someipRule"; }

/// @brief Get configuration file tag string constant: clientId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00333
/// @trace_id_dd=DD_FW_00451
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkClientId() noexcept { return "clientId"; }

/// @brief Get configuration file tag string constant: lengthVerification
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00334
/// @trace_id_dd=DD_FW_00452
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkLengthVerification() noexcept { return "lengthVerification"; }

/// @brief Get configuration file tag string constant: majorVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00335
/// @trace_id_dd=DD_FW_00453
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkMajorVersion() noexcept { return "majorVersion"; }

/// @brief Get configuration file tag string constant: messageType
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00336
/// @trace_id_dd=DD_FW_00454
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkMessageType() noexcept { return "messageType"; }

/// @brief Get configuration file tag string constant: methodId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00337
/// @trace_id_dd=DD_FW_00455
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkMethodId() noexcept { return "methodId"; }

/// @brief Get configuration file tag string constant: protocolVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00338
/// @trace_id_dd=DD_FW_00456
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkProtocolVersion() noexcept { return "protocolVersion"; }

/// @brief Get configuration file tag string constant: returnCode
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00339
/// @trace_id_dd=DD_FW_00457
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkReturnCode() noexcept { return "returnCode"; }

/// @brief Get configuration file tag string constant: serviceInterfaceId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00340
/// @trace_id_dd=DD_FW_00458
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkServiceInterfaceId() noexcept { return "serviceInterfaceId"; }

/// SOME/IP SD Rule

/// @brief Get configuration file tag string constant: someIpSdRule
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00341
/// @trace_id_dd=DD_FW_00459
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkSomeIpSdRule() noexcept { return "someipSdRule"; }

/// @brief Get configuration file tag string constant: entryType
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00342
/// @trace_id_dd=DD_FW_00460
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkEntryType() noexcept { return "entryType"; }

/// @brief Get configuration file tag string constant: eventGroupId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00343
/// @trace_id_dd=DD_FW_00461
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkEventGroupId() noexcept { return "eventGroupId"; }

/// @brief Get configuration file tag string constant: maxMajorVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00344
/// @trace_id_dd=DD_FW_00462
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkMaxMajorVersion() noexcept { return "maxMajorVersion"; }

/// @brief Get configuration file tag string constant: maxMinjorVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00345
/// @trace_id_dd=DD_FW_00463
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkMaxMinjorVersion() noexcept { return "maxMinjorVersion"; }

/// @brief Get configuration file tag string constant: minMajorVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00346
/// @trace_id_dd=DD_FW_00464
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkMinMajorVersion() noexcept { return "minMajorVersion"; }

/// @brief Get configuration file tag string constant: minMinjorVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00347
/// @trace_id_dd=DD_FW_00465
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkMinMinjorVersion() noexcept { return "minMinjorVersion"; }

/// @brief Get configuration file tag string constant: serviceInstanceId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00348
/// @trace_id_dd=DD_FW_00466
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkServiceInstanceId() noexcept { return "serviceInstanceId"; }

/// @brief Get configuration file tag string constant: serviceInterfaceId
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00349
/// @trace_id_dd=DD_FW_00467
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkSdServiceInterfaceId() noexcept { return "serviceInterfaceId"; }

/// DoIp Rule

/// @brief Get configuration file tag string constant: doipRule
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00350
/// @trace_id_dd=DD_FW_00468
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDoIpRule() noexcept { return "doipRule"; }

/// @brief Get configuration file tag string constant: destinationMaxAddress
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00351
/// @trace_id_dd=DD_FW_00469
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDestMaxAddress() noexcept { return "destinationMaxAddress"; }

/// @brief Get configuration file tag string constant: destinationMinAddress
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00352
/// @trace_id_dd=DD_FW_00470
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkDestMinAddress() noexcept { return "destinationMinAddress"; }

/// @brief Get configuration file tag string constant: inverseProtocolVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00353
/// @trace_id_dd=DD_FW_00471
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkInverseProtocolVersion() noexcept { return "inverseProtocolVersion"; }

/// @brief Get configuration file tag string constant: payloadLength
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00354
/// @trace_id_dd=DD_FW_00472
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkPayloadLength() noexcept { return "payloadLength"; }

/// @brief Get configuration file tag string constant: payloadType
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00355
/// @trace_id_dd=DD_FW_00473
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkPayloadType() noexcept { return "payloadType"; }

/// @brief Get configuration file tag string constant: protocolVersion
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00356
/// @trace_id_dd=DD_FW_00474
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkProVersion() noexcept { return "protocolVersion"; }

/// @brief Get configuration file tag string constant: sourceMaxAddress
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00357
/// @trace_id_dd=DD_FW_00475
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkSourceMaxAddress() noexcept { return "sourceMaxAddress"; }

/// @brief Get configuration file tag string constant: sourceMinAddress
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00358
/// @trace_id_dd=DD_FW_00476
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkSourceMinAddress() noexcept { return "sourceMinAddress"; }

/// @brief Get configuration file tag string constant: udsService
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00359
/// @trace_id_dd=DD_FW_00477
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkUdsService() noexcept { return "udsService"; }

/// @brief Get configuration file tag string constant: PayloadRule
/// @return
/// @code{.isoft}
/// PayloadBytePatternRule
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00360
/// @trace_id_dd=DD_FW_00478
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkPayloadRule() noexcept { return "payloadRule"; }
/// @brief Get configuration file tag string constant: offset
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00361
/// @trace_id_dd=DD_FW_00479
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkOffset() noexcept { return "offset"; }

/// @brief Get configuration file tag string constant: value
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00362
/// @trace_id_dd=DD_FW_00480
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkValue() noexcept { return "value"; }

}  // namespace internal
}  // namespace fw
}  // namespace ara

#endif  //
