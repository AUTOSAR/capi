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
/// @file       idsm_manifest.h
/// @brief      Configuration accessor
/// @details
/// @date       2023-01-13
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/config center
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0011
/// @unit_description=Configuration accessor
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_MANIFEST_H_
#define ARA_IDSM_MANIFEST_H_
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <chrono>
#include <cstdint>
#include <memory>
#include <utility>

namespace ara {
namespace idsm {

/// @brief Get IDSR network address and port number
/// @code{.isoft}
/// @unit_name=NetworkInterface
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00197
/// @trace_id_dd=DD_IDSM_00469
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0011
/// @endcode
struct NetworkInterface
{
public:
    /// @name ipv4IpAddress
    /// @brief IDSR service IP address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00470
    /// @needwork = dda
    /// @endcode
    ara::core::String ipv4IpAddress;
    /// @name tcpPort
    /// @brief IDSR service port number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00471
    /// @needwork = dda
    /// @endcode
    uint16_t tcpPort;
    /// @brief Get IDSR network address and port number
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00472
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Get IDSR network information && IDSM time domain
/// @code{.isoft}
/// @unit_name=IdsmModuleInstantiation
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00198
/// @trace_id_dd=DD_IDSM_00473
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0003,SR_IDSM_0011
/// @endcode
struct IdsmModuleInstantiation
{
public:
    /// @name net
    /// @brief Network data structure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00474
    /// @needwork = dda
    /// @endcode
    NetworkInterface net;
    /// @name timeBase
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00475
    /// @needwork = dda
    /// @endcode
    ara::core::String timeBase;
    /// @brief Get IDSR network information && IDSM time domain
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00476
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Rate limiting filter
/// @code{.isoft}
/// @unit_name=RateLimitationFilter
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00199
/// @trace_id_dd=DD_IDSM_00477
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0013
/// @endcode
struct RateLimitationFilter
{
public:
    /// @name maxEventsInInterval
    /// @brief Upper limit on the number of security events sent within the rate limiting period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00478
    /// @needwork = dda
    /// @endcode
    uint32_t maxEventsInInterval;
    /// @name timeInterval
    /// @brief Rate limiting period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00479
    /// @needwork = dda
    /// @endcode
    float timeInterval;
    /// @brief Get IDSM rate limiting information
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00480
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Traffic shaping filter
/// @code{.isoft}
/// @unit_name=TrafficLimitationFilter
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00200
/// @trace_id_dd=DD_IDSM_00481
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0013
/// @endcode
struct TrafficLimitationFilter
{
public:
    /// @name maxBytesInInterva
    /// @brief Upper limit on the number of bytes sent within the traffic shaping period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00482
    /// @needwork = dda
    /// @endcode
    uint32_t maxBytesInInterva;
    /// @name timeInterval
    /// @brief Traffic shaping period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00483
    /// @needwork = dda
    /// @endcode
    float timeInterval;
    /// @brief Get IDSM traffic shaping information
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00484
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Memory limit
/// @code{.isoft}
/// @unit_name=MemoryLimitation
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00201
/// @trace_id_dd=DD_IDSM_00485
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0002
/// @endcode
struct MemoryLimitation
{
public:
    /// @name eventMem
    /// @brief Upper limit on memory used for processing security events (excluding event context)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00486
    /// @needwork = dda
    /// @endcode
    uint32_t eventMem;
    /// @name contextMem
    /// @brief Upper limit on memory used for context of security events being processed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00487
    /// @needwork = dda
    /// @endcode
    uint32_t contextMem;
    /// @brief Get memory limit
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00488
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Data signature
/// @code{.isoft}
/// @unit_name=SignatureSupportAp
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00202
/// @trace_id_dd=DD_IDSM_00489
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0011
/// @endcode
struct SignatureSupportAp
{
public:
    /// @brief Algorithm used in signature
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00490
    /// @needwork = dda
    /// @endcode
    ara::core::String cryptoPrimitive;
    /// @brief Key slot used in signature
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00491
    /// @needwork = dda
    /// @endcode
    ara::core::String keySlot;
    /// @brief Get signature related data
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00492
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};

/// @brief IDSM instance
/// @code{.isoft}
/// @unit_name=IdsmInstance
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00203
/// @trace_id_dd=DD_IDSM_00493
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0011
/// @endcode
struct IdsmInstance
{
public:
    /// @name shortName
    /// @brief Name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00494
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName;
    /// @name fqn
    /// @brief Identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00495
    /// @needwork = dda
    /// @endcode
    ara::core::String fqn;
    /// @name idsmInstanceId
    /// @brief Instance ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00496
    /// @needwork = dda
    /// @endcode
    uint32_t idsmInstanceId;
    /// @name signatureSupportCp
    /// @brief Cp related
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00497
    /// @needwork = dda
    /// @endcode
    ara::core::String signatureSupportCp;
    /// @name timestampFormat
    /// @brief Timestamp format
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00498
    /// @needwork = dda
    /// @endcode
    ara::core::String timestampFormat;
    /// @name moduleInstall
    /// @brief Module installation
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00499
    /// @needwork = dda
    /// @endcode
    IdsmModuleInstantiation moduleInstall;
    /// @name rateLimitationFilter
    /// @brief Traffic shaping filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00500
    /// @needwork = dda
    /// @endcode
    RateLimitationFilter rateLimitationFilter;
    /// @name trafficLimitationFilter
    /// @brief Rate limiting filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00501
    /// @needwork = dda
    /// @endcode
    TrafficLimitationFilter trafficLimitationFilter;
    /// @name memLimitation
    /// @brief Memory threshold
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00502
    /// @needwork = dda
    /// @endcode
    MemoryLimitation memLimitation;
    /// @brief Signature
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00503
    /// @needwork = dda
    /// @endcode
    SignatureSupportAp signAp;
    /// @brief Get IDSM instance information
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00504
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Aggregation filter
/// @code{.isoft}
/// @unit_name=Aggregation
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00204
/// @trace_id_dd=DD_IDSM_00505
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0009
/// @endcode
struct Aggregation
{
public:
    /// @name contextDataSource
    /// @brief Source of context data for aggregated events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00506
    /// @needwork = dda
    /// @endcode
    ara::core::String contextDataSource{};
    /// @name minimumIntervalLength
    /// @brief Aggregation period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00507
    /// @needwork = dda
    /// @endcode
    float minimumIntervalLength{0};
    /// @brief Get aggregation filter
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00508
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Threshold filter
/// @code{.isoft}
/// @unit_name=Threshold
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00205
/// @trace_id_dd=DD_IDSM_00509
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0010
/// @endcode
struct Threshold
{
public:
    /// @name intervalLength
    /// @brief  Threshold period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00510
    /// @needwork = dda
    /// @endcode
    float intervalLength;
    /// @name thresholdNumber
    /// @brief Threshold
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00511
    /// @needwork = dda
    /// @endcode
    uint32_t thresholdNumber;
    /// @brief Get threshold filter
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00512
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Function group status
/// @code{.isoft}
/// @unit_name=FunctionGroupState
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00206
/// @trace_id_dd=DD_IDSM_00513
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0007
/// @endcode
struct FunctionGroupState
{
public:
    /// @name modeDeclarationGroup
    /// @brief Function group name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00514
    /// @needwork = dda
    /// @endcode
    ara::core::String modeDeclarationGroup;
    /// @name modeDeclaration
    /// @brief Function group status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00515
    /// @needwork = dda
    /// @endcode
    ara::core::String modeDeclaration;
    /// @brief Get function group and function group status
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00516
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Status filter
/// @code{.isoft}
/// @unit_name=FGState
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00207
/// @trace_id_dd=DD_IDSM_00517
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0007
/// @endcode
struct FGState
{
public:
    /// @name blockIfStateActiveAp
    /// @brief Function group status blacklist
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00518
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< FunctionGroupState > blockIfStateActiveAp;
    /// @brief Get status filter
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00519
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Filter chain
/// @code{.isoft}
/// @unit_name=SecurityEventFilterChain
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00208
/// @trace_id_dd=DD_IDSM_00520
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0006
/// @endcode
struct SecurityEventFilterChain
{
public:
    /// @brief fqn identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00521
    /// @needwork = dda
    /// @endcode
    ara::core::String fqn{};
    /// @brief Whether there is a sampling filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00522
    /// @needwork = dda
    /// @endcode
    bool hasOneEveryN{false};
    /// @brief Sampling filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00523
    /// @needwork = dda
    /// @endcode
    uint32_t oneEveryN{1};
    /// @brief Whether there is an aggregation filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00524
    /// @needwork = dda
    /// @endcode
    bool hasAgg{false};
    /// @brief Aggregation filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00525
    /// @needwork = dda
    /// @endcode
    Aggregation agg{};
    /// @brief Whether there is a status filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00526
    /// @needwork = dda
    /// @endcode
    bool hasState{false};
    /// @brief Status filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00527
    /// @needwork = dda
    /// @endcode
    FGState state{};
    /// @brief Whether there is a threshold filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00528
    /// @needwork = dda
    /// @endcode
    bool hasThres{false};
    /// @brief Threshold filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00529
    /// @needwork = dda
    /// @endcode
    Threshold thres{};
    /// @brief Get filter chain
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00530
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Security event definition
/// @code{.isoft}
/// @unit_name=SecurityEventDefinition
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00209
/// @trace_id_dd=DD_IDSM_00531
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0004,SR_IDSM_0006,SR_IDSM_0011
/// @endcode
struct SecurityEventDefinition
{
public:
    /// @name shortName
    /// @brief Name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00532
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName{};
    /// @name fqn
    /// @brief fqn identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00533
    /// @needwork = dda
    /// @endcode
    ara::core::String fqn{};
    /// @name eventSymbolName
    /// @brief Event symbol name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00534
    /// @needwork = dda
    /// @endcode
    ara::core::String eventSymbolName{};
    /// @name id
    /// @brief Security event ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00535
    /// @needwork = dda
    /// @endcode
    uint16_t id{0};
    /// @brief Get security event definition
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00536
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Security event attributes
/// @code{.isoft}
/// @unit_name=SecurityEventContextProp
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00210
/// @trace_id_dd=DD_IDSM_00537
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0004,SR_IDSM_0006,SR_IDSM_0011
/// @endcode
struct SecurityEventContextProp
{
public:
    /// @name shortName
    /// @brief Name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00538
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName{};
    /// @name fqn
    /// @brief fqn identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00539
    /// @needwork = dda
    /// @endcode
    ara::core::String fqn{};
    /// @brief Security event reporting mode
    /// @name defaultReportingMode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00540
    /// @needwork = dda
    /// @endcode
    ara::core::String defaultReportingMode{};
    /// @name persistentStorage
    /// @brief Security event persistence switch
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00541
    /// @needwork = dda
    /// @endcode
    bool persistentStorage{true};
    /// @name securityEvent
    /// @brief fqn of security event definition
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00542
    /// @needwork = dda
    /// @endcode
    ara::core::String securityEvent{};
    /// @name sensorInstanceId
    /// @brief Sensor instance ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00543
    /// @needwork = dda
    /// @endcode
    uint16_t sensorInstanceId{1};
    /// @name severity
    /// @brief Severity level
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00544
    /// @needwork = dda
    /// @endcode
    uint16_t severity{0};
    /// @brief Get security event attributes
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00545
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Security event port mapping to process
/// @code{.isoft}
/// @unit_name=ProcessMappingPort
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00211
/// @trace_id_dd=DD_IDSM_00546
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0001
/// @endcode
struct ProcessMappingPort
{
public:
    /// @name shortName
    /// @brief Name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00547
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName{};
    /// @name fqn
    /// @brief fqn identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00548
    /// @needwork = dda
    /// @endcode
    ara::core::String fqn{};
    /// @name id
    /// @brief id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00549
    /// @needwork = dda
    /// @endcode
    uint32_t id{0};
    /// @name instanceId
    /// @brief Port descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00550
    /// @needwork = dda
    /// @endcode
    ara::core::String instanceId{};
    /// @name mappedProecess
    /// @brief Processes allowed to report to the Port
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00551
    /// @needwork = dda
    /// @endcode
    ara::core::String mappedProecess{};
    /// @brief Get mapping from event port to process
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00552
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Security event port mapping
/// @code{.isoft}
/// @unit_name=SecurityEventMappingPort
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00212
/// @trace_id_dd=DD_IDSM_00553
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0001
/// @endcode
struct SecurityEventMappingPort
{
public:
    /// @name shortName
    /// @brief Name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00554
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName;
    /// @name fqn
    /// @brief fqn identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00555
    /// @needwork = dda
    /// @endcode
    ara::core::String fqn;
    /// @name instanceId
    /// @brief Instance descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00556
    /// @needwork = dda
    /// @endcode
    ara::core::String instanceId;
    /// @name mappedEvent
    /// @brief Event type ID associated with the port
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00557
    /// @needwork = dda
    /// @endcode
    ara::core::String mappedEvent;
    /// @brief Get mapping from event port to event context prop
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00558
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Diagnostic data identifier mapping to security event attributes
/// @code{.isoft}
/// @unit_name=DidToSecurityEventContextProp
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00213
/// @trace_id_dd=DD_IDSM_00559
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0004
/// @endcode
struct DidToSecurityEventContextProp
{
public:
    /// @name did
    /// @brief Diagnostic data identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00560
    /// @needwork = dda
    /// @endcode
    uint16_t did{1};
    /// @name securityEventContextProps
    /// @brief Security event associated with the diagnostic data identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00561
    /// @needwork = dda
    /// @endcode
    ara::core::String securityEventContextProps{};
    /// @brief Get mapping from diagnostic data identifier to event context prop
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00562
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Diagnostic event mapping to security event attributes
/// @code{.isoft}
/// @unit_name=DtcToSecurityEventContextProp
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00214
/// @trace_id_dd=DD_IDSM_00563
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0011
/// @endcode
struct DtcToSecurityEventContextProp
{
public:
    /// @name did
    /// @brief Diagnostic data identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00564
    /// @needwork = dda
    /// @endcode
    uint16_t did{1};
    /// @name securityEventContextProps
    /// @brief Security event associated with the diagnostic data identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00565
    /// @needwork = dda
    /// @endcode
    ara::core::String securityEventContextProps{};
    /// @name didInstance
    /// @brief DID instance descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00566
    /// @needwork = dda
    /// @endcode
    ara::core::String didInstance{};
    /// @name monitorInstance
    /// @brief Diagnostic monitor instance descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00567
    /// @needwork = dda
    /// @endcode
    ara::core::String monitorInstance{};
    /// @brief Get diagnostic dtcserver related configuration
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00568
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief BswModule mapping to security event attributes
/// @code{.isoft}
/// @unit_name=SecurityEventContextMappingBswModule
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00215
/// @trace_id_dd=DD_IDSM_00569
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0011
/// @endcode
struct SecurityEventContextMappingBswModule
{
public:
    /// @name shortName
    /// @brief Name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00570
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName{};
    /// @name fqn
    /// @brief fqn identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00571
    /// @needwork = dda
    /// @endcode
    ara::core::String fqn{};
    /// @name affectedBswModule
    /// @brief Affected Bsw modules
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00572
    /// @needwork = dda
    /// @endcode
    ara::core::String affectedBswModule{};
    /// @name filterChain
    /// @brief Filter chain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00573
    /// @needwork = dda
    /// @endcode
    ara::core::String filterChain{};
    /// @name mappedSecurityEvent
    /// @brief Security events processed by the filter chain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00574
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > mappedSecurityEvent{};
    /// @brief Mapping relationship between BswModule's filter chain and events
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00575
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Functional cluster mapping to security event attributes
/// @code{.isoft}
/// @unit_name=SecurityEventContextMappingFunctionalCluster
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00216
/// @trace_id_dd=DD_IDSM_00576
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0011
/// @endcode
struct SecurityEventContextMappingFunctionalCluster
{
public:
    /// @name shortName
    /// @brief Name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00577
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName;
    /// @name fqn
    /// @brief fqn identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00578
    /// @needwork = dda
    /// @endcode
    ara::core::String fqn;
    /// @name affectedFunctionalCluster
    /// @brief Affected functional cluster
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00579
    /// @needwork = dda
    /// @endcode
    ara::core::String affectedFunctionalCluster;
    /// @name filterChain
    /// @brief Filter chain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00580
    /// @needwork = dda
    /// @endcode
    ara::core::String filterChain;
    /// @name mappedSecurityEvent
    /// @brief Security events processed by the filter chain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00581
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > mappedSecurityEvent;
    /// @brief Mapping relationship between FunctionalCluster's filter chain and events
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00582
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief CommConnector mapping to security event attributes
/// @code{.isoft}
/// @unit_name=SecurityEventContextMappingCommConnector
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00217
/// @trace_id_dd=DD_IDSM_00583
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0011
/// @endcode
struct SecurityEventContextMappingCommConnector
{
public:
    /// @name shortName
    /// @brief Name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00584
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName;
    /// @name fqn
    /// @brief fqn identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00585
    /// @needwork = dda
    /// @endcode
    ara::core::String fqn;
    /// @name filterChain
    /// @brief Filter chain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00586
    /// @needwork = dda
    /// @endcode
    ara::core::String filterChain;
    /// @name mappedSecurityEvent
    /// @brief Security events processed by the filter chain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00587
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > mappedSecurityEvent;
    /// @brief Mapping relationship between CommConnector's filter chain and events
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00588
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};
/// @brief Application mapping to security event attributes
/// @code{.isoft}
/// @unit_name=SecurityEventContextMappingApplication
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00218
/// @trace_id_dd=DD_IDSM_00589
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0011
/// @endcode
struct SecurityEventContextMappingApplication
{
public:
    /// @name shortName
    /// @brief Name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00590
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName;
    /// @name fqn
    /// @brief fqn identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00591
    /// @needwork = dda
    /// @endcode
    ara::core::String fqn;
    /// @name affectedApplication
    /// @brief Affected application
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00592
    /// @needwork = dda
    /// @endcode
    ara::core::String affectedApplication;
    /// @name filterChain
    /// @brief Filter chain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00593
    /// @needwork = dda
    /// @endcode
    ara::core::String filterChain;
    /// @name mappedSecurityEvent
    /// @brief Security events processed by the filter chain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00594
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > mappedSecurityEvent;
    /// @brief Mapping relationship between Application's filter chain and events
    /// @param node Configuration object to be parsed
    /// @return 0 on success, -1 on failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00595
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;
};

/// @brief Singleton class to obtain information required by Idsm for processing security events
/// @code{.isoft}
/// @unit_name=IdsmManifest
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00219
/// @trace_id_dd=DD_IDSM_00596
/// @needwork = ad
/// @trace_id_sr=SR_IDSM_0011
/// @endcode
class IdsmManifest
{
public:
    /// @brief Constructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00597
    /// @needwork = dda
    /// @endcode
    IdsmManifest() = default;

protected:
    /// @brief Copy constructor
    /// @param manifest Object to be copied
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00598
    /// @needwork = dda
    /// @endcode
    IdsmManifest(IdsmManifest const &manifest) = default;
    /// @brief Move constructor
    /// @param manifest Object to be moved
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00599
    /// @needwork = dda
    /// @endcode
    IdsmManifest(IdsmManifest &&manifest) = default;
    /// @brief Copy assignment operator
    /// @param right Object to be copied
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00600
    /// @needwork = dda
    /// @endcode
    IdsmManifest &operator=(IdsmManifest const &right) = default;
    /// @brief Move assignment operator
    /// @param right Object to be moved
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00601
    /// @needwork = dda
    /// @endcode
    IdsmManifest &operator=(IdsmManifest &&right) = default;

public:
    /// @brief Destructor
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00602
    /// @needwork = dda
    /// @endcode
    virtual ~IdsmManifest() = default;

public:
    /// @brief Parse configuration file and extract required information
    /// @param jsonFile Configuration file
    /// @return 0: success, non-zero: failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00603
    /// @needwork = dda
    /// @endcode
    int32_t Parse(ara::core::String const &jsonFile);

public:
    /// @brief Get IDSM instance
    /// @return IDSM instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00604
    /// @needwork = dda
    /// @endcode
    IdsmInstance GetIdsmInstance() noexcept { return idsmInstance_; }
    /// @brief Get security event definition
    /// @return Security event definition
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00605
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SecurityEventDefinition > GetSecurityEventDefinitions() const noexcept
    {
        return securityEventDefinitions_;
    }
    /// @brief Get security event attributes
    /// @return Security event attributes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00606
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SecurityEventContextProp > GetSecurityEventContextProps() const noexcept
    {
        return securityEventContextProps_;
    }
    /// @brief Get filter chain
    /// @return Filter chain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00607
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SecurityEventFilterChain > GetSecurityEventFilterChains() const noexcept
    {
        return securityEventFilterChains_;
    }
    /// @brief Get mapping from security event to Port
    /// @return Mapping from security event to Port
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00608
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SecurityEventMappingPort > GetSecurityEventPorts() const noexcept { return securityEventPorts_; }
    /// @brief Get mapping from Port to process
    /// @return Mapping from Port to process
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00609
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ProcessMappingPort > GetSecurityEventProcess() const noexcept { return securityEventProcess_; }
    /// @brief Get mapping from security event to diagnostic data identifier
    /// @return Mapping from security event to diagnostic data identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00610
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< DidToSecurityEventContextProp > GetDidToSecurityEventContextProps() const noexcept
    {
        return didToSecurityEventContextProps_;
    }
    /// @brief Get mapping from security event to diagnostic event
    /// @return Mapping from security event to diagnostic event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00611
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< DtcToSecurityEventContextProp > GetDtcToSecurityEventContextProps() const noexcept
    {
        return dtcToSecurityEventContextProps_;
    }
    /// @brief Get mapping from filter chain to security event
    /// @return Mapping from filter chain to security event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00612
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SecurityEventContextMappingApplication > GetSecurityEventContextMappingApplications()
        const noexcept
    {
        return securityEventContextMappingApplications_;
    }

private:
    /// @name idsmInstance_
    /// @brief IDSM instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00613
    /// @needwork = dda
    /// @endcode
    IdsmInstance idsmInstance_;
    /// @name securityEventFilterChains_
    /// @brief Filter chain
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00614
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SecurityEventFilterChain > securityEventFilterChains_;
    /// @name securityEventDefinitions_
    /// @brief Security event definition
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00615
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SecurityEventDefinition > securityEventDefinitions_;
    /// @name securityEventContextProps_
    /// @brief Security event related attributes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00616
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SecurityEventContextProp > securityEventContextProps_;
    /// @name didToSecurityEventContextProps_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00617
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< DidToSecurityEventContextProp > didToSecurityEventContextProps_;
    /// @name dtcToSecurityEventContextProps_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00618
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< DtcToSecurityEventContextProp > dtcToSecurityEventContextProps_;
    /// @name securityEventPorts_
    /// @brief Mapping relationship between port and event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00619
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SecurityEventMappingPort > securityEventPorts_;
    /// @name securityEventProcess_
    /// @brief Mapping relationship between process and port
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00620
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ProcessMappingPort > securityEventProcess_;
    /// @name securityEventContextMappingBswModules_
    /// @brief Security event mapping to Bsw module
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00621
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SecurityEventContextMappingBswModule > securityEventContextMappingBswModules_;
    /// @name securityEventContextMappingFunctionalClusters_
    /// @brief Security event mapping to functional cluster
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00622
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SecurityEventContextMappingFunctionalCluster > securityEventContextMappingFunctionalClusters_;
    /// @name securityEventContextMappingCommConnectors_
    /// @brief Security event mapping to communication connector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00623
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SecurityEventContextMappingCommConnector > securityEventContextMappingCommConnectors_;
    /// @name securityEventContextMappingApplications_
    /// @brief Security event mapping to application
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00624
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SecurityEventContextMappingApplication > securityEventContextMappingApplications_;
};

}  // namespace idsm
}  // namespace ara

#endif