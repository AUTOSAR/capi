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
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/config_manager.h"

#include <ara/core/string_view.h>

#include "ara/phm/internal/config_field.h"
#include "ara/phm/internal/phm_log.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Load the conf file and parse it.
/// @return 0, success; < 0, fail.
int32_t ConfigManager::Parse() noexcept
{
    LOG_INFO << "ConfigManager::Parse start.";

    ara::core::StringView const filtPath{kConfigPath.c_str()};
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > jsonHandle{
        isoft::manifestreader::OpenManifest(filtPath)};
    if (!jsonHandle.HasValue()) {
        LOG_ERROR << "ConfigManager::Parse, open json file error " << kConfigPath.c_str()
                  << ", error msg:" << jsonHandle.Error().Message().data();
        return -1;
    }

    int32_t ret{0};
    std::unique_ptr< isoft::manifestreader::Manifest > const manifest{std::move(jsonHandle).Value()};
    /// @brief phm contributes.
    ara::core::StringView const kContributes{"contributes"};
    int32_t const itArrayRet{std::move(manifest->IterateArray(
        kContributes, [&ret, this](std::size_t, isoft::manifestreader::ManifestNode const& contributeNode) noexcept {
            if (0 == ret) {
                ret = this->_parsePhmContribute(contributeNode);
            }
        }))};
    if (isoft::kSuccess != itArrayRet) {
        LOG_ERROR << "ConfigManager::Parse, manifest->IterateArray error(" << itArrayRet << ")";
        return -1;
    }

    if (ret != 0) {
        LOG_ERROR << "ConfigManager::Parse, _parsePhmContribute error(" << ret << ")";
        return -1;
    }

    if (_parseSupervisionMode(*manifest) != 0) {
        LOG_ERROR << "parse supervision mode error";
        return -1;
    }

    if (_parseRecoveryNotificationPortMapping(*manifest) != 0) {
        LOG_ERROR << "parse recovery notification mapping error";
        return -1;
    }

    if (_parseWatchdogConf(*manifest) != 0) {
        LOG_ERROR << "parse watchdog error";
        return -1;
    }

    LOG_INFO << "ConfigManager::Parse end.";
    return 0;
}

/// @brief Parse phm contribute.
/// @param contributeNode The json node.
/// @return 0 success; < 0 failed.
int32_t ConfigManager::_parsePhmContribute(isoft::manifestreader::ManifestNode const& contributeNode) noexcept
{
    if (_parseCheckpoint(contributeNode) != 0) {
        LOG_ERROR << "parse checkpoint error";
        return -1;
    }

    if (_parseAliveSupervision(contributeNode) != 0) {
        LOG_ERROR << "parse alive supervision error";
        return -1;
    }

    if (_parseDeadlineSupervision(contributeNode) != 0) {
        LOG_ERROR << "parse deadline supervision error";
        return -1;
    }

    if (_parseLogicalSupervision(contributeNode) != 0) {
        LOG_ERROR << "parse logical supervision error";
        return -1;
    }

    if (_parseLocalSupervision(contributeNode) != 0) {
        LOG_ERROR << "parse local supervision error";
        return -1;
    }

    if (_parseGlobalSupervision(contributeNode) != 0) {
        LOG_ERROR << "parse global supervision error";
        return -1;
    }

    if (_parseHealthChannel(contributeNode) != 0) {
        LOG_ERROR << "parse health channel error";
        return -1;
    }

    if (_parseHealthChannelSupervision(contributeNode) != 0) {
        LOG_ERROR << "parse health channel supervision error";
        return -1;
    }

    return 0;
}

/// @brief Parse conf of checkpoint.
/// @param manifestNode The json node.
/// @return 0 success; < 0 failed.
int32_t ConfigManager::_parseCheckpoint(isoft::manifestreader::ManifestNode const& manifestNode) noexcept
{
    ara::core::Vector< CheckpointConf > tmp;

    /// @brief checkpoints.
    ara::core::StringView const kSupervisionCheckpoint{"supervisionCheckpoint"};
    int32_t const ret{manifestNode.Load(kSupervisionCheckpoint, tmp)};
    MANIFEST_RET_CHECK(ret, kSupervisionCheckpoint)

    // TODO(wangyanlong): Will it affect performance?
    // Because there may be multiple contributes, we cannot directly set checkpointConf_ during parsing, otherwise later ones will overwrite previous content
    std::ignore = checkpointConf_.insert(checkpointConf_.cend(), tmp.begin(), tmp.end());
    return 0;
}

/// @brief Parse conf of alive supervision.
/// @param manifestNode The json node.
/// @return 0 success; < 0 failed.
int32_t ConfigManager::_parseAliveSupervision(isoft::manifestreader::ManifestNode const& manifestNode) noexcept
{
    ara::core::Vector< AliveSupervisionConf > tmp;

    ara::core::StringView const kAliveSupervision{std::move(GetAliveSupervision())};
    int32_t const ret{manifestNode.Load(kAliveSupervision, tmp)};
    MANIFEST_RET_CHECK(ret, kAliveSupervision)
    ara::core::Vector< AliveSupervisionConf >::const_iterator const pos{aliveSupervisionConf_.cend()};
    std::ignore = aliveSupervisionConf_.insert(pos, tmp.begin(), tmp.end());
    return 0;
}

/// @brief Parse conf of deadline supervision.
/// @param manifestNode The json node.
/// @return 0 success; < 0 failed.
int32_t ConfigManager::_parseDeadlineSupervision(isoft::manifestreader::ManifestNode const& manifestNode) noexcept
{
    ara::core::Vector< DeadlineSupervisionConf > tmp;

    ara::core::StringView const kDeadlineSupervision{std::move(GetDeadlineSupervision())};
    int32_t const ret{manifestNode.Load(kDeadlineSupervision, tmp)};
    MANIFEST_RET_CHECK(ret, kDeadlineSupervision)
    ara::core::Vector< DeadlineSupervisionConf >::const_iterator const pos{deadlineSupervisionConf_.cend()};
    std::ignore = deadlineSupervisionConf_.insert(pos, tmp.begin(), tmp.end());
    return 0;
}

/// @brief Parse conf of logical supervision.
/// @param manifestNode The json node.
/// @return 0 success; < 0 failed.
int32_t ConfigManager::_parseLogicalSupervision(isoft::manifestreader::ManifestNode const& manifestNode) noexcept
{
    ara::core::Vector< LogicalSupervisionConf > tmp;

    ara::core::StringView const kLogicalSupervision{std::move(GetLogicalSupervision())};
    int32_t const ret{manifestNode.Load(kLogicalSupervision, tmp)};
    MANIFEST_RET_CHECK(ret, kLogicalSupervision)
    ara::core::Vector< LogicalSupervisionConf >::const_iterator const pos{logicalSupervisionConf_.cend()};
    std::ignore = logicalSupervisionConf_.insert(pos, tmp.begin(), tmp.end());
    return 0;
}

/// @brief Parse conf of local supervision.
/// @param manifestNode The json node.
/// @return 0 success; < 0 failed.
int32_t ConfigManager::_parseLocalSupervision(isoft::manifestreader::ManifestNode const& manifestNode) noexcept
{
    ara::core::Vector< LocalSupervisionConf > tmp;

    ara::core::StringView const kLocalSupervision{std::move(GetLocalSupervision())};
    int32_t const ret{manifestNode.Load(kLocalSupervision, tmp)};
    MANIFEST_RET_CHECK(ret, kLocalSupervision)
    ara::core::Vector< LocalSupervisionConf >::const_iterator const pos{localSupervisionConf_.cend()};
    std::ignore = localSupervisionConf_.insert(pos, tmp.begin(), tmp.end());
    return 0;
}

/// @brief Parse conf of global supervision.
/// @param manifestNode The json node.
/// @return 0 success; < 0 failed.
int32_t ConfigManager::_parseGlobalSupervision(isoft::manifestreader::ManifestNode const& manifestNode) noexcept
{
    ara::core::Vector< GlobalSupervisionInfo > globalSupervisionInfo;

    /// @brief global supervision.
    ara::core::StringView const kGlobalSupervision{"globalSupervision"};
    int32_t ret{manifestNode.Load(kGlobalSupervision, globalSupervisionInfo)};
    MANIFEST_RET_CHECK(ret, kGlobalSupervision)

    ara::core::Vector< GlobalSupervisionReoveryInfo > globalSupervisionRecoveryInfo;
    /// @brief health channel supervision.
    ara::core::StringView const kHealthChannelSupervision{"healthChannelSupervision"};
    ret = manifestNode.Load(kHealthChannelSupervision, globalSupervisionRecoveryInfo);
    MANIFEST_RET_CHECK(ret, kHealthChannelSupervision)

    ara::core::Map< ara::core::String, GlobalSupervisionReoveryInfo > recoveryInfoMap;
    for (GlobalSupervisionReoveryInfo& element : globalSupervisionRecoveryInfo) {
        recoveryInfoMap[element.globalSupervision] = element;
    }

    for (GlobalSupervisionInfo& element : globalSupervisionInfo) {
        GlobalSupervisionConf conf;
        conf.shortName                        = element.shortName;
        conf.metaModelIdentifier              = recoveryInfoMap[conf.shortName].metaModelIdentifier;
        conf.supervisionCycle                 = element.supervisionCycle;
        conf.expiredSupervisionCycleTolerance = element.expiredSupervisonCycleTolerance;
        conf.localSupervision                 = element.localSupervision;
        if (recoveryInfoMap.count(conf.shortName) > static_cast< size_t >(0)) {
            conf.isSupervised = true;
            conf.recoveryNotificationRetry
                = recoveryInfoMap[conf.shortName].recoveryNotification.recoveryNotificationRetry;
            conf.recoveryNotificationTimeout
                = recoveryInfoMap[conf.shortName].recoveryNotification.recoveryNotificationTimeout;
        } else {
            LOG_WARN << conf.shortName.c_str() << " not configured recovery param.";
            conf.recoveryNotificationRetry   = 0U;
            conf.recoveryNotificationTimeout = 0;
            conf.isSupervised                = false;
        }
        globalSupervisionConf_.push_back(conf);
    }

    return 0;
}

/// @brief Parse conf of health channel.
/// @param manifestNode The json node.
/// @return 0 success; < 0 failed.
int32_t ConfigManager::_parseHealthChannel(isoft::manifestreader::ManifestNode const& manifestNode) noexcept
{
    ara::core::Vector< HealthChannelEntityConf > tmp;

    /// @brief health channel.
    ara::core::StringView const kHealthChannelPoint{"healthChannelPoint"};
    int32_t const ret{manifestNode.Load(kHealthChannelPoint, tmp)};
    MANIFEST_RET_CHECK(ret, kHealthChannelPoint)
    ara::core::Vector< HealthChannelEntityConf >::const_iterator const pos{healthChannelConf_.cend()};
    std::ignore = healthChannelConf_.insert(pos, tmp.begin(), tmp.end());
    return 0;
}

/// @brief Parse conf of health channel supervision.
/// @param manifestNode The json node.
/// @return 0 success; < 0 failed.
int32_t ConfigManager::_parseHealthChannelSupervision(isoft::manifestreader::ManifestNode const& manifestNode) noexcept
{
    ara::core::Vector< HealthChannelSupervisionConf > tmp;

    /// @brief health channel external status.
    ara::core::StringView const kHealthChannelExternalStatus{"healthChannelExternalStatus"};
    int32_t const ret{manifestNode.Load(kHealthChannelExternalStatus, tmp)};
    MANIFEST_RET_CHECK(ret, kHealthChannelExternalStatus)
    std::ignore = healthChannelSupervisionConf_.insert(healthChannelSupervisionConf_.cend(), tmp.begin(), tmp.end());
    return 0;
}

/// @brief Parse conf of supervision mode.
/// @param manifestNode The json node.
/// @return 0 success; < 0 failed.
int32_t ConfigManager::_parseSupervisionMode(isoft::manifestreader::ManifestNode const& manifestNode) noexcept
{
    ara::core::Vector< FgSupervisionModeConf > tmp;
    /// @brief supervision mode.
    ara::core::StringView const kSupervisionMode{"supervisionMode"};
    int32_t const ret{manifestNode.Load(kSupervisionMode, tmp)};
    MANIFEST_RET_CHECK(ret, kSupervisionMode)
    ara::core::Vector< FgSupervisionModeConf >::const_iterator const pos{supervisionModeConf_.cend()};
    std::ignore = supervisionModeConf_.insert(pos, tmp.begin(), tmp.end());
    return 0;
}

/// @brief Parse conf of recovery notification mapping.
/// @param manifestNode The json node.
/// @return 0 success; < 0 failed.
int32_t ConfigManager::_parseRecoveryNotificationPortMapping(
    isoft::manifestreader::ManifestNode const& manifestNode) noexcept
{
    ara::core::Vector< RecoveryNotificationPortMappingConf > tmp;

    /// @brief mapping of recovery notification to pport prototype.
    ara::core::StringView const kRecoveryNotificationMapping{"recovery_notification_to_pport_prototype_mapping"};
    int32_t const ret{manifestNode.Load(kRecoveryNotificationMapping, tmp)};
    MANIFEST_RET_CHECK(ret, kRecoveryNotificationMapping)
    ara::core::Vector< RecoveryNotificationPortMappingConf >::const_iterator const pos{
        recoveryNotificationMapConf_.cend()};
    std::ignore = recoveryNotificationMapConf_.insert(pos, tmp.begin(), tmp.end());
    return 0;
}

/// @brief Parse conf of watchdog.
/// @param manifestNode The json node.
/// @return 0 success; < 0 failed.
int32_t ConfigManager::_parseWatchdogConf(isoft::manifestreader::ManifestNode const& manifestNode) noexcept
{
    double watchdogTimeoutSecond{0.0};

    /// @brief timeout of watchdog.
    ara::core::StringView const kWatchdogTimeout{"watchdogTimeout"};
    int32_t ret{manifestNode.Load(kWatchdogTimeout, watchdogTimeoutSecond)};
    watchdogConf_.watchdogTimeoutMs = SECOND_TO_MS(watchdogTimeoutSecond);

    // If no watchdog is configured, do not return directly, but use the default timeout
    if (isoft::kSuccess != ret) {
        watchdogConf_.watchdogTimeoutMs = kOsWatchdogMinTimeoutMs;
    }
    // Minimum watchdog timeout
    if (watchdogConf_.watchdogTimeoutMs < kOsWatchdogMinTimeoutMs) {
        watchdogConf_.watchdogTimeoutMs = kOsWatchdogMinTimeoutMs;
    }

    /// @brief os watchdog.
    ara::core::StringView const kOsWatchdog{"osWatchdog"};
    ret = manifestNode.Load(kOsWatchdog, watchdogConf_.osWatchdog);
    if (isoft::kSuccess != ret) {
        watchdogConf_.osWatchdog = "";
        watchdogConf_.enable     = false;
    } else {
        watchdogConf_.enable = true;
    }
    return 0;
}

/// @brief Returns the conf of checkpoint.
/// @return the conf of checkpoint.
ara::core::Vector< CheckpointConf > const& ConfigManager::GetCheckpointConf() const noexcept { return checkpointConf_; }

/// @brief Returns the conf of alive supervision.
/// @return the conf of alive supervision.
ara::core::Vector< AliveSupervisionConf > const& ConfigManager::GetAliveSupervisionConf() const noexcept
{
    return aliveSupervisionConf_;
}

/// @brief Returns the conf of deadline supervision.
/// @return the conf of deadline supervision.
ara::core::Vector< DeadlineSupervisionConf > const& ConfigManager::GetDeadlineSupervisionConf() const noexcept
{
    return deadlineSupervisionConf_;
}

/// @brief Returns the conf of logical supervision.
/// @return the conf of logical supervision.
ara::core::Vector< LogicalSupervisionConf > const& ConfigManager::GetLogicalSupervisionConf() const noexcept
{
    return logicalSupervisionConf_;
}

/// @brief Returns the conf of local supervision
/// @return the conf of local supervision.
ara::core::Vector< LocalSupervisionConf > const& ConfigManager::GetLocalSupervisionConf() const noexcept
{
    return localSupervisionConf_;
}

/// @brief Returns the conf of global supervision
/// @return the conf of global supervision.
ara::core::Vector< GlobalSupervisionConf > const& ConfigManager::GetGlobalSupervisionConf() const noexcept
{
    return globalSupervisionConf_;
}

/// @brief Returns the conf of fg supervision conf.
/// @return the conf of fg supervision conf.
ara::core::Vector< FgSupervisionModeConf > const& ConfigManager::GetSupervisionModeConf() const noexcept
{
    return supervisionModeConf_;
}

/// @brief Returns the conf of health channel.
/// @return the conf of health channel
ara::core::Vector< HealthChannelEntityConf > const& ConfigManager::GetHealthChannelConf() const noexcept
{
    return healthChannelConf_;
}

/// @brief Returns the conf of health channel supervision.
/// @return the conf of health channel supervision.
ara::core::Vector< HealthChannelSupervisionConf > const& ConfigManager::GetHealthChannelSupervisionConf() const noexcept
{
    return healthChannelSupervisionConf_;
}

/// @brief Returns the conf of watchdog
/// @return the conf of watchdog.
WatchdogConf const& ConfigManager::GetWatchdogConf() const noexcept { return watchdogConf_; }

/// @brief Returns the conf of recovery notification mapping.
/// @return the conf of recovery notification mappint.
ara::core::Vector< RecoveryNotificationPortMappingConf > const& ConfigManager::GetRecoveryNotificationMap()
    const noexcept
{
    return recoveryNotificationMapConf_;
}

}  // namespace internal
}  // namespace phm
}  // namespace ara