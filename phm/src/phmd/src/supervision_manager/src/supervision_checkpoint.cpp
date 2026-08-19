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
/// @file       supervision_checkpoint.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/svmanager/supervision_checkpoint.h"

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
/// @brief Manage supervision checkpoint.
SupervisionCheckpoint::SupervisionCheckpoint(CheckpointConf const& conf) noexcept
    : kShortName{conf.shortName}
    , kIdentifier{conf.identifier}
    , kFqn{conf.fqn}
    , kCheckpointId{conf.checkpointId}
    , kProcessName{conf.processName}
    , processClusterAffiliation_{ara::phm::internal::svcom::ProcessClusterAffiliation::kOther}
{
    LOG_INFO << "create checkpoint, name " << kShortName.c_str() << ", fqn " << kFqn.c_str() << ", identifier "
             << kIdentifier.c_str() << ", checkpoint id " << kCheckpointId << ", process name " << kProcessName.c_str();
}

/// @brief Returns name of checkpoint.
/// @return name of checkpoint.
ara::core::String SupervisionCheckpoint::GetShortName() const noexcept { return kShortName; }

/// @brief Returns identifier of checkpoint.
/// @return identifier of checkpoint.
Specifier SupervisionCheckpoint::GetIdentifier() const noexcept { return kIdentifier; }

/// @brief Returns checkpoint id.
/// @return checkpoint id.
CheckpointId SupervisionCheckpoint::GetCheckpointId() const noexcept { return kCheckpointId; }

/// @brief Returns the name of process checkpoint belongs to.
/// @return the name of process checkpoint belongs to.
ara::core::String SupervisionCheckpoint::GetProcessName() const noexcept { return kProcessName; }

/// @brief set process cluster affiliation.
/// @param processClusterAffiliation process cluster affiliation.
/// @trace_id_sr=SR_PHM_01001
/// @needwork = no
void SupervisionCheckpoint::SetProcessClusterAffiliation(
    ara::phm::internal::svcom::ProcessClusterAffiliation const processClusterAffiliation) noexcept
{
    processClusterAffiliation_ = processClusterAffiliation;
}

/// @brief returns process cluster affiliation.
/// @return process cluster affiliation.
/// @trace_id_sr=SR_PHM_01001
/// @needwork = no
ara::phm::internal::svcom::ProcessClusterAffiliation SupervisionCheckpoint::GetProcessClusterAffiliation()
    const noexcept
{
    return processClusterAffiliation_;
}

/// @brief Returns the fqn of checkpoint.
/// @return the fqn of checkpoint.
ara::core::String SupervisionCheckpoint::GetFqn() const noexcept { return kFqn; }

/// @brief == operator of SupervisionCheckpoint.
/// @param left left object
/// @param right right object
/// @return true, equal;false, not equal
bool operator==(SupervisionCheckpoint const& left, SupervisionCheckpoint const& right) noexcept
{
    return ((left.GetIdentifier() == right.GetIdentifier()) && (right.GetCheckpointId() == right.GetCheckpointId()));
}

/// @brief != operator of SupervisionCheckpoint.
/// @param left left object
/// @param right right object
/// @return true, not equal;false, equal
bool operator!=(SupervisionCheckpoint const& left, SupervisionCheckpoint const& right) noexcept
{
    return !(left == right);
}

/// @brief < operator of SupervisionCheckpoint.
/// @param left left object
/// @param right right object
/// @return true, left < right;false, left >= right
bool operator<(SupervisionCheckpoint const& left, SupervisionCheckpoint const& right) noexcept
{
    if (left.GetIdentifier() != right.GetIdentifier()) {
        return left.GetIdentifier() < right.GetIdentifier();
    }
    return left.GetCheckpointId() < right.GetCheckpointId();
}

}  // namespace internal
}  // namespace phm
}  // namespace ara
