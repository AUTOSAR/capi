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
/// @file       idsm_manifest.cpp
/// @brief      Configuration item parsing implementation
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
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_manifest.h"

#include "idsm_manifest_name.h"
#include "isoft/manifestreader/manifest.h"
#include "log/idsm_log.h"

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00219
/// @trace_id_dd=DD_IDSM_00861
/// @needwork = dd
/// @endcode
#define MANIFEST_CHECK_PARSER(ret, fieldName)                                                                          \
    if (isoft::kSuccess != (ret)) {                                                                                    \
        LOG_ERROR << (fieldName) << "parse error, ret:" << (ret);                                                      \
        return (ret);                                                                                                  \
    }

namespace ara {
namespace idsm {

/// @brief Get IDSR network address and port number
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t NetworkInterface::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const ipAddress{"ipv4IpAddress"};
    int32_t ret{node.Load(ipAddress, ipv4IpAddress)};
    MANIFEST_CHECK_PARSER(ret, ipAddress)

    ara::core::StringView const tcpPortStr{"tcpPort"};
    ret = node.Load(tcpPortStr, tcpPort);
    MANIFEST_CHECK_PARSER(ret, tcpPortStr)
    return ret;
}
/// @brief Get IDSR network information && IDSM time domain
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t IdsmModuleInstantiation::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const networkInterface{"networkInterface"};
    int32_t ret{node.Load(networkInterface, net)};
    MANIFEST_CHECK_PARSER(ret, networkInterface)

    ara::core::StringView const timeBaseStr{"timeBase"};
    ret = node.Load(timeBaseStr, timeBase);
    MANIFEST_CHECK_PARSER(ret, timeBaseStr)
    return ret;
}
/// @brief Get IDSM rate limiting information
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t RateLimitationFilter::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const eventMaxNum{"maxEventsInInterval"};
    int32_t ret{node.Load(eventMaxNum, maxEventsInInterval)};
    MANIFEST_CHECK_PARSER(ret, eventMaxNum)

    ara::core::StringView const timeIntervalStr{ManifestNamePool::GetInstance()->GetFieldByName("TIME_INTERVAL")};
    ret = node.Load(timeIntervalStr, timeInterval);
    MANIFEST_CHECK_PARSER(ret, timeIntervalStr)
    return ret;
}
/// @brief Get IDSM traffic shaping information
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t TrafficLimitationFilter::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const bytesMaxNum{"maxBytesInInterval"};
    int32_t ret{node.Load(bytesMaxNum, maxBytesInInterva)};
    MANIFEST_CHECK_PARSER(ret, bytesMaxNum)

    ara::core::StringView const timeIntervalStr{ManifestNamePool::GetInstance()->GetFieldByName("TIME_INTERVAL")};
    ret = node.Load(timeIntervalStr, timeInterval);
    MANIFEST_CHECK_PARSER(ret, timeIntervalStr)
    return ret;
}
/// @brief Get memory limit
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
int32_t MemoryLimitation::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const memoryLimitEvent{"event"};
    int32_t ret{node.Load(memoryLimitEvent, eventMem)};
    MANIFEST_CHECK_PARSER(ret, memoryLimitEvent)

    ara::core::StringView const memoryLimitContext{"context"};
    ret = node.Load(memoryLimitContext, contextMem);
    MANIFEST_CHECK_PARSER(ret, memoryLimitContext)
    return ret;
}
/// @brief Get signature related data
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
int32_t SignatureSupportAp::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const keySlotStr{"keySlot"};
    int32_t ret{node.Load(keySlotStr, keySlot)};
    MANIFEST_CHECK_PARSER(ret, keySlotStr)

    ara::core::StringView const cryptoPrimitiveStr{"cryptoPrimitive"};
    ret = node.Load(cryptoPrimitiveStr, cryptoPrimitive);
    MANIFEST_CHECK_PARSER(ret, cryptoPrimitiveStr)
    return ret;
}
/// @brief Get IDSM instance information
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t IdsmInstance::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const shortNameStr{ManifestNamePool::GetInstance()->GetFieldByName("SHORT_NAME")};
    int32_t ret{node.Load(shortNameStr, shortName)};
    MANIFEST_CHECK_PARSER(ret, ara::core::StringView{shortNameStr})

    ara::core::StringView const fqnStr{ManifestNamePool::GetInstance()->GetFieldByName("FQN")};
    ret = node.Load(ara::core::StringView{fqnStr}, fqn);
    MANIFEST_CHECK_PARSER(ret, ara::core::StringView{fqnStr})

    ara::core::StringView const instanceId{"idsmInstanceId"};
    ret = node.Load(instanceId, idsmInstanceId);
    MANIFEST_CHECK_PARSER(ret, instanceId)

    ara::core::StringView const signSupportCpStr{"signatureSupportCp"};
    ret = node.Load(signSupportCpStr, signatureSupportCp);
    /// @details The field signatureSupportCp does not exist in the N model, it is a useless field in the implementation
    // MANIFEST_CHECK_PARSER(ret, signSupportCpStr)

    ara::core::StringView const timestampFormatStr{"timestampFormat"};
    ret = node.Load(timestampFormatStr, timestampFormat);
    MANIFEST_CHECK_PARSER(ret, timestampFormatStr)

    ara::core::StringView const moduleInstantiation{"idsmModuleInstantiation"};
    ret = node.Load(moduleInstantiation, moduleInstall);
    MANIFEST_CHECK_PARSER(ret, moduleInstantiation)

    ara::core::StringView const rateLimitFilter{"rateLimitationFilter"};
    ret = node.Load(rateLimitFilter, rateLimitationFilter);
    MANIFEST_CHECK_PARSER(ret, rateLimitFilter)

    ara::core::StringView const trafficLimitFilter{"trafficLimitationFilter"};
    ret = node.Load(trafficLimitFilter, trafficLimitationFilter);
    MANIFEST_CHECK_PARSER(ret, trafficLimitFilter)

    ara::core::StringView const memoryLimit{"memoryLimitation"};
    ret = node.Load(memoryLimit, memLimitation);
    MANIFEST_CHECK_PARSER(ret, memoryLimit)

    ara::core::StringView const signSupportApStr{"signatureSupportAp"};
    ret = node.Load(signSupportApStr, signAp);
    MANIFEST_CHECK_PARSER(ret, signSupportApStr)
    return ret;
}
/// @brief Get aggregation filter
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t Aggregation::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const contextDataSourceStr{"contextDataSource"};
    int32_t ret{node.Load(contextDataSourceStr, contextDataSource)};
    MANIFEST_CHECK_PARSER(ret, contextDataSourceStr)

    ara::core::StringView const intervalLength{"minimumIntervalLength"};
    ret = node.Load(intervalLength, minimumIntervalLength);
    MANIFEST_CHECK_PARSER(ret, intervalLength)
    return ret;
}
/// @brief Get threshold filter
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t Threshold::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const intervalLengthStr{"intervalLength"};
    int32_t ret{node.Load(intervalLengthStr, intervalLength)};
    MANIFEST_CHECK_PARSER(ret, intervalLengthStr)

    ara::core::StringView const thresholdNmuStr{"thresholdNumber"};
    ret = node.Load(thresholdNmuStr, thresholdNumber);
    MANIFEST_CHECK_PARSER(ret, thresholdNmuStr)
    return ret;
}
/// @brief Get function group and function group status
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t FunctionGroupState::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const modeDeclarationGrpPort{"modeDeclarationGroupPrototype"};
    int32_t ret{node.Load(modeDeclarationGrpPort, modeDeclarationGroup)};
    MANIFEST_CHECK_PARSER(ret, modeDeclarationGrpPort)

    ara::core::StringView const modeDeclarationStr{"modeDeclaration"};
    ret = node.Load(modeDeclarationStr, modeDeclaration);
    MANIFEST_CHECK_PARSER(ret, modeDeclarationStr)
    return ret;
}
/// @brief Get status filter
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t FGState::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const blockIfStateActiveApStr{"blockIfStateActiveAp"};
    int32_t const ret{node.Load(blockIfStateActiveApStr, blockIfStateActiveAp)};
    MANIFEST_CHECK_PARSER(ret, blockIfStateActiveApStr)
    return ret;
}
/// @brief Get filter chain
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t SecurityEventFilterChain::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const fqnStr{ManifestNamePool::GetInstance()->GetFieldByName("FQN")};
    int32_t const ret{node.Load(fqnStr, fqn)};
    MANIFEST_CHECK_PARSER(ret, fqnStr)

    ara::core::StringView const stateStr{"state"};
    int32_t const stateErr{node.Load(stateStr, state)};
    if (stateErr == isoft::kSuccess) {
        hasState = true;
    } else {
        hasState = false;
    }
    ara::core::StringView const oneEveryNStr{"oneEveryN"};
    int32_t const sampleErr{node.Load(oneEveryNStr, oneEveryN)};
    if (sampleErr == isoft::kSuccess) {
        hasOneEveryN = true;
    } else {
        hasOneEveryN = false;
    }

    ara::core::StringView const aggregationStr{"aggregation"};
    int32_t const aggErr{node.Load(aggregationStr, agg)};
    if (aggErr == isoft::kSuccess) {
        hasAgg = true;
    } else {
        hasAgg = false;
    }

    ara::core::StringView const thresholdStr{"threshold"};
    int32_t const thresErr{node.Load(thresholdStr, thres)};
    if (thresErr == isoft::kSuccess) {
        hasThres = true;
    } else {
        hasThres = false;
    }

    if ((sampleErr != isoft::kSuccess) && (aggErr != isoft::kSuccess) && (stateErr != isoft::kSuccess)
        && (thresErr != isoft::kSuccess)) {
        LOG_WARN << "idsm filter chain is empty. please verify configuration.";
    }
    return isoft::kSuccess;
}
/// @brief Get security event definition
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t SecurityEventDefinition::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const shortNameStr{ManifestNamePool::GetInstance()->GetFieldByName("SHORT_NAME")};
    int32_t ret{node.Load(shortNameStr, shortName)};
    MANIFEST_CHECK_PARSER(ret, shortNameStr)

    ara::core::StringView const fqnStr{ManifestNamePool::GetInstance()->GetFieldByName("FQN")};
    ret = node.Load(fqnStr, fqn);
    MANIFEST_CHECK_PARSER(ret, fqnStr)

    ara::core::StringView const eventSymbolNameStr{"eventSymbolName"};
    ret = node.Load(eventSymbolNameStr, eventSymbolName);
    /// @details The field eventSymbolName does not exist in the N model, it is a useless field in the implementation
    // MANIFEST_CHECK_PARSER(ret, eventSymbolNameStr)

    ara::core::StringView const idStr{ManifestNamePool::GetInstance()->GetFieldByName("ID")};
    ret = node.Load(idStr, id);
    MANIFEST_CHECK_PARSER(ret, idStr)
    return ret;
}
/// @brief Get security event attributes
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t SecurityEventContextProp::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const shortNameStr{ManifestNamePool::GetInstance()->GetFieldByName("SHORT_NAME")};
    int32_t ret{node.Load(shortNameStr, shortName)};
    MANIFEST_CHECK_PARSER(ret, shortNameStr)

    ara::core::StringView const fqnStr{ManifestNamePool::GetInstance()->GetFieldByName("FQN")};
    ret = node.Load(fqnStr, fqn);
    MANIFEST_CHECK_PARSER(ret, fqnStr)

    ara::core::StringView const defaultReportingModeStr{"defaultReportingMode"};
    ret = node.Load(defaultReportingModeStr, defaultReportingMode);
    MANIFEST_CHECK_PARSER(ret, defaultReportingModeStr)

    ara::core::StringView const persistentStorageStr{"persistentStorage"};
    ret = node.Load(persistentStorageStr, persistentStorage);
    MANIFEST_CHECK_PARSER(ret, persistentStorageStr)

    ara::core::StringView const securityEventStr{"securityEvent"};
    ret = node.Load(securityEventStr, securityEvent);
    MANIFEST_CHECK_PARSER(ret, securityEventStr)

    ara::core::StringView const sensorInstanceIdStr{"sensorInstanceId"};
    ret = node.Load(sensorInstanceIdStr, sensorInstanceId);
    MANIFEST_CHECK_PARSER(ret, sensorInstanceIdStr)

    ara::core::StringView const severityStr{"severity"};
    ret = node.Load(severityStr, severity);
    MANIFEST_CHECK_PARSER(ret, severityStr)
    return ret;
}
/// @brief Get mapping from event port to process
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t ProcessMappingPort::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const shortNameStr{ManifestNamePool::GetInstance()->GetFieldByName("SHORT_NAME")};
    int32_t ret{node.Load(shortNameStr, shortName)};
    MANIFEST_CHECK_PARSER(ret, shortNameStr)

    ara::core::StringView const fqnStr{ManifestNamePool::GetInstance()->GetFieldByName("FQN")};
    ret = node.Load(fqnStr, fqn);
    MANIFEST_CHECK_PARSER(ret, fqnStr)

    ara::core::StringView const idStr{ManifestNamePool::GetInstance()->GetFieldByName("ID")};
    ret = node.Load(idStr, id);
    /// @details The field id does not exist in the N model, it is a useless field in the implementation
    // MANIFEST_CHECK_PARSER(ret, idStr)

    ara::core::StringView const processStr{"process"};
    ret = node.Load(processStr, mappedProecess);
    MANIFEST_CHECK_PARSER(ret, processStr)

    ara::core::StringView const eventPortInstance{
        ManifestNamePool::GetInstance()->GetFieldByName("EVENT_PORT_INSTANCE")};
    ret = node.Load(eventPortInstance, instanceId);
    MANIFEST_CHECK_PARSER(ret, eventPortInstance)
    return ret;
}
/// @brief Get mapping from event port to event context prop
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t SecurityEventMappingPort::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const shortNameStr{ManifestNamePool::GetInstance()->GetFieldByName("SHORT_NAME")};
    int32_t ret{node.Load(shortNameStr, shortName)};
    MANIFEST_CHECK_PARSER(ret, shortNameStr)

    ara::core::StringView const fqnStr{ManifestNamePool::GetInstance()->GetFieldByName("FQN")};
    ret = node.Load(fqnStr, fqn);
    MANIFEST_CHECK_PARSER(ret, fqnStr)

    ara::core::StringView const eventPortInstance{
        ManifestNamePool::GetInstance()->GetFieldByName("EVENT_PORT_INSTANCE")};
    ret = node.Load(eventPortInstance, instanceId);
    MANIFEST_CHECK_PARSER(ret, eventPortInstance)

    ara::core::StringView const securityEventDefinition{"securityEventDefinition"};
    ret = node.Load(securityEventDefinition, mappedEvent);
    MANIFEST_CHECK_PARSER(ret, securityEventDefinition)
    return ret;
}
/// @brief Get mapping from diagnostic data identifier to event context prop
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t DidToSecurityEventContextProp::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const didStr{ManifestNamePool::GetInstance()->GetFieldByName("DID")};
    int32_t ret{node.Load(didStr, did)};
    MANIFEST_CHECK_PARSER(ret, didStr)

    ara::core::StringView const eventContextPropStr{
        ManifestNamePool::GetInstance()->GetFieldByName("SECURITY_EVENT_CONTEXT_PROPS_FQN")};
    ret = node.Load(eventContextPropStr, securityEventContextProps);
    MANIFEST_CHECK_PARSER(ret, eventContextPropStr)
    return ret;
}
/// @brief Get diagnostic dtcserver related configuration
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t DtcToSecurityEventContextProp::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const didStr{ManifestNamePool::GetInstance()->GetFieldByName("DID")};
    int32_t ret{node.Load(didStr, did)};
    MANIFEST_CHECK_PARSER(ret, didStr)

    ara::core::StringView const eventContextPropStr{
        ManifestNamePool::GetInstance()->GetFieldByName("SECURITY_EVENT_CONTEXT_PROPS_FQN")};
    ret = node.Load(eventContextPropStr, securityEventContextProps);
    MANIFEST_CHECK_PARSER(ret, eventContextPropStr)

    ara::core::StringView const didInstanceStr{"didInstance"};
    ret = node.Load(didInstanceStr, didInstance);
    MANIFEST_CHECK_PARSER(ret, didInstanceStr)

    ara::core::StringView const monitorInstanceStr{"monitorInstance"};
    ret = node.Load(monitorInstanceStr, monitorInstance);
    MANIFEST_CHECK_PARSER(ret, monitorInstanceStr)
    return ret;
}
/// @brief Mapping relationship between BswModule's filter chain and events
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t SecurityEventContextMappingBswModule::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const shortNameStr{ManifestNamePool::GetInstance()->GetFieldByName("SHORT_NAME")};
    int32_t ret{node.Load(shortNameStr, shortName)};
    MANIFEST_CHECK_PARSER(ret, shortNameStr)

    ara::core::StringView const fqnStr{ManifestNamePool::GetInstance()->GetFieldByName("FQN")};
    ret = node.Load(fqnStr, fqn);
    MANIFEST_CHECK_PARSER(ret, fqnStr)

    ara::core::StringView const affectedBswModuleStr{"affectedBswModule"};
    ret = node.Load(affectedBswModuleStr, affectedBswModule);
    MANIFEST_CHECK_PARSER(ret, affectedBswModuleStr)

    ara::core::StringView const filterChainStr{ManifestNamePool::GetInstance()->GetFieldByName("FILTER_CHAIN")};
    ret = node.Load(filterChainStr, filterChain);
    MANIFEST_CHECK_PARSER(ret, filterChainStr)

    ara::core::StringView const mappedSecurityEventStr{
        ManifestNamePool::GetInstance()->GetFieldByName("MAPPED_SECURITY_EVENT")};
    ret = node.Load(mappedSecurityEventStr, mappedSecurityEvent);
    MANIFEST_CHECK_PARSER(ret, mappedSecurityEventStr)
    return ret;
}
/// @brief Mapping relationship between FunctionalCluster's filter chain and events
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t SecurityEventContextMappingFunctionalCluster::ManifestLoader(
    isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const shortNameStr{ManifestNamePool::GetInstance()->GetFieldByName("SHORT_NAME")};
    int32_t ret{node.Load(shortNameStr, shortName)};
    MANIFEST_CHECK_PARSER(ret, shortNameStr)

    ara::core::StringView const fqnStr{ManifestNamePool::GetInstance()->GetFieldByName("FQN")};
    ret = node.Load(fqnStr, fqn);
    MANIFEST_CHECK_PARSER(ret, fqnStr)

    ara::core::StringView const affectedFunctionalClusterStr{"affectedFunctionalCluster"};
    ret = node.Load(affectedFunctionalClusterStr, affectedFunctionalCluster);
    MANIFEST_CHECK_PARSER(ret, affectedFunctionalClusterStr)

    ara::core::StringView const filterChainStr{ManifestNamePool::GetInstance()->GetFieldByName("FILTER_CHAIN")};
    ret = node.Load(filterChainStr, filterChain);
    MANIFEST_CHECK_PARSER(ret, filterChainStr)

    ara::core::StringView const mappedSecurityEventStr{
        ManifestNamePool::GetInstance()->GetFieldByName("MAPPED_SECURITY_EVENT")};
    ret = node.Load(mappedSecurityEventStr, mappedSecurityEvent);
    MANIFEST_CHECK_PARSER(ret, mappedSecurityEventStr)
    return ret;
}
/// @brief Mapping relationship between CommConnector's filter chain and events
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t SecurityEventContextMappingCommConnector::ManifestLoader(
    isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const shortNameStr{ManifestNamePool::GetInstance()->GetFieldByName("SHORT_NAME")};
    int32_t ret{node.Load(shortNameStr, shortName)};
    MANIFEST_CHECK_PARSER(ret, shortNameStr)

    ara::core::StringView const fqnStr{ManifestNamePool::GetInstance()->GetFieldByName("FQN")};
    ret = node.Load(fqnStr, fqn);
    MANIFEST_CHECK_PARSER(ret, fqnStr)

    ara::core::StringView const filterChainStr{ManifestNamePool::GetInstance()->GetFieldByName("FILTER_CHAIN")};
    ret = node.Load(filterChainStr, filterChain);
    MANIFEST_CHECK_PARSER(ret, filterChainStr)

    ara::core::StringView const mappedSecurityEventStr{
        ManifestNamePool::GetInstance()->GetFieldByName("MAPPED_SECURITY_EVENT")};
    ret = node.Load(mappedSecurityEventStr, mappedSecurityEvent);
    MANIFEST_CHECK_PARSER(ret, mappedSecurityEventStr)
    return ret;
}
/// @brief Mapping relationship between Application's filter chain and events
/// @param node Configuration object to be parsed
/// @return 0 on success, -1 on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t SecurityEventContextMappingApplication::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const shortNameStr{ManifestNamePool::GetInstance()->GetFieldByName("SHORT_NAME")};

    int32_t ret{node.Load(shortNameStr, shortName)};
    MANIFEST_CHECK_PARSER(ret, shortNameStr)

    ara::core::StringView const fqnStr{ManifestNamePool::GetInstance()->GetFieldByName("FQN")};
    ret = node.Load(fqnStr, fqn);
    MANIFEST_CHECK_PARSER(ret, fqnStr)

    ara::core::StringView const affectedApplicationStr{"affectedApplication"};
    ret = node.Load(affectedApplicationStr, affectedApplication);
    /// @details The field affectedApplication does not exist in the N model, it is a useless field in the implementation
    // MANIFEST_CHECK_PARSER(ret, affectedApplicationStr)

    ara::core::StringView const filterChainStr{ManifestNamePool::GetInstance()->GetFieldByName("FILTER_CHAIN")};
    ret = node.Load(filterChainStr, filterChain);
    MANIFEST_CHECK_PARSER(ret, filterChainStr)

    ara::core::StringView const mappedSecurityEventStr{
        ManifestNamePool::GetInstance()->GetFieldByName("MAPPED_SECURITY_EVENT")};
    ret = node.Load(mappedSecurityEventStr, mappedSecurityEvent);
    MANIFEST_CHECK_PARSER(ret, mappedSecurityEventStr)
    return ret;
}
/// @brief Parse configuration file and extract required information
/// @param jsonFile Configuration file
/// @return 0: success, non-zero: failure
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t IdsmManifest::Parse(ara::core::String const& jsonFile)
{
    LOG_INFO << "ManifestParser begin parse file=" << jsonFile.c_str();
    ara::core::StringView const filtPath{jsonFile.c_str()};
    using ManifestPtr = std::unique_ptr< isoft::manifestreader::Manifest >;
    ara::core::Result< ManifestPtr > openResult{isoft::manifestreader::OpenManifest(filtPath)};
    if (!openResult.HasValue()) {
        LOG_ERROR << "ManifestParser::DoParse Error in opening json file: " << jsonFile.c_str()
                  << " errmsg=" << openResult.Error().Message();
        return 1;
    }

    ManifestPtr const manifest{std::move(openResult).Value()};
    LOG_INFO << "begin parse";

    ara::core::StringView const idsmInstanceStr{"idsmInstance"};
    ara::core::StringView const securityEventFilterChainStr{"securityEventFilterChains"};
    ara::core::StringView const securityEventDefinitionStr{"securityEventDefinitions"};
    ara::core::StringView const securityEventContextPropStr{"securityEventContextProps"};
    ara::core::StringView const securityEventReportedMappingPortStr{"securityEventReportedMappingPort"};
    ara::core::StringView const securityEventReportedMappingProcessStr{"securityEventReportedMappingProcess"};
    ara::core::StringView const didSecurityEventContextPropStr{"didToSecurityEventContextProps"};
    ara::core::StringView const dtcToSecurityEventContextPropStr{"dtcToSecurityEventContextProps"};
    ara::core::StringView const securityEventContextMappingBswModuleStr{"securityEventContextMappingBswModules"};
    ara::core::StringView const securityEventContextMappingFunctionalClusterStr{
        "securityEventContextMappingFunctionalClusters"};
    ara::core::StringView const securityEventContextMappingCommConnectorStr{
        "securityEventContextMappingCommConnectors"};
    ara::core::StringView const securityEventContextMappingApplicationStr{"securityEventContextMappingApplications"};
    int32_t ret{manifest->Load(idsmInstanceStr, idsmInstance_)};
    MANIFEST_CHECK_PARSER(ret, idsmInstanceStr)

    ret = manifest->Load(securityEventFilterChainStr, securityEventFilterChains_);
    MANIFEST_CHECK_PARSER(ret, securityEventFilterChainStr)

    ret = manifest->Load(securityEventDefinitionStr, securityEventDefinitions_);
    MANIFEST_CHECK_PARSER(ret, securityEventDefinitionStr)

    ret = manifest->Load(securityEventContextPropStr, securityEventContextProps_);
    MANIFEST_CHECK_PARSER(ret, securityEventContextPropStr)

    ret = manifest->Load(securityEventReportedMappingPortStr, securityEventPorts_);
    MANIFEST_CHECK_PARSER(ret, securityEventReportedMappingPortStr)

    ret = manifest->Load(securityEventReportedMappingProcessStr, securityEventProcess_);
    MANIFEST_CHECK_PARSER(ret, securityEventReportedMappingProcessStr)

    ret = manifest->Load(didSecurityEventContextPropStr, didToSecurityEventContextProps_);
    MANIFEST_CHECK_PARSER(ret, didSecurityEventContextPropStr)

    ret = manifest->Load(dtcToSecurityEventContextPropStr, dtcToSecurityEventContextProps_);
    MANIFEST_CHECK_PARSER(ret, dtcToSecurityEventContextPropStr)

    ret = manifest->Load(securityEventContextMappingBswModuleStr, securityEventContextMappingBswModules_);
    /// @details The field securityEventContextMappingBswModules does not exist in the N model, it is a useless field in the implementation
    // MANIFEST_CHECK_PARSER(ret, securityEventContextMappingBswModuleStr)

    ret = manifest->Load(securityEventContextMappingFunctionalClusterStr,
                         securityEventContextMappingFunctionalClusters_);
    /// @details The field securityEventContextMappingFunctionalClusters does not exist in the N model, it is a useless field in the implementation
    // MANIFEST_CHECK_PARSER(ret, securityEventContextMappingFunctionalClusterStr)

    ret = manifest->Load(securityEventContextMappingCommConnectorStr, securityEventContextMappingCommConnectors_);
    /// @details The field securityEventContextMappingCommConnectors does not exist in the N model, it is a useless field in the implementation
    // MANIFEST_CHECK_PARSER(ret, securityEventContextMappingCommConnectorStr)

    ret = manifest->Load(securityEventContextMappingApplicationStr, securityEventContextMappingApplications_);
    MANIFEST_CHECK_PARSER(ret, securityEventContextMappingApplicationStr)
    LOG_INFO << "begin end";
    return ret;
}

}  // namespace idsm
}  // namespace ara