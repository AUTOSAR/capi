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
/// @file       config_struct.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/config_struct.h"

#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_reader.h>

#include "ara/phm/internal/config_field.h"

namespace ara {
namespace phm {
namespace internal {
/// @brief
/// @param node
/// @return
int32_t CheckpointConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const kShortName{std::move(GetShortName())};
    int32_t ret{node.Load(kShortName, shortName)};
    MANIFEST_RET_CHECK(ret, kShortName)

    ara::core::StringView const kFqn{std::move(GetFqn())};
    ret = node.Load(kFqn, fqn);
    MANIFEST_RET_CHECK(ret, kFqn)

    ara::core::StringView const kProcessName{std::move(GetProcessName())};
    ret = node.Load(kProcessName, processName);
    MANIFEST_RET_CHECK(ret, kProcessName)

    ara::core::StringView const kIdentifierR{std::move(GetIdentifierR())};
    ret = node.Load(kIdentifierR, identifier);
    MANIFEST_RET_CHECK(ret, kIdentifierR)

    /// @brief id of checkpoint.
    ara::core::StringView const kCheckpointId{"checkpointId"};
    ret = node.Load(kCheckpointId, checkpointId);
    MANIFEST_RET_CHECK(ret, kCheckpointId)
    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t AliveSupervisionConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const kShortName{std::move(GetShortName())};
    int32_t ret{node.Load(kShortName, shortName)};
    MANIFEST_RET_CHECK(ret, kShortName)

    ara::core::StringView const kFqn{std::move(GetFqn())};
    ret = node.Load(kFqn, fqn);
    MANIFEST_RET_CHECK(ret, kFqn)

    /// @brief fqn of checkpoint.
    ara::core::StringView const kCheckpointFqn{"checkpointFqn"};
    ret = node.Load(kCheckpointFqn, checkpointFqn);
    MANIFEST_RET_CHECK(ret, kCheckpointFqn)

    /// @brief check cycle in time of alive supervision.
    ara::core::StringView const kAliveReferenceCycle{"aliveRefrenceCycle"};
    ret = node.Load(kAliveReferenceCycle, aliveRefrenceCycle);
    MANIFEST_RET_CHECK(ret, kAliveReferenceCycle)

    /// @brief expected indications of alive supervision in one period.
    ara::core::StringView const kExpectedAliveIndications{"expectedAliveIndications"};
    ret = node.Load(kExpectedAliveIndications, expectedAliveIndications);
    MANIFEST_RET_CHECK(ret, kExpectedAliveIndications)

    /// @brief max margin.
    ara::core::StringView const kMaxMargin{"maxMargin"};
    ret = node.Load(kMaxMargin, maxMargin);
    MANIFEST_RET_CHECK(ret, kMaxMargin)

    /// @brief min margin.
    ara::core::StringView const kMinMargin{"minMargin"};
    ret = node.Load(kMinMargin, minMargin);
    MANIFEST_RET_CHECK(ret, kMinMargin)

    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t DeadlineSupervisionConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const kShortName{std::move(GetShortName())};
    int32_t ret{node.Load(kShortName, shortName)};
    MANIFEST_RET_CHECK(ret, kShortName)

    ara::core::StringView const kFqn{std::move(GetFqn())};
    ret = node.Load(kFqn, fqn);
    MANIFEST_RET_CHECK(ret, kFqn)

    /// @brief checkpoint transition.
    ara::core::StringView const kCheckpointTransition{"checkpointTransition"};
    ret = node.Load(kCheckpointTransition, checkpointTransition);
    MANIFEST_RET_CHECK(ret, kCheckpointTransition)

    /// @brief max deadline.
    ara::core::StringView const kMaxDeadline{"maxDeadline"};
    ret = node.Load(kMaxDeadline, maxDeadline);
    MANIFEST_RET_CHECK(ret, kMaxDeadline)

    /// @brief min deadline.
    ara::core::StringView const kMinDeadline{"minDeadline"};
    ret = node.Load(kMinDeadline, minDeadline);
    MANIFEST_RET_CHECK(ret, kMinDeadline)

    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t TransitionConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const kShortName{std::move(GetShortName())};
    int32_t ret{node.Load(kShortName, shortName)};
    MANIFEST_RET_CHECK(ret, kShortName)

    /// @brief source checkpoint.
    ara::core::StringView const kSourceCheckpoint{"sourceCheckpoint"};
    ret = node.Load(kSourceCheckpoint, sourceCheckpoint);
    MANIFEST_RET_CHECK(ret, kSourceCheckpoint)

    /// @brief target checkpoint.
    ara::core::StringView const kTargetCheckpoint{"targetCheckpoint"};
    ret = node.Load(kTargetCheckpoint, targetCheckpoint);
    MANIFEST_RET_CHECK(ret, kTargetCheckpoint)

    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t LogicalSupervisionConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const kShortName{std::move(GetShortName())};
    int32_t ret{node.Load(kShortName, shortName)};
    MANIFEST_RET_CHECK(ret, kShortName)

    ara::core::StringView const kFqn{std::move(GetFqn())};
    ret = node.Load(kFqn, fqn);
    MANIFEST_RET_CHECK(ret, kFqn)

    /// @brief initial checkpoint.
    ara::core::StringView const kInitialCheckpoint{"initialCheckpoint"};
    ret = node.Load(kInitialCheckpoint, initialCheckpoint);
    MANIFEST_RET_CHECK(ret, kInitialCheckpoint)

    /// @brief final checkpoint.
    ara::core::StringView const kFinalCheckpoint{"finalCheckpoint"};
    ret = node.Load(kFinalCheckpoint, finalCheckpoint);
    MANIFEST_RET_CHECK(ret, kFinalCheckpoint)

    /// @brief checkpoint transition.
    ara::core::StringView const kTransition{"transition"};
    ret = node.Load(kTransition, transition);
    MANIFEST_RET_CHECK(ret, kTransition)

    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t LocalSupervisionConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const kShortName{std::move(GetShortName())};
    int32_t ret{node.Load(kShortName, shortName)};
    MANIFEST_RET_CHECK(ret, kShortName)

    ara::core::StringView const kFqn{std::move(GetFqn())};
    ret = node.Load(kFqn, fqn);
    MANIFEST_RET_CHECK(ret, kFqn)

    /// @brief faild supervision tolerance, used in local supervision.
    ara::core::StringView const kFailedSupervisionCyclesTolerance{"failedSupervisionCyclesTolerance"};
    ret = node.Load(kFailedSupervisionCyclesTolerance, failedSupervisionCyclesTolerance);
    MANIFEST_RET_CHECK(ret, kFailedSupervisionCyclesTolerance)

    ara::core::StringView const kAliveSupervision{std::move(GetAliveSupervision())};
    ret = node.Load(kAliveSupervision, aliveSupervision);
    MANIFEST_RET_CHECK(ret, kAliveSupervision)

    ara::core::StringView const kDeadlineSupervision{std::move(GetDeadlineSupervision())};
    ret = node.Load(kDeadlineSupervision, deadlineSupervision);
    MANIFEST_RET_CHECK(ret, kDeadlineSupervision)

    ara::core::StringView const kLogicalSupervision{std::move(GetLogicalSupervision())};
    ret = node.Load(kLogicalSupervision, logicalSupervision);
    MANIFEST_RET_CHECK(ret, kLogicalSupervision)

    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t GlobalSupervisionInfo::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const kShortName{std::move(GetShortName())};
    int32_t ret{node.Load(kShortName, shortName)};
    MANIFEST_RET_CHECK(ret, kShortName)

    /// @brief supervision cycle, used in global supervision.
    ara::core::StringView const kSupervisionCycle{"supervisionCycle"};
    ret = node.Load(kSupervisionCycle, supervisionCycle);
    MANIFEST_RET_CHECK(ret, kSupervisionCycle)

    /// @brief expired tolerance, used in global supervision.
    ara::core::StringView const kExpiredSupervisionCycleTolerance{"expiredSupervisonCycleTolerance"};
    ret = node.Load(kExpiredSupervisionCycleTolerance, expiredSupervisonCycleTolerance);
    MANIFEST_RET_CHECK(ret, kExpiredSupervisionCycleTolerance)

    ara::core::StringView const kLocalSupervision{std::move(GetLocalSupervision())};
    ret = node.Load(kLocalSupervision, localSupervision);
    MANIFEST_RET_CHECK(ret, kLocalSupervision)

    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t RecoverNotificationConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    /// @brief max wait time permitted when recovery.
    ara::core::StringView const kRecoveryNotificationTimeout{"recoveryNotificationTimeout"};
    int32_t ret{node.Load(kRecoveryNotificationTimeout, recoveryNotificationTimeout)};
    MANIFEST_RET_CHECK(ret, kRecoveryNotificationTimeout)

    /// @brief max retry times permitted when recovery.
    ara::core::StringView const kRecoveryNotificationRetry{"recoveryNotificationRetry"};
    ret = node.Load(kRecoveryNotificationRetry, recoveryNotificationRetry);
    MANIFEST_RET_CHECK(ret, kRecoveryNotificationRetry)

    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t GlobalSupervisionReoveryInfo::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const kShortName{std::move(GetShortName())};
    int32_t ret{node.Load(kShortName, shortName)};
    MANIFEST_RET_CHECK(ret, kShortName)

    ara::core::StringView const kMetaModelIdentifier{std::move(GetMetaModelIdentifier())};
    ret = node.Load(kMetaModelIdentifier, metaModelIdentifier);
    MANIFEST_RET_CHECK(ret, kMetaModelIdentifier)

    /// @brief the supervision.
    ara::core::StringView const kSupervision{"supervision"};
    ret = node.Load(kSupervision, globalSupervision);
    MANIFEST_RET_CHECK(ret, kSupervision)

    ara::core::StringView const kRecoveryNotification{std::move(GetRecoveryNotification())};
    ret = node.Load(kRecoveryNotification, recoveryNotification);
    MANIFEST_RET_CHECK(ret, kRecoveryNotification)

    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t ProcessInfoConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const kProcessName{std::move(GetProcessName())};
    int32_t ret{node.Load(kProcessName, processName)};
    MANIFEST_RET_CHECK(ret, kProcessName)

    /// @brief error code.
    ara::core::StringView const kExecutionError{"executionError"};
    ret = node.Load(kExecutionError, executionError);
    MANIFEST_RET_CHECK(ret, kExecutionError)

    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t OneFgStateConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const kShortName{std::move(GetShortName())};
    int32_t ret{node.Load(kShortName, stateName)};
    MANIFEST_RET_CHECK(ret, kShortName)

    /// @brief the activated phm supervision of function group state.
    ara::core::StringView const kPhmSupervision{"phmSupervision"};
    ret = node.Load(kPhmSupervision, phmSupervision);
    MANIFEST_RET_CHECK(ret, kPhmSupervision)

    /// @brief processes belongs to one function group state.
    ara::core::StringView const kProcesses{"processes"};
    ret = node.Load(kProcesses, processInfo);
    MANIFEST_RET_CHECK(ret, kProcesses)

    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t FgSupervisionModeConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const kFqn{std::move(GetFqn())};
    int32_t ret{node.Load(kFqn, fqn)};
    MANIFEST_RET_CHECK(ret, kFqn)

    /// @brief states of function group.
    ara::core::StringView const kFgState{"fgStates"};
    ret = node.Load(kFgState, fgStateConf);
    MANIFEST_RET_CHECK(ret, kFgState)

    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t HealthStatusConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const kShortName{std::move(GetShortName())};
    int32_t ret{node.Load(kShortName, shortName)};
    MANIFEST_RET_CHECK(ret, kShortName)

    /// @brief id of health status.
    ara::core::StringView const kStatusId{"statusId"};
    ret = node.Load(kStatusId, statusId);
    MANIFEST_RET_CHECK(ret, kStatusId)

    /// @brief whether the health status need recovery.
    ara::core::StringView const kTriggersRecoveryNotification{"triggersRecoveryNotification"};
    ret = node.Load(kTriggersRecoveryNotification, triggersRecoveryNotification);
    MANIFEST_RET_CHECK(ret, kTriggersRecoveryNotification)

    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t HealthChannelEntityConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const kShortName{std::move(GetShortName())};
    int32_t ret{node.Load(kShortName, shortName)};
    MANIFEST_RET_CHECK(ret, kShortName)

    /// @brief health status.
    ara::core::StringView const kHealthStatus{"healthStatus"};
    ret = node.Load(kHealthStatus, healthStatus);
    MANIFEST_RET_CHECK(ret, kHealthStatus)

    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t HealthChannelSupervisionConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    ara::core::StringView const kShortName{std::move(GetShortName())};
    int32_t ret{node.Load(kShortName, shortName)};
    MANIFEST_RET_CHECK(ret, kShortName)

    ara::core::StringView const kMetaModelIdentifier{std::move(GetMetaModelIdentifier())};
    ret = node.Load(kMetaModelIdentifier, metaModelIdentifier);
    MANIFEST_RET_CHECK(ret, kMetaModelIdentifier)

    ara::core::StringView const kRecoveryNotification{std::move(GetRecoveryNotification())};
    ret = node.Load(kRecoveryNotification, recoveryNotification);
    MANIFEST_RET_CHECK(ret, kRecoveryNotification)

    ara::core::StringView const kIdentifierR{std::move(GetIdentifierR())};
    ret = node.Load(kIdentifierR, hcInterfaceIdentifier);
    MANIFEST_RET_CHECK(ret, kIdentifierR)

    /// @brief interface of health channel.
    ara::core::StringView const kHealthChannelInterface{"phmHealthChannelInterface"};
    ret = node.Load(kHealthChannelInterface, phmHealthChannelInterface);
    MANIFEST_RET_CHECK(ret, kHealthChannelInterface)

    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t WatchdogConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    std::ignore = node;
    // nothing
    return isoft::kSuccess;
}

/// @brief
/// @param node
/// @return
int32_t RecoveryNotificationPortMappingConf::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    /// @brief instance specifier.
    ara::core::StringView const kInstanceSpecifier{"instance_specifier"};
    int32_t ret{node.Load(kInstanceSpecifier, instanceSpecifier)};
    MANIFEST_RET_CHECK(ret, kInstanceSpecifier)

    /// @brief health channel id.
    ara::core::StringView const khealthChannelId{"healthChannelId"};
    ret = node.Load(khealthChannelId, healthChannelId);
    MANIFEST_RET_CHECK(ret, khealthChannelId)

    return isoft::kSuccess;
}

}  // namespace internal
}  // namespace phm
}  // namespace ara